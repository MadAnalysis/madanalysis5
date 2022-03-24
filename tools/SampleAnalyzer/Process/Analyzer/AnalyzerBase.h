////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


#ifndef ANALYSISBASE_h
#define ANALYSISBASE_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Process/Plot/PlotManager.h"
#include "SampleAnalyzer/Process/Counter/CounterManager.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/Utils.h"
#include "SampleAnalyzer/Commons/Service/SortingService.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Process/RegionSelection/RegionSelectionManager.h"
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// FJcontrib tools
#ifdef MA5_FASTJET_MODE
    #include "SampleAnalyzer/Interfaces/fastjet/SoftDrop.h"
    #include "SampleAnalyzer/Interfaces/fastjet/Cluster.h"
    #include "SampleAnalyzer/Interfaces/fastjet/Recluster.h"
    #include "SampleAnalyzer/Interfaces/fastjet/Nsubjettiness.h"
    #include "SampleAnalyzer/Interfaces/fastjet/VariableR.h"
    #include "SampleAnalyzer/Interfaces/fastjet/Pruner.h"
    #include "SampleAnalyzer/Interfaces/fastjet/Selector.h"
    #include "SampleAnalyzer/Interfaces/fastjet/Filter.h"
#endif

// STL headers
#include <set>
#include <string>
#include <cmath>
#include <vector>
#include <map>


// initializing MACRO 
#define INIT_ANALYSIS(CLASS,NAME) public: CLASS() {setName(NAME);} virtual ~CLASS() {} private:

namespace MA5
{

class AnalyzerBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public :

  /// name of the analysis
  std::string name_;

  /// Weighted events mode
  MAbool weighted_events_;

  /// A RS manager is associated with each analysis
  RegionSelectionManager manager_;
  std::string outputdir_;

  /// Writer SAF
  SAFWriter out_;

  // options
  std::map<std::string, std::string> options_;

  // parameters
  std::map<std::string, std::string> parameters_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  AnalyzerBase()
  { name_="unknown"; outputdir_=""; }

  /// Destructor
  virtual ~AnalyzerBase()
  { }

  /// Initialize (common part to all analyses)
  MAbool PreInitialize(const std::string& outputName, 
                     const Configuration* cfg)
  {
    weighted_events_ = !cfg->IsNoEventWeight();
    out_.Initialize(cfg,outputName.c_str());
    options_ = cfg->Options();
    return true;
  }

  /// Initialize (specific to the analysis)
  virtual MAbool Initialize(const Configuration& cfg,                  
             const std::map<std::string,std::string>& parameters)=0;

  MAbool Initialize(const Configuration& cfg)
  {
    return Initialize(cfg, parameters_);
  }

  /// PreFinalize
  void PreFinalize(const SampleFormat& summary, 
                   const std::vector<SampleFormat>& samples)
  {
    out_.WriteHeader(summary); 
    out_.WriteFiles(samples);
  }

  /// Finalize
  virtual void Finalize(const SampleFormat& summary, 
                        const std::vector<SampleFormat>& samples)=0;

  /// PostFinalize
  void PostFinalize(const SampleFormat& summary,
                    const std::vector<SampleFormat>& samples)
  {
    // Closing output file
    out_.WriteFoot(summary);
    out_.Finalize();
  }

  /// Execute
  MAbool PreExecute(const SampleFormat& mySample,
                  const EventFormat& myEvent)
  { 
    PHYSICS->Id->SetFinalState(myEvent.mc());
    PHYSICS->Id->SetInitialState(myEvent.mc());
    return true;
  }

  virtual MAbool Execute(SampleFormat& mySample,
                       const EventFormat& myEvent)=0;

  /// Accessor to analysis name
  const std::string name() const {return name_;}

  /// Accessor to the manager
  RegionSelectionManager *Manager() { return &manager_; }

  /// Mutator to analysis name
  void setName(const std::string& Name) {name_=Name;}

  /// Accessor to the output directory name
  const std::string Output() const {return outputdir_;}

  /// Mutator to the output directory name
  void SetOutputDir(const std::string &name) {outputdir_=name;}


  SAFWriter& out()
  { return out_; }

  // Set command line options
  void SetOptions(std::map<std::string, std::string> options) {options_=options;}

