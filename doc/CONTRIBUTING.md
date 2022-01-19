# Contributing to MadAnalysis 5

We are happy to accept contributions to `madanalysis5` via 
[Pull Requests to the GitHub repository](https://github.com/MadAnalysis/madanalysis5/pulls). 
You can start with forking the repository!

- Unless there is a very small fix that does not require discussion, please first [open an issue](https://github.com/MadAnalysis/madanalysis5/issues/new/choose)
to discuss your PR with ma5-dev team. For a PR example you can check the [PR #12](https://github.com/MadAnalysis/madanalysis5/pull/12) by Matthew Feickert ([@matthewfeickert](https://github.com/matthewfeickert)).
- If the change is not limited to a single line, please create a draft pull request. This draft should include context of the change and
description and benefits of the implementation in detail. If there is a change within `SampleAnalyzer` (the `c++` interface)
please provide backwards compatibility checks which that are done, these should include a selected set of PAD analyses with 
different backends (i.e. `SFS`, `Delphes` and `Ma5Tune`) using a large enough sample to make sure everything runs smoothly. 
If the change is within python interface please run some standard tests like plotting already existing variables within 
partonic, hadronic and reconstruction mode. Additionally please make sure to add examples on how to run your implementation.
- If there are any drawbacks of your implementation please specify and offer possible solutions, if any.

### Pull request procedure
Here are the steps to follow to make a pull request:
1. Fork the `madanalysis5` repository.
2. Open an issue and discuss the implementation with the developers.
3. Commit your changes to a feature branch on your fork and push all your changes there.
4. Start a pull request draft including the topics mentioned above and let the developers know about your progress. 
If there is a branch named `dev` please propose the merger to that branch, if not procede with `main`.
5. Pull main (or `dev`, if exists) branch to make sure there is no conflict with the current state of the development.
6. Make sure that you have added your name on the list `doc/CONTRIBUTORS.md` by simply adding it at the end of the list.
7. Once you are done request one of the maintainers to review your PR.