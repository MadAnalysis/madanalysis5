# Release version 1.10

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

* MadAnalysis will keep track of release updates on GitHub and inform users
   regarding the version in use.
   ([#65](https://github.com/MadAnalysis/madanalysis5/pull/65))

## Improvements

* We added an interface to include CRs and VRs with using full likelihoods.
   ([#5](https://github.com/MadAnalysis/madanalysis5/pull/5))

* Hadronic and invisible particle definitions in the reco mode have been externalised
   from the analysis code. Now they will be globally set in `main.cpp`. This allows the
   PADForSFS framework to be able to change the list of hadronic and invisible
   particles through the normal mode of running of the code.
   ([#66](https://github.com/MadAnalysis/madanalysis5/pull/66))

* Installation of Python-based third-party software has been deprecated.
   This now will be handled through `requirements.txt`.
   ([#68](https://github.com/MadAnalysis/madanalysis5/pull/68))

* Debug mode message has been extended to include module, function and file names
   with line number. ([#90](https://github.com/MadAnalysis/madanalysis5/pull/90))

* Set hard limit to python version to 3.6+ (PS: lxplus uses 3.6 minimum)
   ([#92](https://github.com/MadAnalysis/madanalysis5/pull/92))

* An option to set random seed has been implemented, both for the normal and the expert modes
   ([#96](https://github.com/MadAnalysis/madanalysis5/pull/96)).

* Ability to read and store length and energy information has been implemented
  for particle propagator module.
  ([#140](https://github.com/MadAnalysis/madanalysis5/pull/140))

* Pyhf version requirement have been updated.
  ([#134](https://github.com/MadAnalysis/madanalysis5/pull/134))

* More digits have been added to the CLs output file.
  ([#194](https://github.com/MadAnalysis/madanalysis5/pull/194))

* The required version of pyhf was updated to v0.7.3 to take advantage of recent
  edge case bug fixes.
  ([#211](https://github.com/MadAnalysis/madanalysis5/pull/211))

## Bug fixes

* Permanently fix the zlib version to the latest.
   ([#219](https://github.com/MadAnalysis/madanalysis5/pull/219))

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

* Bugfix for covariance matrix construction when global likelihood switch is off
   ([#88](https://github.com/MadAnalysis/madanalysis5/pull/88)).

* Update the version check message for the cases that local version is greater
   than stable version. ([#91](https://github.com/MadAnalysis/madanalysis5/pull/91))
* Error handling has been fixed for the version update checker
   ([#95](https://github.com/MadAnalysis/madanalysis5/pull/95)).

* SL interface break has been fixed in cases of HL extrapolation
   ([#93](https://github.com/MadAnalysis/madanalysis5/pull/93)).

* Custom analyses do not need to have vtable's. The expert mode and normal mode
   analysis writers have therefore been modified to remove these `virtual` functions
   from the generated analyses.
   ([#96](https://github.com/MadAnalysis/madanalysis5/pull/96)).

* `RecEventFormat` memory allocation for MC particles has been freed
   ([#100](https://github.com/MadAnalysis/madanalysis5/pull/100)).

* Bugfix for multiplarticle decleration in expert-reco mode initiation without an SFS card.
  ([#111](https://github.com/MadAnalysis/madanalysis5/pull/111))

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

* Fixed an attribute misspelling in `ObservableBase`.
   ([#171](https://github.com/MadAnalysis/madanalysis5/pull/171))

* Update of the version of Delphes/DelphesMa5tune + compatibility with M1 chips and Mac OS 13.0.
   ([#173](https://github.com/orgs/MadAnalysis/discussions/173))

* Covariance matrix implementation has been fixed for HL extrapolations 
  (see issue [#223](https://github.com/MadAnalysis/madanalysis5/issues/223)).
  ([#225](https://github.com/MadAnalysis/madanalysis5/pull/225))

## Contributors

This release contains contributions from (in alphabetical order):

[Jack Y. Araz](https://github.com/jackaraz),
[Kyle Fan](https://github.com/kfan326),
[Matthew Feickert](https://github.com/matthewfeickert),
[Benjamin Fuks](https://github.com/bfuks)
