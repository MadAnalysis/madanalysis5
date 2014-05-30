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


//SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoFastJet.h"
#include "SampleAnalyzer/Commons/Service/LoopService.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>

using namespace MA5;


ClusterAlgoFastJet::ClusterAlgoFastJet(std::string Algo):ClusterAlgoBase(Algo)
{ JetAlgorithm_=Algo; JetDefinition_=0; }

ClusterAlgoFastJet::~ClusterAlgoFastJet() 
{ if (JetDefinition_!=0) delete JetDefinition_; }


void ClusterAlgoFastJet::GetFinalState(const MCParticleFormat* part, std::set<const MCParticleFormat*>& finalstates)
{
  for (unsigned int i=0; i<part->daughters().size(); i++)
  {
    if (PHYSICS->Id->IsFinalState(part->daughters()[i])) finalstates.insert(part->daughters()[i]);
    else return GetFinalState(part->daughters()[i],finalstates);
  }
}

Bool_t ClusterAlgoFastJet::IsLast(const MCParticleFormat* part, EventFormat& myEvent)
{
  for (unsigned int i=0; i<part->daughters().size(); i++)
  {
    if (part->daughters()[i]->pdgid()==part->pdgid()) return false;
  }
  return true;
}


bool ClusterAlgoFastJet::Execute(SampleFormat& mySample, EventFormat& myEvent)
{


  return true;
}


