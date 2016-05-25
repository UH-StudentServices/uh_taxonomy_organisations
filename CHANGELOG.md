# Changelog

## 7.x-dev
* Migrate: Fixed missing destination class (issue #2)
* Migrate: Added support for minimal cleaning up after import
* Migrate: Added support for full cleaning up after import
* Migrate: Added support for thresholding full cleanup
* Migrate: Use track_changes feature, to avoid repeatedly import same content
* Migrate: Added possibility to use variable to define JSON source path
* Migrate: Fixed migration error "Taxonomy term name is required" when english
  name for organisation is missing
* Migrate: Use organisation codes for mapping purposes (issue #3)

## 7.x-1.1
* Added function for loading taxonomy terms by given organisation code, designed
  to be used by another modules
* Added support for using taxonomy terms as entity translated entities

## 7.x-1.0
* Added main feature module which contains the vocabulary and the fields
* Added submodule which imports whole organisation hierarchy using migrate module
