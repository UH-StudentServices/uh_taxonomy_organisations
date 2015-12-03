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
      'https://od.helsinki.fi/orgrek/opetus.json',
      'id',
      array(
        'code' => 'Organisation code',
        'name_en' => 'Name in english',
        'type' => 'Type of organisation',
        'abbreviation' => 'Abbreviation of organisation',
        'billing_code' => 'Billing code of organisation',
        'parent_id' => 'Parent ID of organisation this belongs to.',
      ),
      array('reader_class' => 'OrganisationJSONReader')
    );
    $this->destination = new TaxonomyMigrateDestinationTerm('organisations_of_university_of_helsinki');

    // Set field mapping
    $this->addFieldMapping('name', 'name_en');
    $this->addFieldMapping('field_ouh_code', 'code');
    $this->addFieldMapping('field_ouh_code:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_ouh_type', 'type');
    $this->addFieldMapping('field_ouh_type:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_ouh_abbreviation', 'abbreviation');
    $this->addFieldMapping('field_ouh_abbreviation:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_ouh_billing_code', 'billing_code');
    $this->addFieldMapping('field_ouh_billing_code:language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // These fields are not migrated
    $this->addUnmigratedDestinations(array('name_fi', 'name_sv', 'description', 'parent_name', 'format', 'weight', 'path'));

  }
}
