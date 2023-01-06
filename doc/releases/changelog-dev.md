# Release 2.x-dev (development release)

## New features since last release

## Improvements

## Bug fixes

 * Fixed destructor in `RegionSelectionManager` so that `RegionSelection` 
   objects allocated inside the `region_vector` are properly destructed upon 
   existing `scope/destruction` of `RegionSelectionManager`.
   ([#113](https://github.com/MadAnalysis/madanalysis5/pull/113))

 * Fixed function names for CLs calculator initialisation which would create 
   error in postfit expected CLs computation.
   ([#124](https://github.com/MadAnalysis/madanalysis5/pull/124))

  * zlib version has been updated.
   ([#152](https://github.com/MadAnalysis/madanalysis5/pull/152))

 * Fixed the directory import mentioned in issue [#146](https://github.com/MadAnalysis/madanalysis5/issues/146)
   ([#156](https://github.com/MadAnalysis/madanalysis5/pull/156))

 * Fixed attribute setting issue presented in issue [#153](https://github.com/MadAnalysis/madanalysis5/issues/153)
   ([#154](https://github.com/MadAnalysis/madanalysis5/pull/154)).

## Contributors

This release contains contributions from (in alphabetical order):

[Jack Y. Araz](https://github.com/jackaraz), [Kyle Fan](https://github.com/kfan326)
