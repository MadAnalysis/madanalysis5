# Release 2.0.0-dev (development release)

## New features since last release

* A module allowing for jet substructure studies has been implemented in
  MadAnalysis 5. [(#13)](https://github.com/MadAnalysis/madanalysis5/pull/13)
  * It includes the following fastjet-contrib functionalities:
    * VariableR: normal and expert mode
    * SoftDrop: expert mode
    * Clusterer: expert mode
    * Jet Filtering: expert mode
    * Reclustering: expert mode
    * Nsubjettiness: expert mode
    * Pruner: expert mode
    * Energy Correlator: expert mode
    * HEPTopTagger: expert mode
  * `RecJetFormat` has been wrapped with a `PseudoJet` structure to accommodate
     dynamic transition between two objects.

  All these methods can be used through the SFS interface.

* Substructure and HEPTopTagger has been redesigned to be shared libraries, so
  that MadAnalysis could be compiled without these modules as well.
  [(#63)](https://github.com/MadAnalysis/madanalysis5/pull/63)

* The tagger module has been redesigned to accommodate the features of
  substructure module.
  [(#86)](https://github.com/MadAnalysis/madanalysis5/pull/86)

* Multi-level object tagging has been enabled through the SFS framework.
  ([#97](https://github.com/MadAnalysis/madanalysis5/pull/97))
  * Option to activate charm-jet tagging has been introduced.
  * Option to use jet-based hadronic tau matching has been introduced.

* Command to fix random seed has been added.
  [(#86)](https://github.com/MadAnalysis/madanalysis5/pull/86)

## Improvements

* The SFS libraries are now included in the file `analysisList.h`, instead of in
  the main analysis file. This leads to much cleaner and independent analysis
  construction. [(#86)](https://github.com/MadAnalysis/madanalysis5/pull/86)

* Compilation time testing has been added for the jet Substructure and
  HEPTopTagger interfaces 
  ([#86](https://github.com/MadAnalysis/madanalysis5/pull/86)).

* `fatjet` and `genjet` collections have been merged into `jetcollection` map.
  ([#135](https://github.com/MadAnalysis/madanalysis5/pull/135))

## Bug fixes

* Various bugfixes for matching the numeric results to MA5 v1.10.
  ([#97](https://github.com/MadAnalysis/madanalysis5/pull/97))

* $M_{eff}$ definition has been fixed.
  [(#86)](https://github.com/MadAnalysis/madanalysis5/pull/86)

* Jet collection accessor has been fixed in DelphesMa5Tune
  ([#135](https://github.com/MadAnalysis/madanalysis5/pull/135))

* Submit function has been modified to eliminate issues with LHCO 
  reader mentioned in issue [#136](https://github.com/MadAnalysis/madanalysis5/issues/136).
  ([#167](https://github.com/MadAnalysis/madanalysis5/pull/167))

## Contributors

This release contains contributions from (in alphabetical order):

[Jack Y. Araz](https://github.com/jackaraz), 
[Benjamin Fuks](https://github.com/BFuks)
