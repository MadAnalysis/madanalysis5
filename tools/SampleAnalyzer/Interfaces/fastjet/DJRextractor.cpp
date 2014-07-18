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


//SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/fastjet/DJRextractor.h"
#include "SampleAnalyzer/Commons/Base/Configuration.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/CompilationService.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>

//STL headers
#include <sstream>


using namespace MA5;

/*
extern"C"
{
  void ktclus_(int *imode, double PP[512][4], int* NN, double* ECUT, double Y[512]);
}
*/


bool DJRextractor::Initialize()
{
  // Initializing clustering algorithm
  JetDefinition_ = new fastjet::JetDefinition(fastjet::kt_algorithm,1.0);
  return true;
}


bool DJRextractor::Execute(SampleFormat& mySample, const EventFormat& myEvent, std::vector<Double_t>& DJRvalues)
{
  // Safety
  if (mySample.mc()==0) return false;
  if (myEvent.mc()==0) return false;

  // Preparing inputs
  std::vector<fastjet::PseudoJet> inputs;
  SelectParticles(inputs,myEvent.mc());

  // Getting DJR observables
  ExtractDJR(inputs,DJRvalues);

  return true;
}

void DJRextractor::Finalize()
{
  // Free memory allocation
  if (JetDefinition_==0) delete JetDefinition_;
}




Double_t DJRextractor::rapidity(Double_t px, Double_t py, Double_t pz)
{
  double PTJET = sqrt( px*px + py*py);
  return fabs(log(std::min((sqrt(PTJET*PTJET+pz*pz)+fabs(pz ))/PTJET,1e5)));
}


void DJRextractor::ExtractDJRwithFortran(const std::vector<fastjet::PseudoJet>& inputs,std::vector<Double_t>& DJRvalues)
{
  double PP[512][4];
  UNUSED(PP);
  for (unsigned int i=0;i<inputs.size();i++)
  {
    PP[i][0]=inputs[i].px();
    PP[i][1]=inputs[i].py();
    PP[i][2]=inputs[i].pz();
    PP[i][3]=inputs[i].e();
  }
  //  int IMODE = 4313;
  //  int NN = inputs.size();
  //  if (NN>512) NN=512;
  //  double ECUT=1.;
  double Y[512];
  for (unsigned int i=0;i<512;i++) Y[i]=0;
  //if (NN!=0) ktclus_(&IMODE,PP,&NN,&ECUT,Y);

  for (unsigned int i=0;i<DJRvalues.size();i++)
  DJRvalues[i]=Y[i];
}

void DJRextractor::ExtractDJR(const std::vector<fastjet::PseudoJet>& inputs,std::vector<Double_t>& DJRvalues)
{
  // JetDefinition_
  fastjet::ClusterSequence sequence(inputs, *JetDefinition_);
  for (unsigned int i=0;i<DJRvalues.size();i++) 
  {
    DJRvalues[i]=sequence.exclusive_dmerge(i);
  }
}



/// Selecting particles for non-hadronized events
void DJRextractor::SelectParticles_NonHadronization(std::vector<fastjet::PseudoJet>& inputs, const MCEventFormat* myEvent)
{
  for (unsigned int i=6;i<myEvent->particles().size();i++)
  {
    // Selecting partons (but not top quark)
    if (fabs(myEvent->particles()[i].pdgid())>5 && 
        myEvent->particles()[i].pdgid()!=21) continue;

    // Selecting final states
    if (myEvent->particles()[i].statuscode()==3) continue;

    // Selecting states not coming from initial proton (beam remnant) 
    // or hadronization
    const MCParticleFormat* myPart = &(myEvent->particles()[i]);
    bool test=true;
    while (myPart->mother1()!=0)
    {
      if (myPart->mothup1_==1 || myPart->mothup1_==2)
      { test=false; break;}
      else if (myPart->mothup1_<=6)
      { test=true; break;}
      else if (myPart->mother1()->pdgid()==91 || 
               myPart->mother1()->pdgid()==92)
      {test=false; break;}
      myPart = myPart->mother1();
    }
    if (!test) continue;

    // Cut on the rapidity
    double ETAJET = rapidity(myEvent->particles()[i].momentum().Px(),
                             myEvent->particles()[i].momentum().Py(),
                             myEvent->particles()[i].momentum().Pz());
    if (fabs(ETAJET)>5) continue;
    
    // add the particle
    inputs.push_back(fastjet::PseudoJet ( myEvent->particles()[i].px(), 
                                          myEvent->particles()[i].py(), 
                                          myEvent->particles()[i].pz(), 
                                          myEvent->particles()[i].e() ) );

    // labeling the particle
    inputs.back().set_user_index(i);
  }
}


/// Selecting particles for non-hadronized events
void DJRextractor::SelectParticles(std::vector<fastjet::PseudoJet>& inputs,
                                    const MCEventFormat* myEvent)
{
  for (unsigned int i=6;i<myEvent->particles().size();i++)
  {
    // Selecting partons (but not top quark)
    if (fabs(myEvent->particles()[i].pdgid())>5 && 
        myEvent->particles()[i].pdgid()!=21) continue;

    // Selecting final states
    if (myEvent->particles()[i].statuscode()!=2) continue;

    // Selecting states not coming from initial proton (beam remnant) 
    // or hadronization
    const MCParticleFormat* myPart = &(myEvent->particles()[i]);
    bool test=true;
    while (myPart->mother1()!=0)
    {
      if (myPart->mothup1_==1 || myPart->mothup1_==2)
      { test=false; break;}
      else if (myPart->mothup1_<=6)
      { test=true; break;}
      else if (myPart->mother1()->pdgid()==91 || 
               myPart->mother1()->pdgid()==92)
      {test=false; break;}
      myPart = myPart->mother1();
    }
    if (!test) continue;

    // Cut on the rapidity
    double ETAJET = rapidity(myEvent->particles()[i].momentum().Px(),
                             myEvent->particles()[i].momentum().Py(),
                             myEvent->particles()[i].momentum().Pz());
    if (fabs(ETAJET)>5) continue;

    // Remove double counting
    if (myEvent->particles()[i].mother1()!=0 && myPart->mother2()==0)
    {
      if (myEvent->particles()[i].pdgid()==myEvent->particles()[i].mother1()->pdgid() &&
          myEvent->particles()[i].statuscode()==myEvent->particles()[i].mother1()->statuscode() &&
          fabs(myEvent->particles()[i].px()-myEvent->particles()[i].mother1()->px())<1e-04 &&
          fabs(myEvent->particles()[i].py()-myEvent->particles()[i].mother1()->py())<1e-04 &&
          fabs(myEvent->particles()[i].pz()-myEvent->particles()[i].mother1()->pz())<1e-04 )
        continue;
    }
    
    // add the particle
    inputs.push_back(fastjet::PseudoJet ( myEvent->particles()[i].px(), 
                                          myEvent->particles()[i].py(), 
                                          myEvent->particles()[i].pz(), 
                                          myEvent->particles()[i].e() ) );

    // labeling the particle
    inputs.back().set_user_index(i);
  }
}



