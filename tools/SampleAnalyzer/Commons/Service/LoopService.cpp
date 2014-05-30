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


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LoopService.h"

using namespace MA5;

/// Initializing the static member 
LoopService* LoopService::Service_ = 0;
const UInt_t LoopService::NcallThreshold_ = 100000;


// -----------------------------------------------------------------------------
// IrrelevantPhoton
// -----------------------------------------------------------------------------
Bool_t LoopService::IrrelevantPhoton_core(const MCParticleFormat* part, 
                                          const SampleFormat& mySample)
{
  // Safety
  if (ReachThreshold()) return false;

  // Reach the initial State ? -> end
  if (part->mother1()==0) return false;

  // Patch for Herwig
  if (mySample.sampleGenerator()==MA5GEN::HERWIG6) 
    if (part->mother1()->statuscode()==103 || 
        part->mother1()->statuscode()==110 ||
        part->mother1()->statuscode()==120) return false;

  // Checking mother
  UInt_t absid = std::abs(part->mother1()->pdgid());

  if (absid==15) return true;

  // BENJ: this is special for HERWIG
  /*  else if(part->mother1()->mother1()!=0)
      {
      if (part->mother1()->mother1()->pdgid()==82) return false;
      else if (part==part->mother1()->mother1())   return false;
      }*/
  // BENJ: end of herwig fix

  else return IrrelevantPhoton_core(part->mother1(),mySample);

  // Default
  return false;
}



// -----------------------------------------------------------------------------
// ComingFromHadronDecay
// -----------------------------------------------------------------------------
Bool_t LoopService::ComingFromHadronDecay_core(const MCParticleFormat* part, 
                                                const SampleFormat& mySample)
{
  // Safety
  if (ReachThreshold()) return false;

  // Weird case ? Safety: removing this case
  if (part->mother1()==0) return true;
  //  std::cout << "part id=" << part->pdgid() << "\tstatus=" << part->statuscode() << "\tmother=" << part->mother1()->pdgid() << std::endl;

  // Patch for Herwig
  if (mySample.sampleGenerator()==MA5GEN::HERWIG6)
    if (part->mother1()->statuscode()==103 || 
        part->mother1()->statuscode()==110 ||
        part->mother1()->statuscode()==120) return false;

  // Weird case
  //  if (part->mother1()==part) { std::cout << "exit" << std::endl; exit(0); }

  // Checking if mother is hadron
  Bool_t had = PHYSICS->Id->IsHadronic(part->mother1()->pdgid()) && part->mother1()->pdgid()!=21;

  // First case : initial parton
  if (had && part->mother1()->mother1()==0) return false;

  // Second case: hadron
  else if (had) return true;

  else return ComingFromHadronDecay(part->mother1(),mySample);
}
