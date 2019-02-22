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
#include "SampleAnalyzer/Commons/Service/LoopService.h"

using namespace MA5;

/// Initializing the static member 
LoopService* LoopService::Service_ = 0;
const MAuint32 LoopService::NcallThreshold_ = 100000;


// -----------------------------------------------------------------------------
// IrrelevantPhoton
// -----------------------------------------------------------------------------
MAbool LoopService::IrrelevantPhoton_core(const MCParticleFormat* part, 
                                          const SampleFormat& mySample)
{
  // Safety
  if (ReachThreshold()) return false;

  // Reach the initial State ? -> end
  if (part->mothers().size()==0) return false;

  // Patch for Herwig
  if (mySample.sampleGenerator()==MA5GEN::HERWIG6) 
    if (part->mothers()[0]->statuscode()==103 || 
        part->mothers()[0]->statuscode()==110 ||
        part->mothers()[0]->statuscode()==120) return false;

  // Checking mother
  MAuint32 absid = std::abs(part->mothers()[0]->pdgid());

  if (absid==15) return true;

  // BENJ: this is special for HERWIG
  /*  else if(part->mother1()->mother1()!=0)
      {
      if (part->mother1()->mother1()->pdgid()==82) return false;
      else if (part==part->mother1()->mother1())   return false;
      }*/
  // BENJ: end of herwig fix

  else return IrrelevantPhoton_core(part->mothers()[0],mySample);

  // Default
  return false;
}



// -----------------------------------------------------------------------------
// ComingFromHadronDecay
// -----------------------------------------------------------------------------
MAbool LoopService::ComingFromHadronDecay_core(const MCParticleFormat* part, 
                                                const SampleFormat& mySample)
{
  // Safety
  if (ReachThreshold()) return false;

  // Weird case ? Safety: removing this case
  if (part->mothers().size()==0) return true;
//   std::cout << "  [][][][] part id=" << part->pdgid() << "\tstatus=" << part->statuscode() << "\tmother=" << part->mothers().size() << std::endl;

  // Patch for Herwig
  if (mySample.sampleGenerator()==MA5GEN::HERWIG6)
    if (part->mothers()[0]->statuscode()==103 || 
        part->mothers()[0]->statuscode()==110 ||
        part->mothers()[0]->statuscode()==120) return false;

  // Weird case
  //  if (part->mother1()==part) { std::cout << "exit" << std::endl; exit(0); }

  // Checking if hard-scattering objects directly generated from the initial state
  if(part->mothers().size()==2 && part->mothers()[0]->statuscode()==21 && part->mothers()[1]->statuscode()==21) return false;

  // Checking if mother is hadron
  MAbool had = PHYSICS->Id->IsHadronic(part->mothers()[0]->pdgid()) && part->mothers()[0]->pdgid()!=21;

  // First case : initial parton
  if (had && part->mothers()[0]->mothers().size()==0) return false;

  // Second case: hadron
  else if (had) return true;

  else return ComingFromHadronDecay(part->mothers()[0],mySample);
}
