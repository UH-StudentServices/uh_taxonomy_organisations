<?php
/**
 * @file
 * uh_taxonomy_organisations_et.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function uh_taxonomy_organisations_et_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'taxonomy_term-organisations_of_university_of_helsinki-name_field'
  $field_instances['taxonomy_term-organisations_of_university_of_helsinki-name_field'] = array(
    'bundle' => 'organisations_of_university_of_helsinki',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 7,
      ),
    ),
    'entity_type' => 'taxonomy_term',
    'field_name' => 'name_field',
    'label' => 'Name',
    'required' => TRUE,
    'settings' => array(
      'entity_translation_sync' => FALSE,
      'hide_label' => array(
        'entity' => FALSE,
        'page' => FALSE,
      ),
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => -5,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Name');

  return $field_instances;
}
