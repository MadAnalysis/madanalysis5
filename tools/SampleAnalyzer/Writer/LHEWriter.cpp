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

// SampleAnalyzer headers
#include "SampleAnalyzer/Writer/LHEWriter.h"

using namespace MA5;

std::string LHEWriter::FortranFormat_SimplePrecision(Float_t value,UInt_t precision)
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


std::string LHEWriter::FortranFormat_DoublePrecision(Double_t value,UInt_t precision)
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
bool LHEWriter::WriteHeader(const SampleFormat& mySample)
{
  // Opening tag
  *output_ << "<LesHouchesEvents version=""1.0"">" << std::endl;

  // Header tag
  *output_ << "<header>" << std::endl;
  *output_ << "<!--" << std::endl;

  // MA5 logo
  WriteMA5header();

  // LHE format
  if (mySample.rec()!=0)
  {
    *output_ << "<MA5Format> Simplified LHE format </MA5Format>" << std::endl;
  }
  else
  {
    *output_ << "<MA5Format> LHE format </MA5Format>" << std::endl;
  }

  // Explanation about the LHE
  *output_ << "<FormatDescription>" << std::endl;
  *output_ << "#################################################################################" << std::endl;
  *output_ << "# The original Les Houches Event (LHE) format is defined in hep-ph/0609017      #" << std::endl;
  *output_ << "#################################################################################" << std::endl;
  *output_ << "# The <init> ... </init> block contains global information about the samples    #" << std::endl;
  *output_ << "# given as a single line:                                                       #" << std::endl;
  *output_ << "#    IDBM1 IDBM2 EBM1 EBM2 PDFG1 PDFG2 PDFS1 PDFS1 PDFS2 IDWT NPR               #" << std::endl;
  *output_ << "# with:                                                                         #" << std::endl;
  *output_ << "#   - IDBM1: PDG code of the first beam.                                        #" << std::endl;
  *output_ << "#   - IDBM2: PDG code of the second beam.                                       #" << std::endl;
  *output_ << "#   - EBM1:  energy of the first beam.                                          #" << std::endl;
  *output_ << "#   - EBM2:  energy of the second beam.                                         #" << std::endl;
  *output_ << "#   - PDFG1: author group of the PDF employed for the first beam.               #" << std::endl;
  *output_ << "#   - PDFG2: author group of the PDF employed for the second beam.              #" << std::endl;
  *output_ << "#   - PDFS1: id of the PDF set employed for the first beam.                     #" << std::endl;
  *output_ << "#   - PDFS2: id of the PDF set employed for the second beam.                    #" << std::endl;
  *output_ << "#   - IDWT:  weighting strategy.                                                #" << std::endl;
  *output_ << "#   - NPR:   number of physics processes involved during the generation of      #" << std::endl;
  *output_ << "#            the sample.                                                        #" << std::endl;
  *output_ << "# The following lines give detailed process information (one line for each      #" << std::endl;
  *output_ << "# process):                                                                     #" << std::endl;
  *output_ << "#    XSEC XERR XMAX LPR                                                         #" << std::endl;
  *output_ << "# with:                                                                         #" << std::endl;
  *output_ << "#   -  XSEC: cross section                                                      #" << std::endl;
  *output_ << "#   -  XERR: cross section error                                                #" << std::endl;
  *output_ << "#   -  XMAX: maximum event weight                                               #" << std::endl;
  *output_ << "#   -  LPR:  process id                                                         #" << std::endl;
  *output_ << "#################################################################################" << std::endl;
  *output_ << "# Each event is described by an <event> ... </event> block. This block always   #" << std::endl;
  *output_ << "# starts by a single line containing general information on the event:          #" << std::endl;
  *output_ << "#    N IDPR XWGT SCAL AQED AQCD                                                 #" << std::endl;
  *output_ << "# with:                                                                         #" << std::endl;
  *output_ << "#   - N:    number of particles                                                 #" << std::endl;
  *output_ << "#   - IDPR: process id                                                          #" << std::endl;
  *output_ << "#   - XWGT: event weight                                                        #" << std::endl;
  *output_ << "#   - SCAL: scale                                                               #" << std::endl;
  *output_ << "#   - AQED: alpha QED                                                           #" << std::endl;
  *output_ << "#   - AQCD: alpha QCD                                                           #" << std::endl;
  *output_ << "# This line is then followed by one line for each particle in the event:        #" << std::endl;
  *output_ << "#    ID IST MOTH1 MOTH2 ICOL1 ICOL2 P1 P2 P3 P4 P5 VTIM SPIN                    #" << std::endl;
  *output_ << "# with:                                                                         #" << std::endl;
  *output_ << "#   - ID:    PDG code                                                           #" << std::endl;
  *output_ << "#   - IST:   status code                                                        #" << std::endl;
  *output_ << "#   - MOTH1: row number corresponding to the first mother particle              #" << std::endl;
  *output_ << "#   - MOTH2: row number corresponding to the second mother particle             #" << std::endl;
  *output_ << "#   - ICOL1: first color tag                                                    #" << std::endl;
  *output_ << "#   - ICOL2: second color tag                                                   #" << std::endl;
  *output_ << "#   - P1:    PX                                                                 #" << std::endl;
  *output_ << "#   - P2:    PY                                                                 #" << std::endl;
  *output_ << "#   - P3:    PZ                                                                 #" << std::endl;
  *output_ << "#   - P4:    E                                                                  #" << std::endl;
  *output_ << "#   - P5:    M (a space-like virtuality is denoted by a negative mass)          #" << std::endl;
  *output_ << "#   - VTIM:  c tau                                                              #" << std::endl;
  *output_ << "#   - SPIN:  cosine of the angle between the spin vector of the particle and    #" << std::endl;
  *output_ << "#            its three-momentum                                                 #" << std::endl;
  *output_ << "#################################################################################" << std::endl;

  // Explanation about the Simplified LHE
  if (mySample.rec()!=0)
  {
    *output_ << "# In the 'simplified LHE' format, there are three types of objects classified   #" << std::endl;
    *output_ << "# according to their statuscode:                                                #" << std::endl;
    *output_ << "#   - objects with StatusCode = -1: initial interacting partons.                #" << std::endl;
    *output_ << "#   - objects with StatusCode = +3: particles produced during the hard process. #" << std::endl;
    *output_ << "#   - objects with StatusCode = +1: physics objects reconstructed by a fast     #" << std::endl;
    *output_ << "#                                   detector simulation (or perfect detector).  #" << std::endl;
    *output_ << "# When MadAnalysis is in charge of the reconstruction (i.e., applying the       #" << std::endl;
    *output_ << "# jet-clustering algorithm), the particle codes follow the conventions:         #" << std::endl;
    *output_ << "#   - particle with a PDG code = +11 or -11: electrons and positrons.           #" << std::endl;
    *output_ << "#     They can be isolated or not as well as possibly issued from the           #" << std::endl;
    *output_ << "#     hadronization process.                                                    #" << std::endl;
    *output_ << "#   - particle with a PDG code = +13 or -13: muons and antimuons.               #" << std::endl;
    *output_ << "#     They can be isolated or not as well as possibly issued from the           #" << std::endl;
    *output_ << "#     hadronization process.                                                    #" << std::endl;
    *output_ << "#   - particle with a PDG code = +15 or -15: hadronically decaying (anti)taus.  #" << std::endl;
    *output_ << "#     These consist of jets matching a hadronically-decaying tau when           #" << std::endl;
    *output_ << "#     inspecting the Monte Carlo history. (Mis)Identification efficiency can be #" << std::endl;
    *output_ << "#     possibly included.                                                        #" << std::endl;
    *output_ << "#   - particle with a PDG code = 5: b-jets.                                     #" << std::endl;
    *output_ << "#     These consist of jets matching a b-quark when inspecting the Monte Carlo  #" << std::endl;
    *output_ << "#     history. (Mis)Identification efficiency can be possibly included.         #" << std::endl;
    *output_ << "#   - particle with a PDG code = 21: jets which are not b-tagged and taus which #" << std::endl;
    *output_ << "#     are not tau-tagged. The jet collection includes also electrons collection #" << std::endl;
    *output_ << "#     and hadronic taus collection                                              #" << std::endl;
    *output_ << "#   - particle with a PDG code = 12: the missing transverse energy.             #" << std::endl;
    *output_ << "#     The missing transverse energy is computed as opposite to the sum of the   #" << std::endl;
    *output_ << "#     four-momenta of all jets, electrons, muons and hadronic taus.             #" << std::endl;
    *output_ << "#################################################################################" << std::endl;
  }
  *output_ << "</FormatDescription>" << std::endl;
  if (mySample.mc()!=0)
  {
    *output_ << "Original header:" << std::endl;
    *output_ << "" << std::endl;

    for (unsigned int i=0;i<mySample.mc()->header().size();i++)
      *output_ << mySample.mc()->header()[i] << std::endl;
  }
  *output_ << "-->" << std::endl;
  *output_ << "</header>" << std::endl;

  // Init block
  *output_ << "<init>" << std::endl;

  // To fill
  if (mySample.mc()==0)
  {
    *output_ << std::setw(9) << std::right << 0 << " "; // PDGID1
    *output_ << std::setw(8) << std::right << 0 << " "; // PDGID2
    *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(0) << " "; // E1
    *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(0) << " "; // E2
    *output_ << std::setw(1) << std::right << 0 << " "; // PDF1
    *output_ << std::setw(1) << std::right << 0 << " "; // PDF2
    *output_ << std::setw(5) << std::right << 0 << " "; // PDFID1
    *output_ << std::setw(5) << std::right << 0 << " "; // PDFID2
    *output_ << std::setw(1) << std::right << 1 << " "; // WEIGHT
    *output_ << std::setw(2) << std::right << 1;        // NPROCESSES
    *output_ << std::endl; 

    // one process
    *output_ << std::setw(19) << std::right << LHEWriter::FortranFormat_DoublePrecision(1.0) << " ";
    *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(0.0) << " ";
    *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(1.0) << " "; 
    *output_ << std::setw(3) << std::right << 1;
    *output_ << std::endl;
  }
  else
  {
    *output_ << std::setw(9)  << std::right << mySample.mc()->beamPDGID_.first << " ";
    *output_ << std::setw(8)  << std::right << mySample.mc()->beamPDGID_.second << " ";
    *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(mySample.mc()->beamE_.first)  << " ";
    *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(mySample.mc()->beamE_.second) << " ";
    *output_ << std::setw(1)  << std::right << mySample.mc()->beamPDFauthor_.first << " ";
    *output_ << std::setw(1)  << std::right << mySample.mc()->beamPDFauthor_.second << " ";
    *output_ << std::setw(5)  << std::right << mySample.mc()->beamPDFID_.first << " ";
    *output_ << std::setw(5)  << std::right << mySample.mc()->beamPDFID_.second << " ";
    *output_ << std::setw(1)  << std::right << mySample.mc()->weightMode_ << " ";
    if (mySample.mc()->nProcesses_==0) *output_ << std::setw(2)  << std::right << 1; else *output_ << std::setw(2)  << std::right << mySample.mc()->nProcesses_;
    *output_ << std::endl;
    for (unsigned int i=0;i<mySample.mc()->processes_.size();i++)
    {
      *output_ << std::setw(19) << std::right << LHEWriter::FortranFormat_DoublePrecision(mySample.mc()->processes_[i].xsectionMean_)  << " ";
      *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(mySample.mc()->processes_[i].xsectionError_) << " ";
      *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(mySample.mc()->processes_[i].weightMax_)     << " "; 
      *output_ << std::setw(3)  << std::right << mySample.mc()->processes_[i].processId_;
      *output_ << std::endl;
    }

    if (mySample.mc()->processes_.size()==0)
    {
      *output_ << std::setw(19) << std::right << LHEWriter::FortranFormat_DoublePrecision(0)  << " ";
      *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(0) << " ";
      *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(1.0)     << " "; 
      *output_ << std::setw(3)  << std::right << 1;
      *output_ << std::endl;
    }


  } 

  *output_ << "</init>" << std::endl;

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
bool LHEWriter::WriteEvent(const EventFormat& myEvent, 
                           const SampleFormat& mySample)
{
  // FirstEvent
  if (FirstEvent_)
  {
    FirstEvent_=false;
    WriteHeader(mySample);
  }

  // Event header
  *output_ << "<event>" << std::endl;

  // Container for particles
  std::vector<LHEParticleFormat> particles;
  UInt_t counter=0;

  // Writing MC particles : only MC info case
  // -> hypothesis : input = LHE
  if (myEvent.mc()!=0 && myEvent.rec()==0)
  {
    counter += myEvent.mc()->particles().size();
  }

  // Writing MC particles : MC+REC info case
  // -> hypothesis : input = HEP
  else if (myEvent.mc()!=0 && myEvent.rec()!=0)
  {
    for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
    {
      if (myEvent.mc()->particles()[i].statuscode()==3 || 
          ( myEvent.mc()->particles()[i].statuscode()>=21 &&
            myEvent.mc()->particles()[i].statuscode()<=29)
          ) counter++;
    }

  }

  // Writing REC particles
  if (myEvent.rec()!=0)
  {
    counter += myEvent.rec()->muons().size() + 
               myEvent.rec()->electrons().size() +
               myEvent.rec()->taus().size() + 
               myEvent.rec()->photons().size() +
               myEvent.rec()->jets().size() + 1 /*MET*/;
  }

  // Writing event global information
  particles.reserve(counter);
  WriteEventHeader(myEvent,counter);

  // Writing MC particles : only MC info case
  // -> hypothesis : input = LHE
  if (myEvent.mc()!=0 && myEvent.rec()==0)
  {
    for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
    {
      particles.push_back(LHEParticleFormat());
      WriteParticle(myEvent.mc()->particles()[i],
                    myEvent.mc()->particles()[i].mothup1_,
                    myEvent.mc()->particles()[i].mothup2_,
                    0,
                    particles.back());
    }
  }

  // Writing MC particles : MC+REC info case
  // -> hypothesis : input = HEP
  if (myEvent.mc()!=0 && myEvent.rec()!=0)
  {
    for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
    {
      if ( myEvent.mc()->particles()[i].statuscode()==-1 ||
           ( myEvent.mc()->particles()[i].statuscode()>=11 &&
             myEvent.mc()->particles()[i].statuscode()<=19 ))
      {
        particles.push_back(LHEParticleFormat());
        WriteParticle(myEvent.mc()->particles()[i],0,0,-1, particles.back());
      }

      else if (myEvent.mc()->particles()[i].statuscode()==3 || 
           ( myEvent.mc()->particles()[i].statuscode()>=21 &&
             myEvent.mc()->particles()[i].statuscode()<=29))
      {
          particles.push_back(LHEParticleFormat());
          Int_t moth1 = GetMotherIndex(myEvent.mc()->particles()[i].mothup1_);
          Int_t moth2 = GetMotherIndex(myEvent.mc()->particles()[i].mothup2_);
          WriteParticle(myEvent.mc()->particles()[i],moth1,moth2,3, particles.back());
      }
    } 
  }

  // Writing REC particles
  if (myEvent.rec()!=0)
  {
    for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
    {
      particles.push_back(LHEParticleFormat());
      WriteMuon(myEvent.rec()->muons()[i],particles.back());
    }
    for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
    {
      particles.push_back(LHEParticleFormat());
      WriteElectron(myEvent.rec()->electrons()[i],particles.back());
    }
    for (unsigned int i=0;i<myEvent.rec()->taus().size();i++)
    {
      particles.push_back(LHEParticleFormat());
      WriteTau(myEvent.rec()->taus()[i],particles.back());
    }
    for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
    {
      particles.push_back(LHEParticleFormat());
      WriteJet(myEvent.rec()->jets()[i],particles.back());
    }
    for (unsigned int i=0;i<myEvent.rec()->photons().size();i++)
    {
      particles.push_back(LHEParticleFormat());
      WritePhoton(myEvent.rec()->photons()[i],particles.back());
    }
    particles.push_back(LHEParticleFormat());
    WriteMET(myEvent.rec()->MET(),particles.back());
  }

  // Event foot
  for (unsigned int i=0;i<particles.size();i++) particles[i].Print(i+1, output_);
  *output_ << "</event>" << std::endl;
  return true; 
}


/// Finalize the event
bool LHEWriter::WriteFoot(const SampleFormat& mySample)
{
  // FirstEvent
  if (FirstEvent_) return false;

  // Foot
  *output_ << "</LesHouchesEvents>" << std::endl;
  return true;
}


/// Writing event global information
bool LHEWriter::WriteEventHeader(const EventFormat& myEvent,
                                 unsigned int nevents)
{
  if (myEvent.mc()!=0)
  {
    *output_ << std::setw(2)  << std::right << nevents << " ";
    *output_ << std::setw(3)  << std::right << myEvent.mc()->processId_ << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(myEvent.mc()->weight_)   << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(myEvent.mc()->scale_)    << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(myEvent.mc()->alphaQED_) << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(myEvent.mc()->alphaQCD_) << std::endl;
  }
  else
  {
    *output_ << std::setw(2)  << std::right << nevents << " ";
    *output_ << std::setw(3)  << std::right << 1 << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(1.0) << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(0.0) << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(0.0) << " ";
    *output_ << std::setw(14) << std::right << LHEWriter::FortranFormat_SimplePrecision(0.0) << std::endl;
  }
  return true;
}


/// Writing a particle
void LHEWriter::WriteParticle(const MCParticleFormat& myPart, 
                              Int_t mother1, Int_t mother2, 
                              Int_t statuscode, LHEParticleFormat& lhe)
{
  if (statuscode!=0) lhe.status = statuscode;
  else lhe.status = myPart.statuscode_;
  lhe.id      = myPart.pdgid_;
  lhe.mother1 = mother1;
  lhe.mother2 = mother2;
  lhe.color1  = 0;
  lhe.color2  = 0;
  lhe.px      = myPart.momentum().Px();
  lhe.py      = myPart.momentum().Py();
  lhe.pz      = myPart.momentum().Pz();
  lhe.e       = myPart.momentum().E();
  lhe.m       = myPart.momentum().M();
  lhe.ctau    = myPart.ctau_;
  lhe.spin    = myPart.spin_;
}


void LHEWriter::WriteJet(const RecJetFormat& jet, LHEParticleFormat& lhe)
{
  if (jet.btag()) lhe.id = 5; else lhe.id = 21;
  lhe.status  = 1;
  lhe.mother1 = 0;
  lhe.mother2 = 0;
  lhe.color1  = 0;
  lhe.color2  = 0;
  lhe.px      = jet.momentum().Px();
  lhe.py      = jet.momentum().Py();
  lhe.pz      = jet.momentum().Pz();
  lhe.e       = jet.momentum().E();
  lhe.m       = jet.momentum().M();
  lhe.ctau    = 0.;
  lhe.spin    = 0.;
}


void LHEWriter::WriteMuon(const RecLeptonFormat& muon, LHEParticleFormat& lhe)
{
  if (muon.charge()>0) lhe.id = -13; else lhe.id = +13;
  lhe.status  = 1;
  lhe.mother1 = 0;
  lhe.mother2 = 0;
  lhe.color1  = 0;
  lhe.color2  = 0;
  lhe.px      = muon.momentum().Px();
  lhe.py      = muon.momentum().Py();
  lhe.pz      = muon.momentum().Pz();
  lhe.e       = muon.momentum().E();
  lhe.m       = muon.momentum().M();
  lhe.ctau    = 0.;
  lhe.spin    = 0.;
}

void LHEWriter::WriteElectron(const RecLeptonFormat& electron, LHEParticleFormat& lhe)
{
  if (electron.charge()>0) lhe.id = -11; else lhe.id = +11;
  lhe.status  = 1;
  lhe.mother1 = 0;
  lhe.mother2 = 0;
  lhe.color1  = 0;
  lhe.color2  = 0;
  lhe.px      = electron.momentum().Px();
  lhe.py      = electron.momentum().Py();
  lhe.pz      = electron.momentum().Pz();
  lhe.e       = electron.momentum().E();
  lhe.m       = electron.momentum().M();
  lhe.ctau    = 0.;
  lhe.spin    = 0.;
}


void LHEWriter::WritePhoton(const RecPhotonFormat& photon, LHEParticleFormat& lhe)
{
  lhe.id      = 22;
  lhe.status  = 1;
  lhe.mother1 = 0;
  lhe.mother2 = 0;
  lhe.color1  = 0;
  lhe.color2  = 0;
  lhe.px      = photon.momentum().Px();
  lhe.py      = photon.momentum().Py();
  lhe.pz      = photon.momentum().Pz();
  lhe.e       = photon.momentum().E();
  lhe.m       = photon.momentum().M();
  lhe.ctau    = 0.;
  lhe.spin    = 0.;
}


void LHEWriter::WriteTau(const RecTauFormat& tau, LHEParticleFormat& lhe)
{
  if (tau.charge()>0) lhe.id = -15; else lhe.id = +15;
  lhe.status  = 1;
  lhe.mother1 = 0;
  lhe.mother2 = 0;
  lhe.color1  = 0;
  lhe.color2  = 0;
  lhe.px      = tau.momentum().Px();
  lhe.py      = tau.momentum().Py();
  lhe.pz      = tau.momentum().Pz();
  lhe.e       = tau.momentum().E();
  lhe.m       = tau.momentum().M();
  lhe.ctau    = 0.;
  lhe.spin    = 0.;
}


void LHEWriter::WriteMET(const ParticleBaseFormat& met, LHEParticleFormat& lhe)
{
  lhe.id      = 12;
  lhe.status  = 1;
  lhe.mother1 = 0;
  lhe.mother2 = 0;
  lhe.color1  = 0;
  lhe.color2  = 0;
  lhe.px      = met.px();
  lhe.py      = met.py();
  lhe.pz      = 0.;
  lhe.e       = met.pt();
  lhe.m       = 0.;
  lhe.ctau    = 0.;
  lhe.spin    = 0.;
}
