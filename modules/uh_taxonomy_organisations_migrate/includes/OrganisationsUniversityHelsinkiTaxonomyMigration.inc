<?php
/**
 * @file
 * This file contains migration class implementation for importing organisations
 * as taxonomy terms.
 *
 * @license GPL, or GNU General Public License, version 3
 * @license http://opensource.org/licenses/GPL-3.0
 * @see README.md how to contribute to this project
 */

/**
 * Class OrganisationsUniversityHelsinkiTaxonomyMigration
 */
class OrganisationsUniversityHelsinkiTaxonomyMigration extends Migration {

  const CLEANUP_NONE = 0;
  const CLEANUP_MINIMAL = 1;
  const CLEANUP_FULL = 0;

  /**
   * Determines the behaviour after import has been completed.
   *
   * Type of post cleanup that we want to perform:
   *   CLEANUP_NONE:    Default. No cleanup will be done after import.
   *   CLEANUP_MINIMAL: Only orphan terms that got created in the vocabulary
   *                    without import will get deleted after import.
   *   CLEANUP_FULL:    Same as CLEANUP_MINIMAL + all terms that does not exist
   *                    anymore in the source JSON.
   *
   * @var self::CLEANUP_NONE|self::CLEANUP_MINIMAL|self::CLEANUP_FULL
   */
  private $perform_post_cleanup;

  /**
   * Configuration of the migration that enables threshold filtering. This can
   * be given as argument of the migration. To enable this feature,
   * perform_post_cleanup option must be set to self::CLEANUP_FULL.
   *
   * @var bool
   */
  private $threshold_filtering_enabled;

