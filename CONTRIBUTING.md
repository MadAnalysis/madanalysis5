# Contributing to MadAnalysis 5

We are happy to accept contributions to `madanalysis5` via
  [Pull Requests to our GitHub repository](https://github.com/MadAnalysis/madanalysis5/pulls).
You can begin this with forking the `main` repository.

Unless there is a very small fix that does not require any discussion, please
always first [open an issue](https://github.com/MadAnalysis/madanalysis5/issues/new/choose)
to discuss your request with the development team. For a good example, please check out the
[PR #12](https://github.com/MadAnalysis/madanalysis5/pull/12) proposed by Matthew Feickert
([@matthewfeickert](https://github.com/matthewfeickert)).

If the desired change is not limited to a couple of lines of code, please create
a draft pull request. This draft should detail the context of the change, its
description and the benefits of the implementation.
 - If there is a change within the `SampleAnalyzer` code (the `c++` core of
  `MadAnalysis5`), please provide information on the backwards compatibility
  tests that have been done. These should preferably include the running of a
  selected set of PAD analyses with different backends (i.e. `SFS`, `Delphes`
  and/or the `MA5tune`), and rely on a large enough event sample to make sure
  everything runs smoothly.
 - If there is a change within the Python interface of the program, please
   proceed with standard tests such as plotting existing variables in the
   partonic, hadronic and reconstructed mode.
 - Please additionally make sure to add examples on how to use the new
   implementation.
- If there are any drawbacks of your implementation, these should be specified.
  Possible solutions should be offered, if any.

### Pull request procedure
Here are the steps to follow to make a pull request:
1. Fork the `madanalysis5` repository.
2. Open an issue and discuss the implementation with the developers.
3. Commit your changes to a feature branch on your fork and push all your
   changes there.
4. Start a draft pull request (see above) and let the developers know about your
   progress. If there is a branch named `dev`, please propose to merge to that
   branch. Otherwise, please proceed with `main`.
5. Pull the main (or `dev`, if exists) branch to make sure that there is no
   conflict with the current developments of the code.
6. Make sure that you have added your name to the list of contributors in
   `doc/CONTRIBUTORS.md` and modify appropriate section of 
   `doc/releases/changelog-dev.md`.
7. Once you are done, request one of the maintainers to review your PR.
