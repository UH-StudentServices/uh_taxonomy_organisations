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
  // TODO: ...
}