  public function __construct($arguments = array()) {
    parent::__construct();

    // Specify how we want to perform an cleanup of organisation terms after
    // import has been done.
    $this->perform_post_cleanup = isset($arguments['perform_post_cleanup']) ? $arguments['perform_post_cleanup'] : self::CLEANUP_NONE;

    // Specify if we want to do threshold filtering determining whether to
    // perform the clean up or not.
    $this->threshold_filtering_enabled = (isset($arguments['threshold_filtering_enabled']) && $arguments['threshold_filtering_enabled'] === TRUE);

    // It doesn't make sense to enable threshold filtering if post cleanup isn't
    // set to self::CLEANUP_FULL.
    if ($this->perform_post_cleanup !== self::CLEANUP_FULL && $this->threshold_filtering_enabled === TRUE) {
      // See self::postImport()
      throw new LogicException('Enabling threshold filtering without performing full post cleanup, does not make sense.');
    }

    // Describe this migration, mapping table, source and destination
    $this->description = 'Migrates "Organisations of University of Helsinki" terms from REST API backend.';
    $this->map = new MigrateSQLMap(
      $this->machineName,
      array(
        'code' => array(
          'type' => 'varchar',
          'length' => '32',
          'not null' => TRUE,
          'description' => 'Organisation code',
        )
      ),
      MigrateDestinationTerm::getKeySchema()
    );
    $this->source = new MigrateSourceJSON(
      variable_get('uh_taxonomy_organisations_migrate_source_path', 'https://od.helsinki.fi/orgrek/opetus.json'),
      'code',
      array(
        'id' => 'Organisation ID',
        'code' => 'Organisation code',
        'name_en' => 'Name in english',
        'type' => 'Type of organisation',
        'abbreviation' => 'Abbreviation of organisation',
        'billing_code' => 'Billing code of organisation',
        'parent_code' => 'Parent code of organisation this belongs to.',
      ),
      array('reader_class' => 'OrganisationJSONReader', 'track_changes' => TRUE)
    );
    $this->destination = new TaxonomyMigrateDestinationTerm('organisations_of_university_of_helsinki');

    // Set field mapping
    $this->addFieldMapping('name', 'name_en')->defaultValue('Untitled');
    if (module_exists('uh_taxonomy_organisations_et')) {
      // Create multilingual field mapping when uh_taxonomy_organisations_et
      // module is enabled. The source field 'name' will be computed in
      // prepareRow() method.
      $this->addFieldMapping('name_field', 'name')->arguments(array('language' => self::getEnabledLanguages()));
    }
    $this->addFieldMapping('field_ouh_code', 'code');
    $this->addFieldMapping('field_ouh_code:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_ouh_type', 'type');
    $this->addFieldMapping('field_ouh_type:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_ouh_abbreviation', 'abbreviation');
    $this->addFieldMapping('field_ouh_abbreviation:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_ouh_billing_code', 'billing_code');
    $this->addFieldMapping('field_ouh_billing_code:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('parent', 'parent_code')->sourceMigration($this->getMachineName());

    // These fields are not migrated
    $this->addUnmigratedDestinations(array('name_fi', 'name_sv', 'description', 'parent_name', 'format', 'weight', 'path'));

  }

  /**
   * Prepare row by creating multilingual fields from given preset returned by
   * self::getMultingualFields().
   *
   * @param $row
   *  Object containing raw source data.
   * @return bool
   *  TRUE to process this row, FALSE to have the source skip it.
   * @see self::getMultingualFields()
   * @see self::getEnabledLanguages()
   */
  public function prepareRow($row) {
    if (module_exists('uh_taxonomy_organisations_et')) {
      foreach (self::getMultilingualFields() as $field) {
        $row->$field = array();
        foreach (self::getEnabledLanguages() as $language) {
          $source_field_name = self::getMultilingualSourceFieldName($field, $language);
          $row->{$field}[$language] = NULL;
          if (isset($row->$source_field_name)) {
            $row->{$field}[$language] = $row->$source_field_name;
          }
        }
      }
    }
    return parent::prepareRow($row);
  }

  /**
   * Override of Migration::postImport()
   */
  public function postImport() {
    parent::postImport();

    // If we perform an minimal cleanup (or full), then handle unidentified
    // terms. They are terms that can't be found from mapping table.
    if ($this->perform_post_cleanup === self::CLEANUP_MINIMAL ||
        $this->perform_post_cleanup === self::CLEANUP_FULL) {
      foreach ($this->getUnmappedTerms() as $tid) {
        taxonomy_term_delete($tid);
      }
    }

    // If we perform an full cleanup, then handle orphans. Orphan terms are
    // terms that were created by import, but does not anymore exist in the
    // source.
    if ($this->perform_post_cleanup === self::CLEANUP_FULL) {
      $orphan_tids = $this->getOrphanTerms();
      $deletable_count = count($orphan_tids);
      if ($this->getThresholdFilteringEnabled() === TRUE && $this->exceedsThreshold($deletable_count)) {
        watchdog('uh_taxonomy_organisations_migrate', 'Deletable count @count exceeded the threshold. Cancelling taxonomy term deletion.', array('@count' => $deletable_count), WATCHDOG_WARNING);
      }
      else {
        foreach ($orphan_tids as $tid) {
          taxonomy_term_delete($tid);
        }
      }
    }
  }

  /**
   * Returns the value of threshold_filtering_enabled argument.
   *
   * @return bool
   */
  private function getThresholdFilteringEnabled() {
    return $this->threshold_filtering_enabled;
  }

  /**
   * Determines from the $deletable_count that is it exceeding the threshold.
   *
   * An example of truth table, where C1 representing the count of organisations
   * that source has available and D1 representing the deletable count. Result
   * is representing whether the threshold is exceeded.
   *
   * +------+------+--------+
   * | C1   | D1   | RESULT |
   * +------+------+--------+
   * | 100  | 30   | FALSE  |
   * | 200  | 30   | FALSE  |
   * | 320  | 30   | FALSE  |
   * | 500  | 200  | FALSE  |
   * | 400  | 200  | FALSE  |
   * | 200  | 200  | TRUE   |
   * | 500  | 300  | TRUE   |
   * | 500  | 1000 | TRUE   |
   * | 10   | 1000 | TRUE   |
   * | 10   | 30   | TRUE   |
   * | 1    | 3    | TRUE   |
   * | 4    | 1    | FALSE  |
   * | 1    | 1    | FALSE  |
   * | 0    | 1    | TRUE   |
   * +------+------+--------+
   *
   * @param $deletable_count
   * @return bool
   * @throws Exception
   */
  private function exceedsThreshold($deletable_count) {

    // We should not use this method if deletable count is not an integer
    if (!is_int($deletable_count)) {
      throw new InvalidArgumentException('Deletable count must be an integer.');
    }

    // We should not use this feature if we can't get count out from the source
    $count = $this->sourceCount();
    if (!is_numeric($count) || $count < 0) {
      throw new Exception('Could not determine the count');
    }

    // If needs updating more than half of organisations, then we exceed the
    // threshold.
    $threshold = round($count/2);

    return $threshold < $deletable_count;
  }

  /**
   * Gets orphan terms from destination vocabulary, that has been previously
   * imported, but was not anymore in the source.
   *
   * We detect this by checking which terms needs still updating.
   *
   * Note: It is important that you call this *AFTER* the import has been
   * completed. Otherwise you'll get term ids also that needs update but exists
   * in the source. @todo how to make this safer?
   *
   * @return array
   */
  private function getOrphanTerms() {
    $tids = array();

    // Get all items that needs updating, but also ensure that they actually do
    // not exist, before returning the list.
    $maximum_count = variable_get('uh_taxonomy_organisations_migrate_needs_update_max_count', 1000);
    $rows = $this->getMap()->getRowsNeedingUpdate($maximum_count);
    if (count($rows) > 0) {

      // Tids that needs updating
      $destids = array_map(function($row) {
        return $row->destid1;
      }, $rows);

      // Load terms that needs update and get list of tids of them
      $found_terms = taxonomy_term_load_multiple($destids);
      $tids = array_keys($found_terms);
    }
    return $tids;
  }

  /**
   * Returns set of term IDs that exists in the vocabulary, but has no map row
   * entry. This can happen for many reasons, like when creating the term
   * manually from UI or by getting created by another migration.
   *
   * @return array
   */
  private function getUnmappedTerms() {
    $tids = array();
    $vocabulary = taxonomy_vocabulary_machine_name_load($this->destination->getBundle());
    if (!empty($vocabulary->vid)) {
      $terms = taxonomy_get_tree($vocabulary->vid);
      foreach ($terms as $term) {
        $found = $this->getMap()->getRowByDestination(array($term->tid));
        if (!$found) {
          $tids[] = $term->tid;
        }
      }
    }
    return $tids;
  }

  /**
   * Returns a collection of enabled languages when locale module is enabled. If
   * module isn't enabled, then return array having one language (active
   * language).
   *
   * @return array
   */
  static function getEnabledLanguages() {
    if (function_exists('locale_language_list')) {
      return array_values(locale_language_list('language'));
    }
    global $language;
    return array($language->language);
  }

  /**
   * Assembles an expected field name for given field and language.
   *
   * Examples of expected returns:
   * +----------------+------------+------------------+
   * | $field         | $langauge  | return           |
   * +----------------+------------+------------------+
   * | 'name'         | 'fi'       | 'name_fi'        |
   * | 'name'         | 'sv'       | 'name_sv'        |
   * | 'description'  | 'fi'       | 'description_fi' |
   * | 'some'         | 'en'       | 'some_en'        |
   * +----------------+------------+------------------+
   *
   * @param $field
   * @param $language
   * @return string
   */
  static function getMultilingualSourceFieldName($field, $language) {
    return $field . '_' . $language;
  }

  /**
   * Gets list of supported multilingual fields. At the time of writing, it's
   * only one field "name".
   *
   * @return array
   */
  static function getMultilingualFields() {
    return array('name');
  }
}
