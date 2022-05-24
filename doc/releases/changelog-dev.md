# Release 1.11.0-dev (development release)

## New features since last release

 * MadAnalysis is now bumped up to use c++11 as a minimal environment.
   ([#10](https://github.com/MadAnalysis/madanalysis5/pull/10))

 * The following functionalities have been introduced in the expert mode.
   ([#10](https://github.com/MadAnalysis/madanalysis5/pull/10))
   * New functionalities added to simplify writing analysis codes. This includes
     various functions which can handle filtering and overlap removals.
   * New functionality to handle the lists of default hadronic and invisible
     particles.
   * Signal region dependent reweigthing is now live.

 * A public test interface has been made available for public execution.
   ([#20](https://github.com/MadAnalysis/madanalysis5/pull/20))

 * There is now the ability to provide signal region information to cuts with
   vectors of SRs.
   ([#29](https://github.com/MadAnalysis/madanalysis5/pull/29)).

 * A `requirements.txt` file has been added to externally handle Python
   libraries via `pip`.
   ([#31](https://github.com/MadAnalysis/madanalysis5/pull/31)).

## Improvements
 * We added an interface to include CRs and VRs with using full likelihoods.
   ([#5](https://github.com/MadAnalysis/madanalysis5/pull/5))

## Bug fixes
 * Zero division error fixed in the simplified likelihoods workflow.
   ([#4](https://github.com/MadAnalysis/madanalysis5/pull/4))
 * Bug fix in the Pyhf wrapper.
   ([#10](https://github.com/MadAnalysis/madanalysis5/pull/10))
 * The ability to flush previous executions has been added to the PADForSFS.
   ([#17](https://github.com/MadAnalysis/madanalysis5/pull/17))
 * The `RecParticleFormat` dataformat now has an `ntracks()` accessor (as
   required by the NormalMode).
   ([#56](https://github.com/MadAnalysis/madanalysis5/pull/56))
 * Negative value can now be used for SFS bounds.
   ([#52](https://github.com/MadAnalysis/madanalysis5/pull/52))

## Contributors

This release contains contributions from (in alphabetical order):

Jack Y. Araz, Benjamin Fuks
