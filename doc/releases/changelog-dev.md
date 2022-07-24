# Release 2.x-dev (development release)

## New features since last release

## Improvements
RegionSelection Manager object should no longer leak memory upon exiting scope/destruction.

## Bug fixes
fixed destructor for RegionSelectionManager to delete allocated memory allocated for RegionSelection pointer vector (PR #113)

## Contributors

This release contains contributions from (in alphabetical order):

Jack Araz, Kyle Fan, Benjamin Fuks
