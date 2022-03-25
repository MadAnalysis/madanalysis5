# Release 1.11.0-dev (development release)

## New features since last release

 * MadAnalysis is now bumped up to use c++11 as a minimal environment. 
([#10](https://github.com/MadAnalysis/madanalysis5/pull/10))

 * Following functionalities have been introduced in expert mode 
([#10](https://github.com/MadAnalysis/madanalysis5/pull/10))
   * Additional functionalities added to Expertmode to simplify the analysis code. This includes various functions which 
can handle filtering and overlap removal in the background.
   * Functionality to handle default hadronic and invisible particles in the background.
   * Signal region dependent reweigthing.
 
 * A public test interface have been made available for public execution 
([#20](https://github.com/MadAnalysis/madanalysis5/pull/20))

 * Ability add singla regions into respective cuts as a vector 
([#29](https://github.com/MadAnalysis/madanalysis5/pull/29)).

 * A `requirements.txt` file has been added to externally handle python libraries via `pip` 
 ([#31](https://github.com/MadAnalysis/madanalysis5/pull/31)).

## Improvements
 * Interface to include CR and VR in the full likelihoods 
  ([#5](https://github.com/MadAnalysis/madanalysis5/pull/5))

## Bug fixes
 * Zero division error in simplified likelihoods workflow 
  ([#4](https://github.com/MadAnalysis/madanalysis5/pull/4))
 * Bug fix in pyhf wrapper ([#10](https://github.com/MadAnalysis/madanalysis5/pull/10))
 * Ability to flush previous executions have been added to PADForSFS 
  ([#17](https://github.com/MadAnalysis/madanalysis5/pull/17))
 * `RecParticleFormat` requires `ntracks()` accessor in order to use it in NormalMode 
  ([#56](https://github.com/MadAnalysis/madanalysis5/pull/56))

## Contributors

This release contains contributions from (in alphabetical order):

Jack Y. Araz, Benjamin Fuks