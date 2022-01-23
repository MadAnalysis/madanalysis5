# Analysis submission to Public Analysis Database
Scientific reproducibility and data preservation solely depend on preserving analysis logic in a reinterpretable form. 
You can contribute to the HEP community by sharing the LHC recast you have implemented in the MadAnalysis 5 framework, 
through [Public Analysis Database (PAD)](http://madanalysis.irmp.ucl.ac.be/wiki/PublicAnalysisDatabase)! 
Please send us your analysis code, detector card, info file and validation note to be included in PAD for public use.

More information and examples can be found in the proceedings of 
[the second MadAnalysis 5 Workshop on LHC recasting in Korea](https://doi.org/10.1142/S0217732321020016). 
Analysis codes have been published, documented and got a DOI so that they can now be cited.

## Guidelines for analysis submission:
- **Analysis file name:** Each file to be submitted needs to bear analysis identification i.e. 
`<experiment>_<collaboration>_<year>_<analysis ID>`. For instance, for the CMS analysis presented in 
[arXiv: 2107.13021](http://arxiv.org/abs/2107.13021) the identification is given at the top right corner of the 
dedicated [CMS result webpage](http://cms-results.web.cern.ch/cms-results/public-results/publications/EXO-20-004/#Ref) 
which is `cms_exo_20_004`.
- Each submission must include three files, namely analysis header file (with `.h` extension), main analysis execution 
file (with `.cpp` extension), detector card and finally, information file, including the signal region information (with `.info` extension).
- **Validation note:** Each analysis must be presented with a validation note. The validation note can be presented as 
a section in a published study or a detailed separate note needs to be provided. For an example on acceptable format
one can see ATLAS - SUSY - 2019 - 08 analysis prepared by Mark Goodsel in 
[this link](http://madanalysis.irmp.ucl.ac.be/raw-attachment/wiki/PublicAnalysisDatabase/validation_atlas_susy_2019_08.pdf).

### Detector card format
MadAnalysis 5 accepts two types of detector simulation, namely Delphes and [SFS](https://arxiv.org/abs/2006.09387). For
Delphes based analyses the card name needs to be `delphes_card_<experiment>_<collaboration>_<year>_<analysis ID>.tcl` and,  
similarly, for [SFS](https://arxiv.org/abs/2006.09387) based analyses `sfs_card_<experiment>_<collaboration>_<year>_<analysis ID>.ma5`. 

Delphes card needs to include all the detector information in the given `tcl` file with the suggested format defined for 
Delphes. The [SFS](https://arxiv.org/abs/2006.09387) card, on the other hand, needs to include MadAnalysis 5 commands to 
construct on-the-fly detector simulation.

### Analysis file format

Each analysis initialization need to start with a description and author contact information; `xyz_abc_ii_jjj.cpp`:
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
Please make sure that your code does not create any output file other than MadAnalysis 5 based curflow and histogram 
information. For details on how to write a recast see [this link](https://madanalysis.irmp.ucl.ac.be/wiki/WritingAnalyses).

### The signal region information file format
The `xyz_abc_ii_jjj.info` file contains the information about observed and numbers of background events in our 
signal regions. This is needed when one wants to compute confidence levels or upper limits embeddded in PAD framework.

For our imaginary analysis we have a luminosity of 20 fb-1, and four each of the four signal regions we have observed 
numbers of events (nobs), estimated backgrounds (nb), and background errors (deltanb) as follows:

- Signal region 0, `200<MET<500,100<HT<500`: `nobs=6159`, `nb=6090.0`, `deltanb_stat=123.0`, `deltanb_syst=156.0`
- Signal region 1, `MET>500,100<HT<500`: `nobs=2305`, `nb=2280.0`, `deltanb=270.0`

From this we create the following `xyz_abc_ii_jjj.info`:

```xml
<!--
this XML file serves for the statistical interpretation of the MA5 simulation. 
it lists the number of observed events <nobs>, number of expected backgrounds <nb> 
and number of background uncertainty <deltanb> in each of the regions  
-->

<!--
to be put in the same directory as the analysis code,
i.e. Build/SampleAnalyzer/Analyzer/
-->

<analysis id="cat_ued_16_013">
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
Note that each region block presented as `<region type="signal" id="200<MET<500,100<HT<500">` has its unique `id`. This
`id` is the name of the signal region declared in the analysis file via 
```cpp
Manager()->AddRegionSelection("200<MET<500,100<HT<500");
```
Note that there are two types of inputs are provided. In the fist signal region in addition to number of observed (`nobs`) 
and expected (`nb`) events there is also statistical (`deltanb_stat`) and systematical (`deltanb_syst`) uncertainties 
on the expected number of events. This allows more precice calculation of the uncertainties for a given Gaussian model.
For details, please see ref. [1910.11418](https://arxiv.org/abs/1910.11418). If these information are not available one can
simply input the total uncertainty (`deltanb`) on expected number of events.
