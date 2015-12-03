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
      array('name_en', 'name_fi', 'name_sv', 'code', 'type', 'abbreviation', 'billing_code', 'parent'),
      array('reader_class' => 'OrganisationJSONReader')
    );
    $this->destination = new TaxonomyMigrateDestinationTerm('organisations_of_university_of_helsinki');

    // Set field mapping
    $this->addFieldMapping('name', 'name_en');
    $this->addFieldMapping('field_ouh_code', 'code');
    $this->addFieldMapping('field_ouh_type', 'type');
    $this->addFieldMapping('field_ouh_abbreviation', 'abbreviation');
    $this->addFieldMapping('field_ouh_billing_code', 'billing_code');

    // These fields are not migrated
    $this->addUnmigratedDestinations(array('description'));

  }
}
