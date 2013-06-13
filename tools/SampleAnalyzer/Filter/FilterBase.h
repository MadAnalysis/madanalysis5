////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


#ifndef FILTER_BASE_h
#define FILTER_BASE_h

// SampleAnalyzer headers
#include "SampleAnalyzer/DataFormat/EventFormat.h"
#include "SampleAnalyzer/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Plot/PlotManager.h"
#include "SampleAnalyzer/Counter/CounterManager.h"
#include "SampleAnalyzer/Writer/SAFWriter.h"
#include "SampleAnalyzer/Service/Physics.h"
#include "SampleAnalyzer/Service/LogService.h"
#include "SampleAnalyzer/Core/Configuration.h"

// STL headers
#include <set>
#include <string>
#include <cmath>
#include <map>

// ROOT headers
#include <TTree.h>
#include <TStyle.h>
#include <TLine.h>
#include <TLegend.h>
#include <TFile.h>
#include <TROOT.h>
#include <Rtypes.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TVector.h>
#include <TClonesArray.h>


// initializing MACRO 
#define INIT_FILTER(CLASS,NAME) public: CLASS() {setName(NAME);} virtual ~CLASS() {} private:

namespace MA5
{

class FilterBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 public :

  /// name of the analysis
  std::string name_;

  /// output ROOT file name
  std::string outputName_;

  /// Weighted events mode
  bool weighted_events_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public :

  /// Constructor without argument 
  FilterBase()
  { name_="unknown"; }

  /// Destructor
  virtual ~FilterBase()
  { }

  /// Initialize (common part to all analyses)
  bool PreInitialize(const std::string& outputName, 
                     const Configuration* cfg)
  { 
    outputName_ = outputName;
    weighted_events_ = cfg->IsNoEventWeight();
    return true;
  }

  /// Initialize (specific to the analysis)
  virtual bool Initialize(const Configuration& cfg,
             const std::map<std::string,std::string>& parameters)=0;

  /// PreFinalize
  void PreFinalize(const SampleFormat& summary, 
                   const std::vector<SampleFormat>& samples,
                   SAFWriter& out)
  { }

  /// Finalize
  virtual void Finalize(const SampleFormat& summary, 
                        const std::vector<SampleFormat>& samples,
                        SAFWriter& out)=0;

  /// PreExecute
  void PreExecute(const SampleFormat& mySample,
                  const EventFormat& myEvent)
  { 
    PHYSICS->SetFinalState(myEvent.mc());
    PHYSICS->SetInitialState(myEvent.mc());
  }

  /// Execute
  virtual bool Execute(SampleFormat& mySample,
                       const EventFormat& myEvent)=0;

  /// Accessor to analysis name
  const std::string name() const {return name_;}

  /// Mutator to analysis name
  void setName(const std::string& Name) {name_=Name;}

 protected :

};

}


#endif
