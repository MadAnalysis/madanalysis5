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

#ifdef FASTJET_USE

//SampleAnalyzer headers
#include "SampleAnalyzer/Process/Analyzer/MergingPlots.h"
#include "SampleAnalyzer/Interfaces/fastjet/DJRextractor.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/CompilationService.h"


//STL headers
#include <sstream>


using namespace MA5;


bool MergingPlots::Initialize(const Configuration& cfg,
             const std::map<std::string,std::string>& parameters)
{
  // Create a new algo
  algo_ = new DJRextractor();

  // Reading options
  merging_nqmatch_   = 4;
  merging_nosingrad_ = false;
  for (std::map<std::string,std::string>::const_iterator 
       it=parameters.begin();it!=parameters.end();it++)
  {
    if (it->first=="njets")
    {
      std::stringstream str;
      str << it->second;
      str >> merging_njets_;
    }
    else
    {
      WARNING << "parameter '" << it->first 
              << "' is unknown and will be ignored." << endmsg;
    }
  }

  // Initializing DJR plots
  if (merging_njets_==0) 
  {
    ERROR << "number of jets requested for DJR plots is zero" << endmsg;
    return false;
  }
  DJR_.resize(merging_njets_);
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    std::stringstream str;
    str << "DJR" << i+1;
    std::string title;
    str >> title;
    DJR_[i].Initialize(DJR_.size()+1,title);
  }

  // Initialize the algo
  algo_->Initialize();

  return true;
}


bool MergingPlots::Execute(SampleFormat& mySample, const EventFormat& myEvent)
{
  // Getting number of extra jets in the event
  UInt_t njets = ExtractJetNumber(myEvent.mc(),mySample.mc());
  if (njets>merging_njets_) return false;

  // Computing DJRvalues
  std::vector<Double_t> DJRvalues(merging_njets_,0.);
  if (!algo_->Execute(mySample,myEvent,DJRvalues)) return false;

  // Getting results
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    double djr = 0.;
    if (DJRvalues[i]>0) djr = std::log10(sqrt(DJRvalues[i]));
    DJR_[i].total->Fill(djr);
    DJR_[i].contribution[njets]->Fill(djr);
  }

  // Ok
  return true;
}


void MergingPlots::Finalize(const SampleFormat& summary, 
                            const std::vector<SampleFormat>& files)
{
  // Clear the algo
  algo_->Finalize();
  delete algo_;

  // Saving plots into file
  Write_TextFormat(out());

  // Deleting plots
  for (unsigned int i=0;i<DJR_.size();i++) DJR_[i].Finalize();
  DJR_.clear();

}


/// Number of jets
UInt_t MergingPlots::ExtractJetNumber(const MCEventFormat* myEvent, 
                                      MCSampleFormat* mySample)
{
  UInt_t njets=0;
  for (unsigned int i=6;i<myEvent->particles().size();i++)
  {
    const MCParticleFormat* myPart = &myEvent->particles()[i];

    // keep particles generated during the matrix element calculation
    if (myPart->statuscode()!=3) continue;

    // keep only partons
    if (abs(myPart->pdgid())>merging_nqmatch_ && myPart->pdgid()!=21) continue;

    // keep only jets whose mother is one of the initial parton
    if (myPart->mother1()==0) continue;

    // coming from initial state ?
    if (myPart->mothup1_>6 && (myPart->mothup1_==0 || myPart->mothup2_==0)) continue;

    // removing color singlet
    /*
    if (merging_nosingrad_)
    {
      for (unsigned int j=0;j<myEvent->particles().size();j++)
      {
        if (i!=j) continue;

        const MCParticleFormat* myPart2 = &myEvent->particles()[j];
        
        // keep particles generated during the matrix element calculation
        if (myPart2->statuscode()!=3) continue;

        // keep only partons 
        if ( myPart2->pdgid()!=-myPart->pdgid() && 
             (myPart2->pdgid()!=21 && myPart->pdgid()!=21)) continue;

        // only final states

      }
    }
    */

    // count particle
    njets++;    
  }
  /*
  if (njets==3)  
  {
    for (unsigned int i=0;i<myEvent->particles().size();i++)
    {
      if (fabs(myEvent->particles()[i].pdgid())>5 && 
          myEvent->particles()[i].pdgid()!=21) continue;
      if (myEvent->particles()[i].statuscode()==1) continue;

      std::cout << "i=" << i+1 
                << " ; id=" << myEvent->particles()[i].pdgid()
                << " ; s="  << myEvent->particles()[i].statuscode()
                << " ; m1=" << myEvent->particles()[i].mothup1_ 
                << " ; m2=" << myEvent->particles()[i].mothup2_ << std::endl;
    }
    exit(1);
    }
  */
  return njets;  
}


/// Saving merging plots in the text output file
void MergingPlots::Write_TextFormat(SAFWriter& output)
{
  *output.GetStream() << "<MergingPlots>" << std::endl;
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    DJR_[i].total->Write_TextFormat(output.GetStream());
    for (unsigned int j=0;j<DJR_[i].contribution.size();j++)
    {
      DJR_[i].contribution[j]->Write_TextFormat(output.GetStream());
    }
  }
  *output.GetStream() << "</MergingPlots>" << std::endl;
}

#endif
