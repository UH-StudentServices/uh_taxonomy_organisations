<?php
/**
 * @license GPL, or GNU General Public License, version 3
 * @license http://opensource.org/licenses/GPL-3.0
 * @see README.md how to contribute to this project
 */

/**
 * Class OrganisationTaxonomyMigrateDestinationTerm
 *
 * This should handle entity translation better.
 */
class OrganisationTaxonomyMigrateDestinationTerm extends MigrateDestinationTerm {

  protected $translation_handler;

  /**
   * Saves translations for all enabled langauges.
   *
   * @param $entity
   * @param stdClass $source_row
   */
  public function prepare($entity, stdClass $source_row) {
    parent::prepare($entity, $source_row);

    $this->translation_handler = entity_translation_get_handler('taxonomy_term', $entity);
    $this->translation_handler->setOriginalLanguage($entity->language);
    foreach (locale_language_list('language') as $langcode) {
      // Specify our translation
      $translation = array(
        'translate' => 0,
        'status' => 1,
        'language' => $langcode,
      );

      // Set the source language of the translation
      if ($entity->language != $langcode) {
        $translation['source'] = $entity->language;
      }
    }

    $this->translation_handler->setTranslation($translation, $entity);
  }

  /**
   * Override MigrateDestinationNode::complete(), so that translations gets set
   * during self::prepare(), but the translations should be saved after entity
   * has been saved.
   *
   * Save translations that was setup during $this->prepare().
   *
   * @param $entity
   * @param stdClass $source_row
   */
  public function complete($entity, stdClass $source_row) {
    parent::complete($entity, $source_row);
    if (isset($this->translation_handler)) {
      $this->translation_handler->saveTranslations();
    }
  }
}