  // Set parameters, initialized in main.cpp
  void SetParameters(std::map<std::string, std::string> params) {parameters_=params;}

  // Accessor to parameters
  const std::map<std::string, std::string> GetParameters() {return parameters_;}

  /// Get an option for this analysis instance as a string.
  std::string getOption(std::string optname) const
  {
    if ( options_.find(optname) != options_.end() )
       return options_.find(optname)->second;
    return "";
  }

    /// Get an option for this analysis instance converted to a specific type
    /// The return type is given by the specified @a def value, or by an explicit template
    /// type-argument, e.g. getOption<double>("FOO", 3).
    template<typename T>
    T getOption(std::string optname, T def) const {
      if (options_.find(optname) == options_.end()) return def;
      std::stringstream ss;
      ss << options_.find(optname)->second;
      T ret;
      ss >> ret;
      return ret;
    }

    /// overload for literal character strings (which don't play well with stringstream)
    /// Note this isn't a template specialisation, because we can't return a non-static
    /// char*, and T-as-return-type is built into the template function definition.
    std::string getOption(std::string optname, const char* def) {
      return getOption<std::string>(optname, def);
    }


    void AddDefaultHadronic()
    {
        // definition of the multiparticle "hadronic"
        PHYSICS->mcConfig().AddHadronicId(-20543);
        PHYSICS->mcConfig().AddHadronicId(-20533);
        PHYSICS->mcConfig().AddHadronicId(-20523);
        PHYSICS->mcConfig().AddHadronicId(-20513);
        PHYSICS->mcConfig().AddHadronicId(-20433);
        PHYSICS->mcConfig().AddHadronicId(-20423);
        PHYSICS->mcConfig().AddHadronicId(-20413);
        PHYSICS->mcConfig().AddHadronicId(-20323);
        PHYSICS->mcConfig().AddHadronicId(-20313);
        PHYSICS->mcConfig().AddHadronicId(-20213);
        PHYSICS->mcConfig().AddHadronicId(-10543);
        PHYSICS->mcConfig().AddHadronicId(-10541);
        PHYSICS->mcConfig().AddHadronicId(-10533);
        PHYSICS->mcConfig().AddHadronicId(-10531);
        PHYSICS->mcConfig().AddHadronicId(-10523);
        PHYSICS->mcConfig().AddHadronicId(-10521);
        PHYSICS->mcConfig().AddHadronicId(-10513);
        PHYSICS->mcConfig().AddHadronicId(-10511);
        PHYSICS->mcConfig().AddHadronicId(-10433);
        PHYSICS->mcConfig().AddHadronicId(-10431);
        PHYSICS->mcConfig().AddHadronicId(-10423);
        PHYSICS->mcConfig().AddHadronicId(-10421);
        PHYSICS->mcConfig().AddHadronicId(-10413);
        PHYSICS->mcConfig().AddHadronicId(-10411);
        PHYSICS->mcConfig().AddHadronicId(-10323);
        PHYSICS->mcConfig().AddHadronicId(-10321);
        PHYSICS->mcConfig().AddHadronicId(-10313);
        PHYSICS->mcConfig().AddHadronicId(-10311);
        PHYSICS->mcConfig().AddHadronicId(-10213);
        PHYSICS->mcConfig().AddHadronicId(-10211);
        PHYSICS->mcConfig().AddHadronicId(-5554);
        PHYSICS->mcConfig().AddHadronicId(-5544);
        PHYSICS->mcConfig().AddHadronicId(-5542);
        PHYSICS->mcConfig().AddHadronicId(-5534);
        PHYSICS->mcConfig().AddHadronicId(-5532);
        PHYSICS->mcConfig().AddHadronicId(-5524);
        PHYSICS->mcConfig().AddHadronicId(-5522);
        PHYSICS->mcConfig().AddHadronicId(-5514);
        PHYSICS->mcConfig().AddHadronicId(-5512);
        PHYSICS->mcConfig().AddHadronicId(-5503);
        PHYSICS->mcConfig().AddHadronicId(-5444);
        PHYSICS->mcConfig().AddHadronicId(-5442);
        PHYSICS->mcConfig().AddHadronicId(-5434);
        PHYSICS->mcConfig().AddHadronicId(-5432);
        PHYSICS->mcConfig().AddHadronicId(-5424);
        PHYSICS->mcConfig().AddHadronicId(-5422);
        PHYSICS->mcConfig().AddHadronicId(-5414);
        PHYSICS->mcConfig().AddHadronicId(-5412);
        PHYSICS->mcConfig().AddHadronicId(-5403);
        PHYSICS->mcConfig().AddHadronicId(-5401);
        PHYSICS->mcConfig().AddHadronicId(-5342);
        PHYSICS->mcConfig().AddHadronicId(-5334);
        PHYSICS->mcConfig().AddHadronicId(-5332);
        PHYSICS->mcConfig().AddHadronicId(-5324);
        PHYSICS->mcConfig().AddHadronicId(-5322);
        PHYSICS->mcConfig().AddHadronicId(-5314);
        PHYSICS->mcConfig().AddHadronicId(-5312);
        PHYSICS->mcConfig().AddHadronicId(-5303);
        PHYSICS->mcConfig().AddHadronicId(-5301);
        PHYSICS->mcConfig().AddHadronicId(-5242);
        PHYSICS->mcConfig().AddHadronicId(-5232);
        PHYSICS->mcConfig().AddHadronicId(-5224);
        PHYSICS->mcConfig().AddHadronicId(-5222);
        PHYSICS->mcConfig().AddHadronicId(-5214);
        PHYSICS->mcConfig().AddHadronicId(-5212);
        PHYSICS->mcConfig().AddHadronicId(-5203);
        PHYSICS->mcConfig().AddHadronicId(-5201);
        PHYSICS->mcConfig().AddHadronicId(-5142);
        PHYSICS->mcConfig().AddHadronicId(-5132);
        PHYSICS->mcConfig().AddHadronicId(-5122);
        PHYSICS->mcConfig().AddHadronicId(-5114);
        PHYSICS->mcConfig().AddHadronicId(-5112);
        PHYSICS->mcConfig().AddHadronicId(-5103);
        PHYSICS->mcConfig().AddHadronicId(-5101);
        PHYSICS->mcConfig().AddHadronicId(-4444);
        PHYSICS->mcConfig().AddHadronicId(-4434);
        PHYSICS->mcConfig().AddHadronicId(-4432);
        PHYSICS->mcConfig().AddHadronicId(-4424);
        PHYSICS->mcConfig().AddHadronicId(-4422);
        PHYSICS->mcConfig().AddHadronicId(-4414);
        PHYSICS->mcConfig().AddHadronicId(-4412);
        PHYSICS->mcConfig().AddHadronicId(-4403);
        PHYSICS->mcConfig().AddHadronicId(-4334);
        PHYSICS->mcConfig().AddHadronicId(-4332);
        PHYSICS->mcConfig().AddHadronicId(-4324);
        PHYSICS->mcConfig().AddHadronicId(-4322);
        PHYSICS->mcConfig().AddHadronicId(-4314);
        PHYSICS->mcConfig().AddHadronicId(-4312);
        PHYSICS->mcConfig().AddHadronicId(-4303);
        PHYSICS->mcConfig().AddHadronicId(-4301);
        PHYSICS->mcConfig().AddHadronicId(-4232);
        PHYSICS->mcConfig().AddHadronicId(-4224);
        PHYSICS->mcConfig().AddHadronicId(-4222);
        PHYSICS->mcConfig().AddHadronicId(-4214);
        PHYSICS->mcConfig().AddHadronicId(-4212);
        PHYSICS->mcConfig().AddHadronicId(-4203);
        PHYSICS->mcConfig().AddHadronicId(-4201);
        PHYSICS->mcConfig().AddHadronicId(-4132);
        PHYSICS->mcConfig().AddHadronicId(-4122);
        PHYSICS->mcConfig().AddHadronicId(-4114);
        PHYSICS->mcConfig().AddHadronicId(-4112);
        PHYSICS->mcConfig().AddHadronicId(-4103);
        PHYSICS->mcConfig().AddHadronicId(-4101);
        PHYSICS->mcConfig().AddHadronicId(-3334);
        PHYSICS->mcConfig().AddHadronicId(-3324);
        PHYSICS->mcConfig().AddHadronicId(-3322);
        PHYSICS->mcConfig().AddHadronicId(-3314);
        PHYSICS->mcConfig().AddHadronicId(-3312);
        PHYSICS->mcConfig().AddHadronicId(-3303);
        PHYSICS->mcConfig().AddHadronicId(-3224);
        PHYSICS->mcConfig().AddHadronicId(-3222);
        PHYSICS->mcConfig().AddHadronicId(-3214);
        PHYSICS->mcConfig().AddHadronicId(-3212);
        PHYSICS->mcConfig().AddHadronicId(-3203);
        PHYSICS->mcConfig().AddHadronicId(-3201);
        PHYSICS->mcConfig().AddHadronicId(-3122);
        PHYSICS->mcConfig().AddHadronicId(-3114);
        PHYSICS->mcConfig().AddHadronicId(-3112);
        PHYSICS->mcConfig().AddHadronicId(-3103);
        PHYSICS->mcConfig().AddHadronicId(-3101);
        PHYSICS->mcConfig().AddHadronicId(-2224);
        PHYSICS->mcConfig().AddHadronicId(-2214);
        PHYSICS->mcConfig().AddHadronicId(-2212);
        PHYSICS->mcConfig().AddHadronicId(-2203);
        PHYSICS->mcConfig().AddHadronicId(-2114);
        PHYSICS->mcConfig().AddHadronicId(-2112);
        PHYSICS->mcConfig().AddHadronicId(-2103);
        PHYSICS->mcConfig().AddHadronicId(-2101);
        PHYSICS->mcConfig().AddHadronicId(-1114);
        PHYSICS->mcConfig().AddHadronicId(-1103);
        PHYSICS->mcConfig().AddHadronicId(-545);
        PHYSICS->mcConfig().AddHadronicId(-543);
        PHYSICS->mcConfig().AddHadronicId(-541);
        PHYSICS->mcConfig().AddHadronicId(-535);
        PHYSICS->mcConfig().AddHadronicId(-533);
        PHYSICS->mcConfig().AddHadronicId(-531);
        PHYSICS->mcConfig().AddHadronicId(-525);
        PHYSICS->mcConfig().AddHadronicId(-523);
        PHYSICS->mcConfig().AddHadronicId(-521);
        PHYSICS->mcConfig().AddHadronicId(-515);
        PHYSICS->mcConfig().AddHadronicId(-513);
        PHYSICS->mcConfig().AddHadronicId(-511);
        PHYSICS->mcConfig().AddHadronicId(-435);
        PHYSICS->mcConfig().AddHadronicId(-433);
        PHYSICS->mcConfig().AddHadronicId(-431);
        PHYSICS->mcConfig().AddHadronicId(-425);
        PHYSICS->mcConfig().AddHadronicId(-423);
        PHYSICS->mcConfig().AddHadronicId(-421);
        PHYSICS->mcConfig().AddHadronicId(-415);
        PHYSICS->mcConfig().AddHadronicId(-413);
        PHYSICS->mcConfig().AddHadronicId(-411);
        PHYSICS->mcConfig().AddHadronicId(-325);
        PHYSICS->mcConfig().AddHadronicId(-323);
        PHYSICS->mcConfig().AddHadronicId(-321);
        PHYSICS->mcConfig().AddHadronicId(-315);
        PHYSICS->mcConfig().AddHadronicId(-313);
        PHYSICS->mcConfig().AddHadronicId(-311);
        PHYSICS->mcConfig().AddHadronicId(-215);
        PHYSICS->mcConfig().AddHadronicId(-213);
        PHYSICS->mcConfig().AddHadronicId(-211);
        PHYSICS->mcConfig().AddHadronicId(111);
        PHYSICS->mcConfig().AddHadronicId(113);
        PHYSICS->mcConfig().AddHadronicId(115);
        PHYSICS->mcConfig().AddHadronicId(130);
        PHYSICS->mcConfig().AddHadronicId(211);
        PHYSICS->mcConfig().AddHadronicId(213);
        PHYSICS->mcConfig().AddHadronicId(215);
        PHYSICS->mcConfig().AddHadronicId(221);
        PHYSICS->mcConfig().AddHadronicId(223);
        PHYSICS->mcConfig().AddHadronicId(225);
        PHYSICS->mcConfig().AddHadronicId(310);
        PHYSICS->mcConfig().AddHadronicId(311);
        PHYSICS->mcConfig().AddHadronicId(313);
        PHYSICS->mcConfig().AddHadronicId(315);
        PHYSICS->mcConfig().AddHadronicId(321);
        PHYSICS->mcConfig().AddHadronicId(323);
        PHYSICS->mcConfig().AddHadronicId(325);
        PHYSICS->mcConfig().AddHadronicId(331);
        PHYSICS->mcConfig().AddHadronicId(333);
        PHYSICS->mcConfig().AddHadronicId(335);
        PHYSICS->mcConfig().AddHadronicId(411);
        PHYSICS->mcConfig().AddHadronicId(413);
        PHYSICS->mcConfig().AddHadronicId(415);
        PHYSICS->mcConfig().AddHadronicId(421);
        PHYSICS->mcConfig().AddHadronicId(423);
        PHYSICS->mcConfig().AddHadronicId(425);
        PHYSICS->mcConfig().AddHadronicId(431);
        PHYSICS->mcConfig().AddHadronicId(433);
        PHYSICS->mcConfig().AddHadronicId(435);
        PHYSICS->mcConfig().AddHadronicId(441);
        PHYSICS->mcConfig().AddHadronicId(443);
        PHYSICS->mcConfig().AddHadronicId(445);
        PHYSICS->mcConfig().AddHadronicId(511);
        PHYSICS->mcConfig().AddHadronicId(513);
        PHYSICS->mcConfig().AddHadronicId(515);
        PHYSICS->mcConfig().AddHadronicId(521);
        PHYSICS->mcConfig().AddHadronicId(523);
        PHYSICS->mcConfig().AddHadronicId(525);
        PHYSICS->mcConfig().AddHadronicId(531);
        PHYSICS->mcConfig().AddHadronicId(533);
        PHYSICS->mcConfig().AddHadronicId(535);
        PHYSICS->mcConfig().AddHadronicId(541);
        PHYSICS->mcConfig().AddHadronicId(543);
        PHYSICS->mcConfig().AddHadronicId(545);
        PHYSICS->mcConfig().AddHadronicId(551);
        PHYSICS->mcConfig().AddHadronicId(553);
        PHYSICS->mcConfig().AddHadronicId(555);
        PHYSICS->mcConfig().AddHadronicId(1103);
        PHYSICS->mcConfig().AddHadronicId(1114);
        PHYSICS->mcConfig().AddHadronicId(2101);
        PHYSICS->mcConfig().AddHadronicId(2103);
        PHYSICS->mcConfig().AddHadronicId(2112);
        PHYSICS->mcConfig().AddHadronicId(2114);
        PHYSICS->mcConfig().AddHadronicId(2203);
        PHYSICS->mcConfig().AddHadronicId(2212);
        PHYSICS->mcConfig().AddHadronicId(2214);
        PHYSICS->mcConfig().AddHadronicId(2224);
        PHYSICS->mcConfig().AddHadronicId(3101);
        PHYSICS->mcConfig().AddHadronicId(3103);
        PHYSICS->mcConfig().AddHadronicId(3112);
        PHYSICS->mcConfig().AddHadronicId(3114);
        PHYSICS->mcConfig().AddHadronicId(3122);
        PHYSICS->mcConfig().AddHadronicId(3201);
        PHYSICS->mcConfig().AddHadronicId(3203);
        PHYSICS->mcConfig().AddHadronicId(3212);
        PHYSICS->mcConfig().AddHadronicId(3214);
        PHYSICS->mcConfig().AddHadronicId(3222);
        PHYSICS->mcConfig().AddHadronicId(3224);
        PHYSICS->mcConfig().AddHadronicId(3303);
        PHYSICS->mcConfig().AddHadronicId(3312);
        PHYSICS->mcConfig().AddHadronicId(3314);
        PHYSICS->mcConfig().AddHadronicId(3322);
        PHYSICS->mcConfig().AddHadronicId(3324);
        PHYSICS->mcConfig().AddHadronicId(3334);
        PHYSICS->mcConfig().AddHadronicId(4101);
        PHYSICS->mcConfig().AddHadronicId(4103);
        PHYSICS->mcConfig().AddHadronicId(4112);
        PHYSICS->mcConfig().AddHadronicId(4114);
        PHYSICS->mcConfig().AddHadronicId(4122);
        PHYSICS->mcConfig().AddHadronicId(4132);
        PHYSICS->mcConfig().AddHadronicId(4201);
        PHYSICS->mcConfig().AddHadronicId(4203);
        PHYSICS->mcConfig().AddHadronicId(4212);
        PHYSICS->mcConfig().AddHadronicId(4214);
        PHYSICS->mcConfig().AddHadronicId(4222);
        PHYSICS->mcConfig().AddHadronicId(4224);
        PHYSICS->mcConfig().AddHadronicId(4232);
        PHYSICS->mcConfig().AddHadronicId(4301);
        PHYSICS->mcConfig().AddHadronicId(4303);
        PHYSICS->mcConfig().AddHadronicId(4312);
        PHYSICS->mcConfig().AddHadronicId(4314);
        PHYSICS->mcConfig().AddHadronicId(4322);
        PHYSICS->mcConfig().AddHadronicId(4324);
        PHYSICS->mcConfig().AddHadronicId(4332);
        PHYSICS->mcConfig().AddHadronicId(4334);
        PHYSICS->mcConfig().AddHadronicId(4403);
        PHYSICS->mcConfig().AddHadronicId(4412);
        PHYSICS->mcConfig().AddHadronicId(4414);
        PHYSICS->mcConfig().AddHadronicId(4422);
        PHYSICS->mcConfig().AddHadronicId(4424);
        PHYSICS->mcConfig().AddHadronicId(4432);
        PHYSICS->mcConfig().AddHadronicId(4434);
        PHYSICS->mcConfig().AddHadronicId(4444);
        PHYSICS->mcConfig().AddHadronicId(5101);
        PHYSICS->mcConfig().AddHadronicId(5103);
        PHYSICS->mcConfig().AddHadronicId(5112);
        PHYSICS->mcConfig().AddHadronicId(5114);
        PHYSICS->mcConfig().AddHadronicId(5122);
        PHYSICS->mcConfig().AddHadronicId(5132);
        PHYSICS->mcConfig().AddHadronicId(5142);
        PHYSICS->mcConfig().AddHadronicId(5201);
        PHYSICS->mcConfig().AddHadronicId(5203);
        PHYSICS->mcConfig().AddHadronicId(5212);
        PHYSICS->mcConfig().AddHadronicId(5214);
        PHYSICS->mcConfig().AddHadronicId(5222);
        PHYSICS->mcConfig().AddHadronicId(5224);
        PHYSICS->mcConfig().AddHadronicId(5232);
        PHYSICS->mcConfig().AddHadronicId(5242);
        PHYSICS->mcConfig().AddHadronicId(5301);
        PHYSICS->mcConfig().AddHadronicId(5303);
        PHYSICS->mcConfig().AddHadronicId(5312);
        PHYSICS->mcConfig().AddHadronicId(5314);
        PHYSICS->mcConfig().AddHadronicId(5322);
        PHYSICS->mcConfig().AddHadronicId(5324);
        PHYSICS->mcConfig().AddHadronicId(5332);
        PHYSICS->mcConfig().AddHadronicId(5334);
        PHYSICS->mcConfig().AddHadronicId(5342);
        PHYSICS->mcConfig().AddHadronicId(5401);
        PHYSICS->mcConfig().AddHadronicId(5403);
        PHYSICS->mcConfig().AddHadronicId(5412);
        PHYSICS->mcConfig().AddHadronicId(5414);
        PHYSICS->mcConfig().AddHadronicId(5422);
        PHYSICS->mcConfig().AddHadronicId(5424);
        PHYSICS->mcConfig().AddHadronicId(5432);
        PHYSICS->mcConfig().AddHadronicId(5434);
        PHYSICS->mcConfig().AddHadronicId(5442);
        PHYSICS->mcConfig().AddHadronicId(5444);
        PHYSICS->mcConfig().AddHadronicId(5503);
        PHYSICS->mcConfig().AddHadronicId(5512);
        PHYSICS->mcConfig().AddHadronicId(5514);
        PHYSICS->mcConfig().AddHadronicId(5522);
        PHYSICS->mcConfig().AddHadronicId(5524);
        PHYSICS->mcConfig().AddHadronicId(5532);
        PHYSICS->mcConfig().AddHadronicId(5534);
        PHYSICS->mcConfig().AddHadronicId(5542);
        PHYSICS->mcConfig().AddHadronicId(5544);
        PHYSICS->mcConfig().AddHadronicId(5554);
        PHYSICS->mcConfig().AddHadronicId(10111);
        PHYSICS->mcConfig().AddHadronicId(10113);
        PHYSICS->mcConfig().AddHadronicId(10211);
        PHYSICS->mcConfig().AddHadronicId(10213);
        PHYSICS->mcConfig().AddHadronicId(10221);
        PHYSICS->mcConfig().AddHadronicId(10223);
        PHYSICS->mcConfig().AddHadronicId(10311);
        PHYSICS->mcConfig().AddHadronicId(10313);
        PHYSICS->mcConfig().AddHadronicId(10321);
        PHYSICS->mcConfig().AddHadronicId(10323);
        PHYSICS->mcConfig().AddHadronicId(10331);
        PHYSICS->mcConfig().AddHadronicId(10333);
        PHYSICS->mcConfig().AddHadronicId(10411);
        PHYSICS->mcConfig().AddHadronicId(10413);
        PHYSICS->mcConfig().AddHadronicId(10421);
        PHYSICS->mcConfig().AddHadronicId(10423);
        PHYSICS->mcConfig().AddHadronicId(10431);
        PHYSICS->mcConfig().AddHadronicId(10433);
        PHYSICS->mcConfig().AddHadronicId(10441);
        PHYSICS->mcConfig().AddHadronicId(10443);
        PHYSICS->mcConfig().AddHadronicId(10511);
        PHYSICS->mcConfig().AddHadronicId(10513);
        PHYSICS->mcConfig().AddHadronicId(10521);
        PHYSICS->mcConfig().AddHadronicId(10523);
        PHYSICS->mcConfig().AddHadronicId(10531);
        PHYSICS->mcConfig().AddHadronicId(10533);
        PHYSICS->mcConfig().AddHadronicId(10541);
        PHYSICS->mcConfig().AddHadronicId(10543);
        PHYSICS->mcConfig().AddHadronicId(10551);
        PHYSICS->mcConfig().AddHadronicId(10553);
        PHYSICS->mcConfig().AddHadronicId(20113);
        PHYSICS->mcConfig().AddHadronicId(20213);
        PHYSICS->mcConfig().AddHadronicId(20223);
        PHYSICS->mcConfig().AddHadronicId(20313);
        PHYSICS->mcConfig().AddHadronicId(20323);
        PHYSICS->mcConfig().AddHadronicId(20333);
        PHYSICS->mcConfig().AddHadronicId(20413);
        PHYSICS->mcConfig().AddHadronicId(20423);
        PHYSICS->mcConfig().AddHadronicId(20433);
        PHYSICS->mcConfig().AddHadronicId(20443);
        PHYSICS->mcConfig().AddHadronicId(20513);
        PHYSICS->mcConfig().AddHadronicId(20523);
        PHYSICS->mcConfig().AddHadronicId(20533);
        PHYSICS->mcConfig().AddHadronicId(20543);
        PHYSICS->mcConfig().AddHadronicId(20553);
        PHYSICS->mcConfig().AddHadronicId(100443);
        PHYSICS->mcConfig().AddHadronicId(100553);
        PHYSICS->mcConfig().AddHadronicId(9900440);
        PHYSICS->mcConfig().AddHadronicId(9900441);
        PHYSICS->mcConfig().AddHadronicId(9900443);
        PHYSICS->mcConfig().AddHadronicId(9900551);
        PHYSICS->mcConfig().AddHadronicId(9900553);
        PHYSICS->mcConfig().AddHadronicId(9910441);
        PHYSICS->mcConfig().AddHadronicId(9910551);
    }

    void AddDefaultInvisible()
    {
        // definition of the multiparticle "invisible"
        PHYSICS->mcConfig().AddInvisibleId(-16);
        PHYSICS->mcConfig().AddInvisibleId(-14);
        PHYSICS->mcConfig().AddInvisibleId(-12);
        PHYSICS->mcConfig().AddInvisibleId(12);
        PHYSICS->mcConfig().AddInvisibleId(14);
        PHYSICS->mcConfig().AddInvisibleId(16);
        PHYSICS->mcConfig().AddInvisibleId(1000022);
        PHYSICS->mcConfig().AddInvisibleId(1000039);
    }

 protected :

};

}


#endif
