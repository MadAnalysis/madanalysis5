////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
#include "SampleAnalyzer/Writer/LHCOWriter.h"

using namespace MA5;

std::string LHCOWriter::FortranFormat_SimplePrecision(Float_t value,UInt_t precision)
{
  std::stringstream str;
  str.precision(precision);
  std::string word;

  Bool_t negative=false;
  if (value<0) {negative=true; value*=-1;}

  Int_t exponent = 0;
  if (value!=0)
  {
    for (; value > 1.0; exponent++) value/=10.;
    for (; value < 0.0; exponent--) value*=10.;
  }

  str << std::uppercase << std::fixed << value << "E";
  if (exponent>=0) str << "+"; else str << "-";
  if (abs(exponent)<10) str << "0";
  str << abs(exponent);
  str >> word;
  if (!negative) return word;
  else return "-"+word;
}


std::string LHCOWriter::FortranFormat_DoublePrecision(Double_t value,UInt_t precision)
{
  std::stringstream str;
  str.precision(precision);
  std::string word;

  Bool_t negative=false;
  if (value<0) {negative=true; value*=-1;}

  Int_t exponent = 0;
  if (value!=0)
  {
    for (; value > 1.0; exponent++) value/=10.;
    for (; value < 0.0; exponent--) value*=10.;
  }

  str << std::uppercase << std::fixed << value << "E";
  if (exponent>=0) str << "+"; else str << "-";
  if (abs(exponent)<10) str << "0";
  str << abs(exponent);
  str >> word;
  if (!negative) return word;
  else return "-"+word;
}


/// Read the sample
bool LHCOWriter::WriteHeader(const SampleFormat& mySample)
{
  // Opening tag
  *output_ << "<LesHouchesEvents version=""1.0"">" << std::endl;

  // Header block
  *output_ << "<header>" << std::endl;
  *output_ << "<!--" << std::endl;
  *output_ << "#*********************************************************************" << std::endl;
  *output_ << "#                                                                    *" << std::endl;
  *output_ << "#           This file has been produced by MadAnalysis 5             *" << std::endl;
  *output_ << "#                                                                    *" << std::endl;
  *output_ << "#....................................................................*" << std::endl;
  *output_ << "" << std::endl;

  if (mySample.mc()!=0)
  {
    *output_ << "Original header:" << std::endl;
    *output_ << "" << std::endl;

    for (unsigned int i=0;i<mySample.mc()->header().size();i++)
      *output_ << mySample.mc()->header()[i] << std::endl;
  }

  *output_ << "  #" << std::endl;
  // *output_ << " ## More info on LHCO files: "
  //        << "http://cp3wks05.fynu.ucl.ac.be/Manual/lhco.html" << std::endl;
  *output_ << "  #        eventNum triggerWord" << std::endl;
  *output_ << "  # typ ";
  *output_ << std::setw(18) << std::right << "eta";
  *output_ << " ";
  *output_ << std::setw(18) << std::right << "phi";
  *output_ << " ";
  *output_ << std::setw(18) << std::right << "pt";
  *output_ << " ";
  *output_ << std::setw(18) << std::right << "jmass";
  *output_ << " ";
  *output_ << std::setw(5) << std::right << "ntracks";
  *output_ << " ";
  *output_ << std::setw(4) << std::right << "btag";
  *output_ << " ";
  *output_ << std::setw(7) << std::right << "had/em";
  *output_ << " ";
  *output_ << std::setw(4) << std::right << "dum1";
  *output_ << " ";
  *output_ << std::setw(4) << std::right << "dum2" << std::endl;

  return true;
}


Int_t GetMotherIndex(Int_t index)
{
  if (index==0) return 0;
  if (index==1 || index==3 || index==5) return 1;
  else if (index==2 || index==4 || index==6) return 2;
  else return index-4;
}

/// Read the event
bool LHCOWriter::WriteEvent(const EventFormat& myEvent, 
                           const SampleFormat& mySample)
{
  // Event header
  *output_ << "0" << std::endl;

  // Skipping event if no reconstructed object
  if (myEvent.rec()==0) return true;

  // Counter of particles
  unsigned int npart=0;

  // Writing electrons
  for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
  {
    npart++;
    WriteElectron(myEvent.rec()->electrons()[i],npart);
  } 

  // Writing muons
  for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
  {
    npart++;
    WriteMuon(myEvent.rec()->muons()[i],npart);
  }

  // Writing taus
  for (unsigned int i=0;i<myEvent.rec()->taus().size();i++)
  {
    npart++;
    WriteTau(myEvent.rec()->taus()[i],npart);
  }
    
  // Writing jets
  for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
  {
    npart++;
    WriteJet(myEvent.rec()->jets()[i],npart);
  }

  // Writing MET
  npart++;
  WriteMET(myEvent.rec()->MET(),npart);

  return true;
}


/// Finalize the event
bool LHCOWriter::WriteEventHeader(const SampleFormat& mySample,unsigned int numEvent)
{
  // Particle number
  *output_ << std::setw(3) << std::right << 0;
  *output_ << "     ";

  // Event number
  *output_ << std::setw(18) << std::right << numEvent;
  *output_ << " ";

  // Trigger word
  *output_ << std::setw(18) << std::right << 0;

  return true;
}


