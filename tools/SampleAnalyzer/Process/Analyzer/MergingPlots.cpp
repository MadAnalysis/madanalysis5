////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


// To be included only if fastjet is available
#ifdef FASTJET_USE
// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Analyzer/MergingPlots.h"
#include "SampleAnalyzer/Interfaces/fastjet/DJRextractor.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/CompilationService.h"

// STL headers
#include <sstream>
#include <map>


using namespace MA5;


MAbool MergingPlots::Initialize(const Configuration& cfg,
             const std::map<std::string,std::string>& parameters)
{

  // Create a new region
  Manager()->AddRegionSelection("myregion");

  // Create a new algo
  algo_ = new DJRextractor();

  // Reading options
  merging_nqmatch_   = 4;
  merging_nosingrad_ = false;
  ma5_mode_ = false;

  for (std::map<std::string,std::string>::const_iterator 
       it=parameters.begin();it!=parameters.end();it++)
  {
    if (it->first=="njets")
    {
      std::stringstream str;
      str << it->second;
      str >> merging_njets_;
    }
    else if (it->first=="ma5_mode")
    {
      if (it->second=="1") ma5_mode_=true;
      else if (it->second=="0") ma5_mode_=false;
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
  for (MAuint32 i=0;i<DJR_.size();i++)
  {
    std::stringstream str;
    str << "DJR" << i+1;
    std::string title;
    str >> title;
    DJR_[i].Initialize(DJR_.size()+1,title,Manager());
  }

  // Initialize the algo
  algo_->Initialize();

  return true;
}


MAbool MergingPlots::Execute(SampleFormat& mySample, const EventFormat& myEvent)
{
  // Event weight
  MAfloat64 myEventWeight;
  if(Configuration().IsNoEventWeight()) myEventWeight=1.;
  else if(myEvent.mc()->weight()!=0.) myEventWeight=myEvent.mc()->weight();
  else
  {
    WARNING << "Found one event with a zero weight. Skipping..." << endmsg;
    return false;
  }
  Manager()->InitializeForNewEvent(myEventWeight);

  // Getting number of extra jets in the event
  MAuint32 njets = 0;

  if (!ma5_mode_) // normal mode
  {
    njets = ExtractHardJetNumber(myEvent.mc(),mySample.mc());
  }
  else // ma5 mode
  {
    njets = myEvent.mc()->processId() % 10;
  }
  if (njets>merging_njets_) return false;

  // Computing DJRvalues
  std::vector<MAfloat64> DJRvalues(merging_njets_,0.);
  if (!algo_->Execute(mySample,myEvent,DJRvalues)) return false;

  // Getting results
  for (MAuint32 i=0;i<DJR_.size();i++)
  {
    MAfloat64 djr = 0.;
    if (DJRvalues[i]>0) djr = std::log10(sqrt(DJRvalues[i]));
    std::stringstream str,str2;
    str  << "DJR" << i+1 << "_" << njets << "jet";
    str2 << "DJR" << i+1 << "_total";
    std::string title,title2;
    str  >> title;
    str2 >> title2;
    Manager()->FillHisto(title, djr);
    Manager()->FillHisto(title2, djr);
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
  for (MAuint32 i=0;i<DJR_.size();i++) DJR_[i].Finalize();
  DJR_.clear();

}


/// Number of jets
MAuint32 MergingPlots::ExtractHardJetNumber(const MCEventFormat* myEvent, 
                                      MCSampleFormat* mySample)
{
  MAuint32 njets=0;

  // Indexing
  std::map<const MCParticleFormat*,int> indices;
  for (MAuint32 i=0;i<myEvent->particles().size();i++)
    indices[&(myEvent->particles()[i])]=i;

  // Filters
  std::map<const MCParticleFormat*,MAbool> filters;
  for (MAuint32 i=0;i<myEvent->particles().size();i++)
  {
    const MCParticleFormat* myPart = &myEvent->particles()[i];
    if (myPart->mothers().size()==0) continue;
    if (myPart->mothers()[0]->mothers().size()==0) continue;
    std::vector<MCParticleFormat*> family=myEvent->particles()[i].mothers()[0]->daughters();

    // Filters
    if(myEvent->particles()[i].mothers().size()>1) filters[&(myEvent->particles()[i])] = false;
    else if(filters.find(myEvent->particles()[i].mothers()[0])!=filters.end())
    {
      // The mother is already filtered (easy)
      if(filters[myPart->mothers()[0]]) filters[myPart]=true;
      // This is not a radiation or decay pattern -> let's keep it
      else if(myPart->mothers()[0]->daughters().size()<2) filters[myPart]=false;
      // The mother is not filtered -> testing if we have a radiation pattern
      else
      {
        // Get all brothers and sisters and kill MAfloat64s
        for(MAint32 j=family.size()-1;j>0;j--)
        {
          if (indices[family[j]] ==indices[myPart]) { family.erase(family.begin()+j); continue; }
          for(MAuint32 k=j+1;k<family.size();k++)
            if (indices[family[j]]==indices[family[k]]) { family.erase(family.begin()+k); break; }
        }
        // Checking whether we have partons in the family
        MAuint32 ng=0, ninit=0, nq=0,nqb=0;
        if(myPart->pdgid()==myPart->mothers()[0]->pdgid()) ninit++;
        for(MAuint32 i=0; i< family.size();i++)
        {
          if(family[i]->pdgid()<=4 && family[i]->pdgid()>0) nq++;
          if(family[i]->pdgid()>=-4 && family[i]->pdgid()<0) nqb++;
          if(family[i]->pdgid()==21) ng++;
          if(family[i]->pdgid()==myPart->mothers()[0]->pdgid()) ninit++;
        }
        MAbool condition1 = myPart->mothers()[0]->pdgid()!=21 && ninit>0;
        MAbool condition2 = myPart->mothers()[0]->pdgid()==21 && (ng>=2 || (nqb>0 && nq>0));
        if(!condition1 && !condition2) filters[myPart]=true;
        else                           filters[myPart]=false;
      }
    }
    else filters[&(myEvent->particles()[i])]=false;

    // keep particles generated during the matrix element calculation
    if (myPart->statuscode()!=3 && !(myPart->statuscode()>20 && myPart->statuscode()<30) ) continue;

    // keep only partons
    if (abs(myPart->pdgid())>merging_nqmatch_ && myPart->pdgid()!=21) continue;

    // keep only jets whose mother is one of the initial parton
    if (myPart->mothers().size()==0) continue;

    // coming from initial state ? 6 first particles
    MAbool initial=false;
    for (MAuint32 ind=0;ind<6;ind++)
    {
      if (myPart->mothers()[0]== &(myEvent->particles()[ind]))
      { initial=true; break; }
    }
    if (initial) continue;
    if (myPart->mothers()[0]==0 || myPart->mothers()[1]==0) continue;

    // Pythia 6 format: removing the initial guys
    if(i<6 && *mySample->GeneratorType()==MA5GEN::PYTHIA6) continue;

    //count particle
    if(!filters[myPart])
      njets++;

  }
  return njets;
}


void MergingPlots::Write_TextFormat(SAFWriter& output)
{
}

#endif
