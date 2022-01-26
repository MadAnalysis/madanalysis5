# Analysis submission to Public Analysis Database
Scientific reproducibility and data preservation at the LHC require us to
preserve the logic of all LHC analyses in a reinterpretable form. Such an option
is available within the MadAnalysis 5 framework. In the case you have
implemented an existing LHC analysis and would like this contribution to be
officially included in the MadAnalysis 5 framework through its
[Public Analysis Database (PAD)](http://madanalysis.irmp.ucl.ac.be/wiki/PublicAnalysisDatabase).
In this case, please send us your analysis code, detector card, info file and
validation note. We will then review them and include the implementation in our
[dataverse](https://dataverse.uclouvain.be/dataverse/madanalysis) and on the
[PAD](http://madanalysis.irmp.ucl.ac.be/wiki/PublicAnalysisDatabase) for punlic usage.

More information and examples can be found in the proceedings of 
[the second MadAnalysis 5 Workshop on LHC recasting in Korea](https://doi.org/10.1142/S0217732321020016). 
Analysis codes have been published, documented and got a DOI so that they can now be cited.


## Guidelines for the submission of a new implementation

### Generalities
**Analysis filename:** Each submitted file needs to be related to the
corresponding analysis through its filename. The latter reads
   `<experiment>_<collaboration>_<year>_<analysis ID>`.
For instance, for the CMS analysis presented in
[arXiv: 2107.13021](http://arxiv.org/abs/2107.13021), we can find the analysis
identification code in the top right corner of the dedicated
[CMS Twiki webpage](http://cms-results.web.cern.ch/cms-results/public-results/publications/EXO-20-004/#Ref).
It is here `cms_exo_20_004`.

Each submission must include **four files**, namely a header file (with a `.h`
extension), the main source code of the analysis (with a `.cpp` extension),
a detector card and the information file (with a `.info` extension) that
includes observation and background information for each signal region of the
analysis.

**Validation note:** Each submission must be provided together with a
validation note. This note can be presented as a section in a published study,
or a detailed separate note. A good example of an acceptable format could be
found through
[this link](http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PublicAnalysisDatabase/validation_atlas_susy_2019_08.pdf).


### Detector card format
Collider analysis implementations in MadAnalysis 5 can rely on two format for
the parametrisation of the detector simulation, namely the Delphes and
[SFS](https://arxiv.org/abs/2006.09387) format. For implementations based on
Delphes, the detector card name needs to be of the form
  `delphes_card_<experiment>_<collaboration>_<year>_<analysis ID>.tcl`
Similarly, for implementation based on the SFS, the detector simulator filename
reads
  `sfs_card_<experiment>_<collaboration>_<year>_<analysis ID>.ma5`.

The Delphes card needs to include all detector information in a way compatible
with the Delphes `tcl` format. The SFS card includes MadAnalysis 5 commands to
perform an on-the-fly detector simulation on run-time, as detailed in the
[SFS manual](https://arxiv.org/abs/2006.09387)).


### Analysis file format

Each analysis initialisation method needs to start with a description of the
analysis considered and information on the author of the code. This gives, in
the file `xyz_abc_ii_jjj.cpp`:
```cpp
bool xyz_abc_ii_jjj::Initialize(const MA5::Configuration& cfg, 
                                const std::map<std::string,std::string>& parameters)
{
  INFO << "    <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
  INFO << "    <>                          XYZ ABC II JJJ                          <>" << endmsg;
  INFO << "    <>    phase space info @ XX TeV, YYY fb^-1                          <>" << endmsg;
  INFO << "    <>    arXiv: xxxx.xxxxx                                             <>" << endmsg;
  INFO << "    <>    Recasted by : ...                                             <>" << endmsg;
  INFO << "    <>    Contact     : ...                                             <>" << endmsg;
  INFO << "    <>    Based on MadAnalysis 5 <version> and above                    <>" << endmsg;
  INFO << "    <>    DOI: TBA                                                      <>" << endmsg;
  INFO << "    <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
```
Please make sure that your code does not generate any output file other than
those MadAnalysis 5 can deal with (cutflow and charts histograms). For extra
details, in particular on how to write a full code, please
[this link](https://madanalysis.irmp.ucl.ac.be/wiki/WritingAnalyses) or
[this paper](https://arxiv.org/abs/1808.00480).

### The signal region information file format
The `xyz_abc_ii_jjj.info` file contains information about the observed number of
events populating each signal region of the analysis, and about the background
expectation. These pieces of information are needed for the computation of
exclusion levels or upper limits on a given signal within the PAD framework.

For example, we consider an imaginary analysis targeting a luminosity of 20/fb
and containing two signal regions. We have observed those regions to be
populated by some events (nobs). In addition, we can extract information on the
estimated number of background events (nb) and their errors (deltanb) from the
experimental papers. This would give:
- Signal region 0, `200<MET<500,100<HT<500`: `nobs=6159`, `nb=6090.0`, `deltanb_stat=123.0`, `deltanb_syst=156.0`
- Signal region 1, `MET>500,100<HT<500`: `nobs=2305`, `nb=2280.0`, `deltanb=270.0`

From this we can create the following `xyz_abc_ii_jjj.info` file:

```xml
<!--
this XML file serves for the statistical interpretation of the MA5 simulation. 
it lists the number of observed events <nobs>, number of expected backgrounds <nb> 
and number of background uncertainty <deltanb> in each of the regions  
-->

<!--
to be put in the same directory as the analysis code,
i.e. Build/SampleAnalyzer/User/Analyzer/
-->

<analysis id="xyz_abc_ii_jjj">
  <lumi>20.0</lumi> <!-- in fb^-1 -->

  <!-- first, the signal regions targeting stop -> t neutralino -->

  <!--
     region definition: the attribute "id" has to match the name of the region
     as defined in the analysis code;
     the attribute "type" can be "signal" or "control" and is optional (default=signal)
  -->

  <region type="signal" id="200<MET<500,100<HT<500">
    <nobs>6159</nobs>
    <nb>6090.0</nb>
    <deltanb_stat>123.0</deltanb_stat>
    <deltanb_syst>156.0</deltanb_syst>
  </region>

  <region type="signal" id="MET>500,100<HT<500">
    <nobs>2305</nobs>
    <nb>2280.0</nb>
    <deltanb>270.0</deltanb>
  </region>
</analysis>
```
Here, each region XML block is presented as
  `<region type="signal" id="200<MET<500,100<HT<500">`
with a unique identifier `id`. This `id` is the name of the signal region as
declared in the analysis code via 
```cpp
Manager()->AddRegionSelection("200<MET<500,100<HT<500");
```
Moreover, each region block is declared slightly differently. In the first
signal region we can find the number of observed events (`nobs`) and the
background expectation (`nb`) with its statistical error (`deltanb_stat`) and
systematical error (`deltanb_syst`). This allows for more precice calculation
of the final uncertainties following a given Gaussian model. For details, please
see [this paper](https://arxiv.org/abs/1910.11418). If these pieces of
information are not available, we can simply input the total uncertainty
(`deltanb`) on the background, as for the second signal region.
