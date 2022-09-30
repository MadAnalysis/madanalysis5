# Release 2.x-dev (development release)

## New features since last release

## Improvements

 * Ability to read and store length and energy information has been implemented
   for particle propagator module.
   ([#140](https://github.com/MadAnalysis/madanalysis5/pull/140))

## Bug fixes

 * Fixed destructor in `RegionSelectionManager` so that `RegionSelection` 
   objects allocated inside the `region_vector` are properly destructed upon 
   existing `scope/destruction` of `RegionSelectionManager`.
   ([#113](https://github.com/MadAnalysis/madanalysis5/pull/113))

 * Fixed function names for CLs calculator initialisation which would create 
   error in postfit expected CLs computation.
   ([#124](https://github.com/MadAnalysis/madanalysis5/pull/124))

## Contributors

This release contains contributions from (in alphabetical order):

[Jack Y. Araz](https://github.com/jackaraz), [Kyle Fan](https://github.com/kfan326)
