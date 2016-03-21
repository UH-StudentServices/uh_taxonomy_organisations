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
  public function __construct() {
    parent::__construct();

    // Describe this migration, mapping table, source and destination
    $this->description = 'Migrates "Organisations of University of Helsinki" terms from REST API backend.';
    $this->map = new MigrateSQLMap(
      $this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Organisation ID',
        )
      ),
      MigrateDestinationTerm::getKeySchema()
    );
    $this->source = new MigrateSourceJSON(
      variable_get('uh_taxonomy_organisations_migrate_source_path', 'https://od.helsinki.fi/orgrek/opetus.json'),
      'id',
      array(
        'id' => 'Organisation ID',
        'code' => 'Organisation code',
        'name_en' => 'Name in english',
        'type' => 'Type of organisation',
        'abbreviation' => 'Abbreviation of organisation',
        'billing_code' => 'Billing code of organisation',
        'parent_id' => 'Parent ID of organisation this belongs to.',
      ),
      array('reader_class' => 'OrganisationJSONReader', 'track_changes' => TRUE)
    );
    $this->destination = new TaxonomyMigrateDestinationTerm('organisations_of_university_of_helsinki');

    // Set field mapping
    $this->addFieldMapping('name', 'name_en');
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
    $this->addFieldMapping('parent', 'parent_id')->sourceMigration($this->getMachineName());

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
