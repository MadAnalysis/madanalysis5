# Release v1.11

## New features since last release

* Restframes framework has been impelemented for expert mode

## Improvements

* Statistical inference has been moved to Spey
* The validation module has been rebranded (for now)
* Moving the file incuding the link to all PAD analyses to github (see [PR320](https://github.com/MadAnalysis/madanalysis5/pull/320))

## Bug fixes
* Bug fix in the HistFrequency module of SampleAnalyzer
  (The first time an entry was found, it was not added to the histo)
* Fixing the PAD installation due to change in the metadata management of the dataverse (see [PR319](https://github.com/MadAnalysis/madanalysis5/pull/319))
* Region removal was missing from the CLI (see [PR320](https://github.com/MadAnalysis/madanalysis5/pull/320))


## Contributors

This release contains contributions from (in alphabetical order):

* Jack Y. Araz
* Benjamin Fuks
