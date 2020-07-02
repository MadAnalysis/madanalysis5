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


// SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/fastjet/DJRextractor.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/CompilationService.h"

// FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>

// STL headers
#include <map>
#include <sstream>
#include <cmath>


using namespace MA5;


MAbool DJRextractor::Initialize()
{
  // Initializing clustering algorithm
  JetDefinition_ = new fastjet::JetDefinition(fastjet::kt_algorithm,1.0);
  return true;
}


MAbool DJRextractor::Execute(SampleFormat& mySample, const EventFormat& myEvent, std::vector<MAdouble64>& DJRvalues)
{
  // Safety
  if (mySample.mc()==0) return false;
  if (myEvent.mc()==0) return false;

  // Preparing inputs
  std::vector<fastjet::PseudoJet> inputs;
  SelectParticles(inputs,myEvent.mc());

  // Extract the DJR values
  ExtractDJR(inputs,DJRvalues);

  return true;
}

void DJRextractor::Finalize()
{
  // Free memory allocation
  if (JetDefinition_==0) delete JetDefinition_;
}


MAdouble64 DJRextractor::rapidity(MAdouble64 px, MAdouble64 py, MAdouble64 pz)
{
  MAfloat64 PTJET = sqrt( px*px + py*py);
  return std::abs(log(std::min((sqrt(PTJET*PTJET+pz*pz)+std::abs(pz ))/PTJET,1e5)));
}


void DJRextractor::ExtractDJR(const std::vector<fastjet::PseudoJet>& inputs,std::vector<MAdouble64>& DJRvalues)
{
  // JetDefinition_
  fastjet::ClusterSequence sequence(inputs, *JetDefinition_);
  for (MAuint32 i=0;i<DJRvalues.size();i++)
  {
    DJRvalues[i]=sequence.exclusive_dmerge(i);
  }
}

/// Selecting particles for non-hadronized events
void DJRextractor::SelectParticles(std::vector<fastjet::PseudoJet>& inputs,
                                    const MCEventFormat* myEvent)
{
  // Indexing
  std::map<const MCParticleFormat*,int> indices;
  for (MAuint32 i=0;i<myEvent->particles().size();i++)
    indices[&(myEvent->particles()[i])]=i;

  // The main routine
  std::map<const MCParticleFormat*,MAbool> filters;
  for (MAuint32 i=0;i<myEvent->particles().size();i++)
  {
    if (myEvent->particles()[i].mothers().size()==0) continue;
    if (myEvent->particles()[i].mothers()[0]->mothers().size()==0) continue;

    std::vector<MCParticleFormat*> family=myEvent->particles()[i].mothers()[0]->daughters();
    // Filters
    if(myEvent->particles()[i].mothers().size()>1) filters[&(myEvent->particles()[i])] = false;
    else if(filters.find(myEvent->particles()[i].mothers()[0])!=filters.end())
    {
      const MCParticleFormat* part=&(myEvent->particles()[i]);
      // The mother is already filtered (easy)
      if(filters[part->mothers()[0]]) filters[part]=true;
      // This is not a radiation or decay pattern -> let's keep it
      else if(part->mothers()[0]->daughters().size()<2) filters[part]=false;
      // The mother is not filtered -> testing if we have a radiation pattern
      else
      {
        // Get all brothers and sisters and kill MAfloat64s
        for(MAint32 j=family.size()-1;j>0;j--)
        {
          if (indices[family[j]] ==indices[part]) { family.erase(family.begin()+j); continue; }
          for(MAuint32 k=j+1;k<family.size();k++)
            if (indices[family[j]]==indices[family[k]]) { family.erase(family.begin()+k); break; }
        }
        // Checking whether we have partons in the family
        MAuint32 ng=0, ninit=0, nq=0,nqb=0;
        if(part->pdgid()==part->mothers()[0]->pdgid()) ninit++;
        for(MAuint32 i=0; i< family.size();i++)
        {
          if(family[i]->pdgid()<= 4 && family[i]->pdgid()>0) nq++;
          if(family[i]->pdgid()>=-4 && family[i]->pdgid()<0) nqb++;
          if(family[i]->pdgid()==21) ng++;
          if(family[i]->pdgid()==part->mothers()[0]->pdgid()) ninit++;
        }
        MAbool condition1 = part->mothers()[0]->pdgid()!=21 && ninit>0;
        MAbool condition2 = part->mothers()[0]->pdgid()==21 && (ng>=2 || (nqb>0 && nq>0));
        if(!condition1 && !condition2) filters[part]=true;
        else                           filters[part]=false;
      }
    }
    else filters[&(myEvent->particles()[i])]=false;

    // Selecting partons (but not top quark)
    if (std::abs(myEvent->particles()[i].pdgid())>6 && 
        myEvent->particles()[i].pdgid()!=21) continue;


    // Selecting radiative states
    if (myEvent->particles()[i].statuscode()!=2 &&
        myEvent->particles()[i].statuscode()!=71) continue;

    // Selecting states not coming from initial proton (beam remnant) 
    // or hadronization
    const MCParticleFormat* myPart = &(myEvent->particles()[i]);
    MAbool test=true;
    while (myPart->mothers().size()!=0)
    {
      const MCParticleFormat* myMum = myPart->mothers()[0];
       
      if (myMum==&(myEvent->particles()[0]) || myMum==&(myEvent->particles()[1]))
      { test=false; break;}
      else if (myMum==&(myEvent->particles()[2]) || myMum==&(myEvent->particles()[3]) ||
               myMum==&(myEvent->particles()[4]) || myMum==&(myEvent->particles()[5]))
      { test=true; break;}
      else if (myPart->mothers()[0]->pdgid()==91 || 
               myPart->mothers()[0]->pdgid()==92)
      {test=false; break;}
      myPart = myPart->mothers()[0];
    }
    if (!test) continue;

    // Cut on the rapidity
    MAfloat64 ETAJET = rapidity(myEvent->particles()[i].momentum().Px(),
                             myEvent->particles()[i].momentum().Py(),
                             myEvent->particles()[i].momentum().Pz());
    if (std::abs(ETAJET)>5) continue;

    // Remove MAfloat64 counting
    if (myEvent->particles()[i].mothers().size()==1)
    {
      if (myEvent->particles()[i].pdgid()      == myEvent->particles()[i].mothers()[0]->pdgid() &&
          myEvent->particles()[i].statuscode() == myEvent->particles()[i].mothers()[0]->statuscode() &&
          std::abs(myEvent->particles()[i].px()-myEvent->particles()[i].mothers()[0]->px())<1e-04 &&
          std::abs(myEvent->particles()[i].py()-myEvent->particles()[i].mothers()[0]->py())<1e-04 &&
          std::abs(myEvent->particles()[i].pz()-myEvent->particles()[i].mothers()[0]->pz())<1e-04 )
        continue;
    }

    if(!filters[&(myEvent->particles()[i])])
    {
      // add the particle
      inputs.push_back(fastjet::PseudoJet ( myEvent->particles()[i].px(), 
                                            myEvent->particles()[i].py(), 
                                            myEvent->particles()[i].pz(), 
                                            myEvent->particles()[i].e() ) );

      // labeling the particle
      inputs.back().set_user_index(i);
    }
  }
}

