# Release 2.x-dev (development release)

## New features since last release

## Improvements

RegionSelectionManager Object should not longer leak memory

## Bug fixes

Fixed destructor in RegionSelectionManager so that RegionSelection objects allocated inside the region_ vector are properly destructed upon existing scope/destruction of RegionSelectionManager.


## Contributors

This release contains contributions from (in alphabetical order):

Jack Araz, Kyle Fan, Benjamin Fuks
