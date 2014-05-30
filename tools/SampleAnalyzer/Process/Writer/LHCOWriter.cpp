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


// STL headers
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/LHCOWriter.h"

using namespace MA5;


/// Read the sample
bool LHCOWriter::WriteHeader(const SampleFormat& mySample)
{
  // MA5 logo
  WriteMA5header();

  // LHCO format tag
  *output_ << "#<MA5Format> LHCO format </MA5Format>" << std::endl;

  // Python interface version
  *output_ << "#<MadAnalysis5Version> " << cfg_->GetPythonInterfaceVersion() 
           << " " << cfg_->GetPythonInterfaceDate() 
           << "</MadAnalysis5Version>" << std::endl;

  // SampleAnalyzer version
  *output_ << "#<SampleAnalyzerVersion> "<< cfg_->GetSampleAnalyzerVersion()
           << " " << cfg_->GetSampleAnalyzerVersion()
           << " </SampleAnalyzerVersion>" << std::endl;

  // Explanation about the LHCO
  *output_ << "#<FormatDescription>" << std::endl;
  *output_ << "#################################################################################" << std::endl;
  *output_ << "# Each event begins with a header row. Meaning of the different columns:        #" << std::endl;
  *output_ << "# the following:                                                                #" << std::endl;
  *output_ << "# - column 1: label '0' indicating the beginning a new event.                   #" << std::endl;
  *output_ << "# - column 2: event number.                                                     #" << std::endl;
  *output_ << "# - column 3: triggering information. For files produced by MadAnalysis, this   #" << std::endl;
  *output_ << "#             value is set to 0.                                                #" << std::endl;
  *output_ << "#                                                                               #" << std::endl;
  *output_ << "# The rest of the rows are the physics objects in the events. Meaning of the    #" << std::endl;
  *output_ << "# different columns:                                                            #" << std::endl;
  *output_ << "# - column 1: row number                                                        #" << std::endl;
  *output_ << "# - column 2: type of physics object. The possible values are:                  #" << std::endl;
  *output_ << "#               0 = photon, 1 = electron, 2 = muon,                             #" << std::endl;
  *output_ << "#               3 = hadronically-decaying tau, 4 = jet, 6 = MET                 #" << std::endl;
  *output_ << "# - column 3: pseudorapidity (eta) of the physics object                        #" << std::endl;
  *output_ << "# - column 4: azimuthal angle (phi) of the physics object                       #" << std::endl;
  *output_ << "# - column 5: transverse momentum (pt) of the physics object                    #" << std::endl;
  *output_ << "# - column 6: mass of the physics object. Required information to               #" << std::endl;
  *output_ << "#             reconstruct the four-vector momentum of jets.                     #" << std::endl;
  *output_ << "# - column 7:                                                                   #" << std::endl;
  *output_ << "#   + case of a jet: number of tracks associated with the object.               #" << std::endl;
  *output_ << "#   + case of an electron/muon: electric charge.                                #" << std::endl;
  *output_ << "#   + case of an hadronically-decaying tau: electric charge times the number of #" << std::endl;
  *output_ << "#                                           tracks associated with the object.  #" << std::endl;
  *output_ << "#                                           Usual values: +/-1 , +/-3.          #" << std::endl;
  *output_ << "#   + other cases: 0.                                                           #" << std::endl;
  *output_ << "# - column 8:                                                                   #" << std::endl;
  *output_ << "#   + case of a jet: b-jet tag. Allowed values are:                             #" << std::endl;
  *output_ << "#                      0 = no identified b-jet,                                 #" << std::endl;
  *output_ << "#                      1 = b-jet identified by a loose tagger algorithm,        #" << std::endl;
  *output_ << "#                      2 = b-jet identified by a tight tagger algorithm         #" << std::endl;
  *output_ << "#                          (the value 2 is not used by MadAnalysis 5).          #" << std::endl;
  *output_ << "#   + case of a muon: the value indicates the row number of the closest jet     #" << std::endl;
  *output_ << "#                     in DeltaR.                                                #" << std::endl;
  *output_ << "#   + other cases: 0.                                                           #" << std::endl;
  *output_ << "# - column 9:                                                                   #" << std::endl;
  *output_ << "#   + case of jet/electron/photon: ratio of the hadronic versus                 #" << std::endl;
  *output_ << "#                                  electromagnetic energy deposited in the      #" << std::endl;
  *output_ << "#                                  calorimeters cells associated to the object. #" << std::endl;
  *output_ << "#                                  It is typically > 1 for a jet and < 1 for an #" << std::endl;
  *output_ << "#                                  electron or a photon.                        #" << std::endl;
  *output_ << "#   + case of muon: value related to muon isolation. The format of this value   #" << std::endl;
  *output_ << "#                   is XXX.YYY. The integer part (XXX) is the summed transverse #" << std::endl;
  *output_ << "#                   momentum of tracks in a DeltaR-cone (typically DeltaR=0.4), #" << std::endl;
  *output_ << "#                   excluding the muon track. The decimal part (YYY) is the     #" << std::endl;
  *output_ << "#                   ratio of the transverse energy in the same DeltaR-cone to   #" << std::endl;
  *output_ << "#                   the transverse momentum of the muon.                        #" << std::endl;
  *output_ << "#   + in other cases: 0.                                                        #" << std::endl;
  *output_ << "# - column 10: empty column, added for possible use.                            #" << std::endl;
  *output_ << "# - column 11: empty column, added for possible use.                            #" << std::endl;
  *output_ << "#################################################################################" << std::endl;
  *output_ << "#</FormatDescription>" << std::endl;

  if (mySample.mc()!=0)
  {
    *output_ << "#Original header:" << std::endl;
    *output_ << "" << std::endl;

    for (unsigned int i=0;i<mySample.header().size();i++)
      *output_ << "#" << mySample.header()[i] << std::endl;
  }

  // One line to remind the meaning of each line
  *output_ << "  #" << std::endl;
  *output_ << LHCOParticleFormat::header << std::endl;

  return true;
}

