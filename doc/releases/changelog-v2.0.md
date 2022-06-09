# Release 2.0.0-dev (development release)

## New features since last release

* Substructure module have been implemented to MadAnalysis 5.
  [(#13)](https://github.com/MadAnalysis/madanalysis5/pull/13)
  * Module includes the following functionality through normal/expert mode
    * VariableR: normal and expert mode
    * SoftDrop: expert mode
    * Clusterer: expert mode
    * Jet Filtering: expert mode
    * Reclustering: expert mode
    * Nsubjettiness: expert mode
    * Pruner: expert mode
    * Energy Correlator: expert mode
    * HEPTopTagger: expert mode
  * `RecJetFormat` has been wrapped with `PseudoJet` to accommodate dynamic
    transition between two objects.
  
  All these methods can be used through SFS interface.

* Substructure and HEPTopTagger has been redesigned to be shared library 
so that MadAnalysis can be compiled without these modules as well.
  [(#63)](https://github.com/MadAnalysis/madanalysis5/pull/63)

* Tagger module has been redesigned to accommodate substructure module.
  [(#86)](https://github.com/MadAnalysis/madanalysis5/pull/86)
    

## Improvements

* SFS libraries included in `analysisList.h` file instead of main analysis
  file. This will lead to much cleaner and independent analysis construction.
  [(#86)](https://github.com/MadAnalysis/madanalysis5/pull/86)

## Bug fixes

## Contributors

This release contains contributions from (in alphabetical order):

[Jack Y. Araz](https://github.com/jackaraz), 
[Benjamin Fuks](https://github.com/BFuks)