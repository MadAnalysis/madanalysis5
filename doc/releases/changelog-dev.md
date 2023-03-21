# Release 2.x-dev (development release)

## New features since last release

## Improvements

## Bug fixes

 * Fixed destructor in `RegionSelectionManager` so that `RegionSelection` 
   objects allocated inside the `region_vector` are properly destructed upon 
   existing `scope/destruction` of `RegionSelectionManager`.
   ([#113](https://github.com/MadAnalysis/madanalysis5/pull/113))


## Contributors

This release contains contributions from (in alphabetical order):

[Kyle Fan](https://github.com/kfan326)