/// Read the event
bool LHCOWriter::WriteEvent(const EventFormat& myEvent, 
                           const SampleFormat& mySample)
{
  // FirstEvent
  if (counter_==0)
  {
    WriteHeader(mySample);
  }

  // Skipping event if no reconstructed object
  if (myEvent.rec()==0) return true;

  // Event header
  LHCOParticleFormat::WriteEventHeader(counter_,output_);

  // Particle container in LHCO format
  std::vector<LHCOParticleFormat> PartTable;
  PartTable.reserve( myEvent.rec()->photons().size() +
                     myEvent.rec()->electrons().size() +
                     myEvent.rec()->muons().size() + 
                     myEvent.rec()->taus().size() + 
                     myEvent.rec()->jets().size() + 1 /*MET*/);

  // Writing photons (=0)
  for (unsigned int i=0;i<myEvent.rec()->photons().size();i++)
  {
    PartTable.push_back(LHCOParticleFormat());
    WritePhoton(myEvent.rec()->photons()[i],&PartTable.back());
  } 

  // Writing electrons (=1)
  for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
  {
    PartTable.push_back(LHCOParticleFormat());
    WriteElectron(myEvent.rec()->electrons()[i],&PartTable.back());
  } 

  // Writing muons (=2)
  for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
  {
    PartTable.push_back(LHCOParticleFormat());
    WriteMuon(myEvent.rec()->muons()[i],&PartTable.back(),myEvent.rec(),
              myEvent.rec()->photons().size()+
              myEvent.rec()->electrons().size()+
              myEvent.rec()->muons().size());
  }

  // Writing taus (=3)
  for (unsigned int i=0;i<myEvent.rec()->taus().size();i++)
  {
    PartTable.push_back(LHCOParticleFormat());
    WriteTau(myEvent.rec()->taus()[i],&PartTable.back());
  }
    
  // Writing jets (=4)
  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    PartTable.push_back(LHCOParticleFormat());
    WriteJet(myEvent.rec()->jets()[i],&PartTable.back());
  }

  // Writing MET (=6)
  PartTable.push_back(LHCOParticleFormat());
  WriteMET(myEvent.rec()->MET(),&PartTable.back());

  // Printing the table
  for (UInt_t i=0;i<PartTable.size();i++) PartTable[i].Print(i+1,output_);

  // Incremeting event counter
  counter_++;

  return true;
}




