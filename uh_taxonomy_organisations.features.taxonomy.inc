<?php
/**
 * @file
 * uh_taxonomy_organisations.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function uh_taxonomy_organisations_taxonomy_default_vocabularies() {
  return array(
    'organisations_of_university_of_helsinki' => array(
      'name' => 'Organisations of University of Helsinki',
      'machine_name' => 'organisations_of_university_of_helsinki',
      'description' => 'Contains complete list of organisations in University of Helsinki.',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
  );
}
