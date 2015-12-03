<?php
/**
 * @file
 * Contains implementation class of organisation JSON reader.
 *
 * This reader handles the JSON object hierarchy. Each item represents an
 * organisation which may or may not contain list of childrens.
 *
 * For allowing reading each item (including children), we need to probide
 * ability iterate on each parent and children and provide potential parent
 * information along with the organisation object.
 *
 * This is a snipped example of what type JSON we expect:
 * {
 *   "id": 0,
 *   "code": "HY",
 *   "name_fi": "Helsingin yliopisto (HY)",
 *   "name_en": "University of Helsinki (HY)",
 *   "name_sv": "Helsingfors universitet (HY)",
 *   "type": "yritys / yhteiso",
 *   "abbreviation": "HY",
 *   "children": [
 *     {
 *       "id": 743,
 *       "code": "H92",
 *       "name_fi": "HY, Helsingin yliopiston tutkijakoulut (HYTK)",
 *       "name_en": "HY, Doctoral Schools (HYTK)",
 *       "name_sv": "HY, Forskarskolor (HYTK)",
 *       "type": "tiedekunnan kaltainen,koontiyksikko",
 *       "abbreviation": "HYTK",
 *       "billing_code": "A92400",
 *       "children": []
 *     },
 *     {
 *       "id": 3140,
 *       "code": "H01",
 *       "name_fi": "HY, Keskushallinto (KEHA)",
 *       "name_en": "HY, Central Administration (KEHA)",
 *       "name_sv": "HY, Centralförvaltningen (KEHA)",
 *       "type": "tiedekunnan kaltainen",
 *       "abbreviation": "KEHA",
 *       "billing_code": "A01000",
 *       "children": [
 *         {
 *           "id": 4462,
 *           "code": "H021",
 *           "name_fi": "KEHA, kvestuuri (KVEST)",
 *           "name_en": "KEHA, Finance (KVEST)",
 *           "name_sv": "KEHA, kvesturen (KVEST)",
 *           "type": "toimiala",
 *           "abbreviation": "KVEST",
 *           "billing_code": "A02100",
 *           "children": []
 *         }
 *       ]
 *     }
 *   ]
 * }
 *
 * @license GPL, or GNU General Public License, version 3
 * @license http://opensource.org/licenses/GPL-3.0
 * @see README.md how to contribute to this project
 */

/**
 * Class OrganisationJSONReader
 */
class OrganisationJSONReader extends MigrateJSONReader {

  /**
   * Holds the readable items. If it's NULL, they have not been processed,
   * otherwise it's an list of items (empty or populated).
   * @var NULL|array
   */
  protected $organisations;

  /**
   * Implementation of Iterator::next().
   *
   * This implementation fetches items and aggregate them when needed so that we
   * can loop each children and figure out each parent properly.
   *
   * This iterator does not implement parsing same way MigrateJSONReader, making
   * it consume more memory. We could use here an nice lightweight parsing
   * technique which supports reading each item's children too while parsing
   * items.
   *
   * @return void
   */
  public function next() {
    migrate_instrument_start('OrganisationJSONReader::next');

    // Construct current element and ID
    $this->currentElement = $this->currentId = NULL;

    // If we don't have items fetched and aggregated, then do it
    if (!is_array($this->organisations)) {

      // We never fetched this, lets fetch the root object now
      if (!($organisation = $this->fetchOrganisation())) {
        // When FALSE, something went wrong while fetching.
        return;
      }

      // Construct items as array and flatten the organisation
      $this->organisations = array();
      self::flattenStructure($this->organisations, $organisation);

    }

    // Take next item from the aggregated items
    $this->currentElement = array_shift($this->organisations);
    if (!is_null($this->currentElement)) {
      $this->currentId = $this->currentElement->{$this->idField};
    }

    migrate_instrument_stop('OrganisationJSONReader::next');
  }

  /**
   * Flattens the structure recurssively.
   *
   * @param $organisations
   * @param $organisation
   * @param null $parent_id
   */
  static function flattenStructure(&$organisations, $organisation, $parent_id = NULL) {

    // Copy children if exists and then unset them for push
    $children = array();
    if (isset($organisation->id) && !empty($organisation->children)) {
      $children = $organisation->children;
      unset($organisation->children);
    }

    // Now push this specific organisation with given parent id
    $organisation->parent_id = $parent_id;
    array_push($organisations, $organisation);

    // Now deal with the children if any
    if (!empty($children)) {
      foreach ($children as $child) {
        self::flattenStructure($organisations, $child, $organisation->id);
      }
    }

  }

  /**
   * Fetches organisation JSON from the source URL. Returns FALSE if not able to
   * fetch and decode.
   * @return bool|object
   */
  protected function fetchOrganisation() {
    $content = file_get_contents($this->url);
    if (($json = json_decode($content))) {
      return $json;
    }
    else {
      // We couldn't decode the content as json
      Migration::displayMessage(t('!url was not able to be fetched and/or JSON-decoded.',
        array('!url' => $this->url)));
    }
    return FALSE;
  }

}