/// Finalize the event
bool LHCOWriter::WriteFoot(const SampleFormat& mySample)
{
  return true;
}


void LHCOWriter::WriteJet(const RecJetFormat& jet,
                          LHCOParticleFormat* lhco)
{
  lhco->id    = 4;
  lhco->eta   = jet.momentum().Eta();
  lhco->phi   = jet.momentum().Phi();
  lhco->pt    = jet.momentum().Pt();
  lhco->jmass = jet.momentum().M();
  lhco->ntrk  = jet.ntracks();
  if (jet.btag()) lhco->btag = 1.0; else lhco->btag = 0.0;
  lhco->hadem = jet.HEoverEE();
}

void LHCOWriter::WriteMuon(const RecLeptonFormat& muon,
                           LHCOParticleFormat* lhco, 
                           const RecEventFormat* myEvent,
                           unsigned int npart)
{
  lhco->id    = 2;
  lhco->eta   = muon.momentum().Eta();
  lhco->phi   = muon.momentum().Phi();
  lhco->pt    = muon.momentum().Pt();
  lhco->jmass = muon.momentum().M();
  if (muon.charge()>0) lhco->ntrk=+1.; else lhco->ntrk=-1.;

  //------------- the closest jet ---------------
  unsigned int theClosestJet=0;
  Double_t minDeltaR=-1;
  for (unsigned int i=0;i<myEvent->jets().size();i++)
  {
    if (myEvent->jets()[i].pt()==0) continue;
    Double_t DeltaR=muon.dr(myEvent->jets()[i]);
    if (i==0 || DeltaR<minDeltaR)
    {
      theClosestJet=i;
      minDeltaR=DeltaR;
    }
  }  
  if (minDeltaR<0) lhco->btag = 0.;
  else lhco->btag = theClosestJet+npart+1;

  //---------------- isolation ------------------

  // isolation : sumPT_isol
  double isolation = 0;
  isolation += std::floor(muon.sumPT_isol());

  // isolation : sumET_isol
  double ET_PT = 0;
  if (muon.pt()!=0) ET_PT=muon.sumET_isol()/muon.pt();

  // gathering isolation variables
  if (ET_PT>100) ET_PT=0.99; else ET_PT=ET_PT/100.;

  isolation+=ET_PT;
  lhco->hadem = isolation;
}

void LHCOWriter::WriteElectron(const RecLeptonFormat& electron, 
                               LHCOParticleFormat* lhco)
{
  lhco->id    = 1;
  lhco->eta   = electron.momentum().Eta();
  lhco->phi   = electron.momentum().Phi();
  lhco->pt    = electron.momentum().Pt();
  lhco->jmass = electron.momentum().M();
  if (electron.charge()>0) lhco->ntrk=+1.; else lhco->ntrk=-1.;
  lhco->btag = 0.;
  lhco->hadem = electron.HEoverEE();
}

void LHCOWriter::WritePhoton(const RecPhotonFormat& photon, 
                             LHCOParticleFormat* lhco)
{
  lhco->id    = 0;
  lhco->eta   = photon.momentum().Eta();
  lhco->phi   = photon.momentum().Phi();
  lhco->pt    = photon.momentum().Pt();
  lhco->jmass = photon.momentum().M();
  lhco->ntrk  = 0.;
  lhco->btag  = 0.;
  lhco->hadem = photon.HEoverEE();
}

void LHCOWriter::WriteTau(const RecTauFormat& tau, 
                          LHCOParticleFormat* lhco)
{
  lhco->id    = 3;
  lhco->eta   = tau.momentum().Eta();
  lhco->phi   = tau.momentum().Phi();
  lhco->pt    = tau.momentum().Pt();
  lhco->jmass = tau.momentum().M();
  if (tau.charge()>0) lhco->ntrk=tau.ntracks(); else lhco->ntrk=-tau.ntracks();
  lhco->btag = 0.;
  lhco->hadem = tau.HEoverEE();
}

void LHCOWriter::WriteMET(const ParticleBaseFormat& met, 
                          LHCOParticleFormat* lhco)
{
  lhco->id    = 6;
  lhco->eta   = 0.;
  lhco->phi   = met.phi();
  lhco->pt    = met.pt();
}
