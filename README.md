# University of Helsinki, Organisations

This is a Drupal 7 module for providing taxonomy vocabulary of organisations of [University of Helsinki](http://www.helsinki.fi).

## Latest changes
* Added function for loading taxonomy terms by given organisation code, designed
  to be used by another modules
* Added support for using taxonomy terms as entity translated entities

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

## Questions
Please post your question to doo-projekti@helsinki.fi

## License
This software is developed under [GPL v3](LICENSE.txt).