/// Finalize the event
bool LHCOWriter::WriteFoot(const SampleFormat& mySample)
{
  return true;
}


void LHCOWriter::WriteJet(const RecJetFormat& jet,unsigned int partNum)
{
  // Particle number
  *output_ << std::setw(3) << std::right << partNum;
  *output_ << " ";

  // Particle type
  *output_ << std::setw(3) << std::right << 4;
  *output_ << " ";

  // eta
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(jet.momentum().Eta()) 
           << " ";

  // phi
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(jet.momentum().Phi()) 
           << " ";

  // pt
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(jet.momentum().Pt())
           << " ";

  // mass
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(jet.momentum().M())
           << " ";

  // ntracks
  *output_ << std::setw(5) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(jet.ntracks()) 
           << " ";

  // b tagging
  if (jet.btag()) *output_ << " 1.0 ";
  else *output_ << " 0.0 ";

  // HE/EE
  *output_ << std::setw(7) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(jet.HEoverEE()) 
           << " ";

  // dummy1 & dummy2
  *output_ << "0.00 0.00";

  // end
  *output_ << std::endl;
}

void LHCOWriter::WriteMuon(const RecLeptonFormat& muon,unsigned int partNum)
{
  // Particle number
  *output_ << std::setw(3) << std::right << partNum;
  *output_ << " ";

  // Particle type
  *output_ << std::setw(3) << std::right << 2;
  *output_ << " ";

  // eta
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(muon.momentum().Eta())
           << " ";

  // phi
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(muon.momentum().Phi())
           << " ";

  // pt
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(muon.momentum().Pt())
           << " ";

  // mass
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(muon.momentum().M())
           << " ";

  // electric charge
  if (muon.charge()>0) *output_ << " +1.0 ";
  else *output_ << " -1.0 ";

  // null
  *output_ << "0.0 ";

  // isolation : sumPT_isol
  double isolation = 0;
  isolation += std::floor(muon.sumPT_isol());

  // isolation : sumET_isol
  double ET_PT = 0;
  if (muon.sumPT_isol()!=0) ET_PT=muon.sumET_isol()/muon.sumPT_isol();
  ET_PT=std::floor(ET_PT);

  // gathering isolation variables
  bool test=false;
  for (unsigned int j=0;j<5;j++)
  {
    ET_PT/=10;
    if (ET_PT<1.)
    {
      test=true;
      break;
    }
  }
  if (!test) ET_PT=0;
  isolation+=ET_PT;
  *output_ << std::setw(7) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(isolation) 
           << " ";

  // dummy1 & dummy2
  *output_ << "0.00 0.00";

  // end
  *output_ << std::endl;
}

void LHCOWriter::WriteElectron(const RecLeptonFormat& electron, unsigned int partNum)
{
  // Particle number
  *output_ << std::setw(3) << std::right << partNum;
  *output_ << " ";

  // Particle type
  *output_ << std::setw(3) << std::right << 1;
  *output_ << " ";

  // eta
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(electron.momentum().Eta()) << " ";

  // phi
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(electron.momentum().Phi()) << " ";

  // pt
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(electron.momentum().Pt()) << " ";

  // mass
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(electron.momentum().M())  << " ";

  // electric charge
  if (electron.charge()>0) *output_ << " +1.0 ";
  else *output_ << " -1.0 ";

  // null
  *output_ << "0.0 ";

  // HE/EE
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(electron.HEoverEE()) 
           << " ";

  // dummy1 & dummy2
  *output_ << "0.00 0.00";

  // end
  *output_ << std::endl;
}

void LHCOWriter::WriteTau(const RecTauFormat& tau, unsigned int partNum)
{
  // Particle number
  *output_ << std::setw(3) << std::right << partNum;
  *output_ << " ";

  // Particle type
  *output_ << std::setw(3) << std::right << 3;
  *output_ << " ";

  // eta
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(tau.momentum().Eta()) << " ";

  // phi
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(tau.momentum().Phi()) << " ";

  // pt
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(tau.momentum().Pt()) << " ";

  // mass
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(tau.momentum().M())  << " ";

  // electric charge
  if (tau.charge()>0) *output_ << " +1.0 ";
  else *output_ << " -1.0 ";

  // HE/EE
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(tau.HEoverEE()) << " ";

  // dummy1 & dummy2
  *output_ << "0.00 0.00";

  // end
  *output_ << std::endl;
}

void LHCOWriter::WriteMET(const ParticleBaseFormat& met, unsigned int partNum)
{
  // Particle number
  *output_ << std::setw(3) << std::right << partNum;
  *output_ << " ";

  // Particle type
  *output_ << std::setw(3) << std::right << 6;
  *output_ << " ";

  // no eta for met
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(0) 
           << " ";

  // phi
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(met.phi())
           << " ";

  // et
  *output_ << std::setw(18) << std::right 
           << LHCOWriter::FortranFormat_DoublePrecision(met.et())
           << " ";

  // dummy1 & dummy2
  *output_ << "0.00 0.00";

  // end
  *output_ << std::endl;
}
