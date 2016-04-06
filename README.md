# University of Helsinki, Organisations

This is a Drupal 7 module for providing taxonomy vocabulary of organisations of [University of Helsinki](http://www.helsinki.fi).

## New in 7.x-1.x
* Migrate: Added support for cleaning up orphan organisation terms after import
* Migrate: Use track_changes feature, to avoid repeatedly import same content
* Migrate: Added possibility to use variable to define JSON source path
* Migrate: Fixed migration error "Taxonomy term name is required" when english
  name for organisation is missing

See more changes in [CHANGELOG.md](CHANGELOG.md).

## Getting Started
We encourage you to participate in this open source project. Please make Pull
Requests, Bug Reports, ideas, (security) code reviews or any kind of positive
contribution.

Before adding suggestions to the main source repository we will send you a
agreement to give to ownership of the code to [University of Helsinki](http://www.helsinki.fi).
That way we can keep the ownership of the main trunk code clear and have the
possibility to change the open source license if needed.

### Examples
```php
// Load open university taxonomy term and print out its billing code
$term = uh_taxonomy_organisations_load_by_code('H930');
echo $term->field_ouh_billing_code[LANGUAGE_NONE][0]['value'];
```

## Submodules
Main module contains two optional submodules.

### University of Helsinki, Organisations ET
Brings support for using entity translation / field translations on taxonomy
terms.

### University of Helsinki, Organisations Migrate
Allows you to import the organisations from the JSON source. Enable module and
register the migration, then perform import.

You have option to also perform an cleanup after import if you're planning to
perform import periodacally by altering the migration definition:

```php
/**
 * Implements hook_migrate_api_alter().
 */
function MYMODULE_migrate_api_alter(array &$info) {
  if (isset($info['uh_taxonomy_organisations_migrate']['migrations']['OrganisationsUniversityHelsinkiTaxonomy'])) {
    $info['uh_taxonomy_organisations_migrate']['migrations']['OrganisationsUniversityHelsinkiTaxonomy']['arguments']['perform_post_cleanup'] = TRUE;
  }
}
```

Note that you might need to re-register migration if alteration gets in place
after original registration.

## Questions
Please post your question to doo-projekti@helsinki.fi

## License
This software is developed under [GPL v3](LICENSE.txt).
