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

  // Header block
  *output_ << "<header>" << std::endl;
  *output_ << "<!--" << std::endl;
  *output_ << "#*********************************************************************" << std::endl;
  *output_ << "#                                                                    *" << std::endl;
  *output_ << "#           This file has been produced by MadAnalysis 5             *" << std::endl;
  *output_ << "#                                                                    *" << std::endl;
  *output_ << "#....................................................................*" << std::endl;
  *output_ << "" << std::endl;
  *output_ << "<MA5Format> SimplifiedLHE </MA5Format>" << std::endl;
  *output_ << "<FormatDescription>" << std::endl;
  *output_ << "Les Houches Event (LHE) format is defined in hep-ph/0609017" << std::endl;
  *output_ << "" << std::endl;
  *output_ << "The <init> ... </init> contains global information about the samples in one line:" << std::endl;
  *output_ << " IDBMUP1 IDBMUP2 EBMUP1 EBMUP2 PDFGUP1 PDFGUP2 PDFSUP1 PDFSUP1 PDFSUP2 IDWTUP NPRUP" << std::endl;
  *output_ << "with:" << std::endl;
  *output_ << " - IDBMUP1: PDG id of the first beam" << std::endl;
  *output_ << " - IDBMUP2: PDG id of the second beam" << std::endl;
  *output_ << " - EBMUP1:  energy of the first beam" << std::endl;
  *output_ << " - EBMUP2:  energy of the second beam" << std::endl;
  *output_ << " - PDFGUP1: author group of the PDF related to the first beam" << std::endl;
  *output_ << " - PDFGUP2: author group of the PDF related to the second beam" << std::endl;
  *output_ << " - PDFSUP1: set id of the PDF related to the first beam" << std::endl;
  *output_ << " - PDFSUP2: set id of the PDF related to the second beam" << std::endl;
  *output_ << " - IDWTUP:  weighting strategy" << std::endl;
  *output_ << " - NPRUP:   number of physics processes involved during the generation" << std::endl;
  *output_ << "; the following lines give process information (one line per process):" << std::endl;
  *output_ << " XSECUP XERRUP XMAXUP LPRUP" << std::endl;
  *output_<<  "with:" << std::endl;
  *output_ << " -  XSECUP: cross section value" << std::endl;
  *output_ << " -  XERRUP: cross section error" << std::endl;
  *output_ << " -  XMAXUP: maximum event-weight" << std::endl;
  *output_ << " -  LPRUP:  process id" << std::endl;
  *output_ << "" << std::endl;
  *output_ << "Each event is described by a <event> ... </event> block. This block type contains always a line with general information:" << std::endl;
  *output_ << " NUP IDPRUP XWGTUP SCALUP AQEDUP AQCDUP" << std::endl;
  *output_ << "with:" << std::endl;
  *output_ << " - NUP:    number of particles" << std::endl;
  *output_ << " - IDPRUP: process id" << std::endl;
  *output_ << " - XWGTUP: event-weight" << std::endl;
  *output_ << " - SCALUP: scale" << std::endl;
  *output_ << " - AQEDUP: alpha QED" << std::endl;
  *output_ << " - AQCDUP: alpha QCD" << std::endl;
  *output_ << " following by a line per particle:" << std::endl;
  *output_ << " IDUP ISTUP MOTHUP1 MOTHUP2 ICOLUP1 ICOLUP2 PUP1 PUP2 PUP3 PUP4 PUP5 VTIMUP SPINUP" << std::endl;
  *output_ << " - IDUP:    PDG id" << std::endl;
  *output_ << " - ISTUP:   status code" << std::endl;
  *output_ << " - MOTHUP1: index of the first mother particle" << std::endl;
  *output_ << " - MOTHUP2: index of the second mother particle" << std::endl;
  *output_ << " - ICOLUP1: first color tag" << std::endl;
  *output_ << " - ICOLUP2: second color tag" << std::endl;
  *output_ << " - PUP1:    PX [GeV/c]" << std::endl;
  *output_ << " - PUP2:    PY [GeV/c]" << std::endl;
  *output_ << " - PUP3:    PZ [GeV/c]" << std::endl;
  *output_ << " - PUP4:    E  [GeV]" << std::endl;
  *output_ << " - PUP5:    M  [GeV/c^2] (a space-like virtuality is denoted by a negative sign on the mass)" << std::endl;
  *output_ << " - VTIMUP:  ctau" << std::endl;
  *output_ << " - SPINUP:  cosine of the angle between the spin vector of a particle and its three-momentum" << std::endl;

  // Explanation about Simplified LHE
  if (mySample.rec()!=0)
  {
    *output_ << "" << std::endl;
    *output_ << "In the 'simplified LHE' format, three collections of objects are stored for each object:" << std::endl;
    *output_ << " - Objects with StatusCode=-1 : initial partons iteracting" << std::endl;
    *output_ << " - Objects with StatusCode=+3 : particles produced during the hard process" << std::endl;
    *output_ << " - Objects with StatusCode=+1 : reconstructed objects (after applying a fast-simulation detector a after applying a jet-clustering algorithm" << std::endl;  
    *output_ << "In the case where reconstructed objects have been produced by MadAnalysis (e.g. applying a jet-clustering algorithm), the following conventions are adopted:" << std::endl;
    *output_ << " - particle with PDG id = +/-11 : electrons" << std::endl;
    *output_ << "   electrons: all electrons in the final state (including isolated electrons or electrons coming from jets" << std::endl;
    *output_ << " - particle with PDG id = +/-13 : muons." << std::endl;
    *output_ << "   all muons in the final state (including isolated muons or muons coming from jets" << std::endl;
    *output_ << " - particle with PDG id = +/-15: taus decaying hadronically." << std::endl;
    *output_ << "   jets matching with a tau decaying into hadrons in the Monte Carlo histo" << std::endl;
    *output_ << "   identification and misidentification efficiency could be applied." << std::endl;
    *output_ << " - particle with PDG id = 5: b-jets." << std::endl;
    *output_ << "   jets matching a b-quark in the Monte-Carlo history" << std::endl;
    *output_ << "   identification and misidentification efficiency could be applied." << std::endl;
    *output_ << " - particle with PDG id = 21: jets not b-tagged." << std::endl;
    *output_ << "   the jet collection includes also electrons collection and hadronic taus collection" << std::endl;
    *output_ << " - particle with PDG id = 12: MET." << std::endl;
    *output_ << "   the missing transverse energy has been computed as the opposite to the 4-momentum vector sum of jets (including electrons and hadronic taus) and muons." << std::endl; 
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

  unsigned int counter = 0;

  // Writing MC particles : only MC info case
  // -> hypothesis : input = LHE
  if (myEvent.mc()!=0 && myEvent.rec()==0)
  {
    for (unsigned int i=0;i<myEvent.mc()->particles().size();i++) counter ++;
  }

  // Writing MC particles : MC+REC info case
  // -> hypothesis : input = HEP
  if (myEvent.mc()!=0 && myEvent.rec()!=0)
  {
    for (unsigned int i=4;i<myEvent.mc()->particles().size();i++)
      if (myEvent.mc()->particles()[i].statuscode()==3 || 
          ( myEvent.mc()->particles()[i].statuscode()>=21 &&
            myEvent.mc()->particles()[i].statuscode()<=29)
          ) counter++;
  }

  // Writing REC particles
  if (myEvent.rec()!=0)
  {
    if (myEvent.mc()==0)
    {
      for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
        counter++;
      for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
        counter++;
      for (unsigned int i=0;i<myEvent.rec()->taus().size();i++)
        counter++;
    }
    else
    {
      for (unsigned int i=6;i<myEvent.mc()->particles().size();i++)
      {
        if ( (myEvent.mc()->particles()[i].statuscode()==3 || 
              ( myEvent.mc()->particles()[i].statuscode()>=21 &&
                myEvent.mc()->particles()[i].statuscode()<=29))&& (
            fabs(myEvent.mc()->particles()[i].pdgid())==11 ||
            fabs(myEvent.mc()->particles()[i].pdgid())==13 ||
            fabs(myEvent.mc()->particles()[i].pdgid())==15 ))
          {
            counter++;
          }
      }
    }
    for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
      counter++;
    counter++;
  }

  // Writing event global information
  WriteEventHeader(myEvent,counter);

  // Writing MC particles : only MC info case
  // -> hypothesis : input = LHE
  if (myEvent.mc()!=0 && myEvent.rec()==0)
  {
    for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
      WriteParticle(myEvent.mc()->particles()[i],
                    myEvent.mc()->particles()[i].mothup1_,
                    myEvent.mc()->particles()[i].mothup2_);
  }

  // Writing MC particles : MC+REC info case
  // -> hypothesis : input = HEP
  if (myEvent.mc()!=0 && myEvent.rec()!=0)
  {
    for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
    {
      if ( myEvent.mc()->particles()[i].statuscode()>=11 &&
           myEvent.mc()->particles()[i].statuscode()<=19 )
         WriteParticle(myEvent.mc()->particles()[i],0,0,-1);

      else if (i>3 && i<6 && myEvent.mc()->particles()[i].statuscode()==3)
        WriteParticle(myEvent.mc()->particles()[i],0,0,-1);

      else if (myEvent.mc()->particles()[i].statuscode()==3 || 
           ( myEvent.mc()->particles()[i].statuscode()>=21 &&
             myEvent.mc()->particles()[i].statuscode()<=29))
      {
          Int_t moth1 = GetMotherIndex(myEvent.mc()->particles()[i].mothup1_);
          Int_t moth2 = GetMotherIndex(myEvent.mc()->particles()[i].mothup2_);
          WriteParticle(myEvent.mc()->particles()[i],moth1,moth2,3);
      }
    } 
  }

  // Writing REC particles
  if (myEvent.rec()!=0)
  {
    //    if (myEvent.mc()==0)
    {
      for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
        WriteMuon(myEvent.rec()->muons()[i]);
      for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
        WriteElectron(myEvent.rec()->electrons()[i]);
      for (unsigned int i=0;i<myEvent.rec()->taus().size();i++)
        WriteTau(myEvent.rec()->taus()[i]);
    }
    /*
    else
    {
      for (unsigned int i=6;i<myEvent.mc()->particles().size();i++)
      {
        if ((myEvent.mc()->particles()[i].statuscode()==3 || 
            ( myEvent.mc()->particles()[i].statuscode()>=21 &&
              myEvent.mc()->particles()[i].statuscode()<=29)) && (
            fabs(myEvent.mc()->particles()[i].pdgid())==11 ||
            fabs(myEvent.mc()->particles()[i].pdgid())==13 ||
            fabs(myEvent.mc()->particles()[i].pdgid())==15 ))
          {
            WriteParticle(myEvent.mc()->particles()[i],0,0,1);
          }
      }
    }*/

    for (unsigned int i=0;i<myEvent.rec()->jets().size();i++)
      WriteJet(myEvent.rec()->jets()[i]);
    WriteMET(myEvent.rec()->MET());
  }

  // Event foot
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
bool LHEWriter::WriteParticle(const MCParticleFormat& myPart, Int_t mother1, Int_t mother2, Int_t statuscode )
{
  *output_ << std::setw(9)  << std::right << myPart.pdgid_ << " ";
  if (statuscode!=0)   *output_ << std::setw(4)  << std::right << statuscode << " ";
  else *output_ << std::setw(4)  << std::right << myPart.statuscode_ << " ";
  *output_ << std::setw(4)  << std::right << mother1 /*myPart.mothup1_*/ << " ";
  *output_ << std::setw(4)  << std::right << mother2 /*myPart.mothup2_*/ << " ";
  *output_ << std::setw(4)  << std::right << 0 << " ";
  *output_ << std::setw(4)  << std::right << 0 << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(myPart.momentum_.Px()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(myPart.momentum_.Py()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(myPart.momentum_.Pz()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(myPart.momentum_.E())  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(myPart.momentum_.M())  << " "; 
  *output_ << std::setw(2)  << std::right << std::showpoint << myPart.ctau_ << " ";
  *output_ << std::setw(3)  << std::right << std::showpoint << myPart.spin_;
  *output_ << std::endl;
  return true;
}


void LHEWriter::WriteJet(const RecJetFormat& jet)
{
  if (jet.btag()) *output_ << std::setw(9)  << std::right << 5 << " ";
  else *output_ << std::setw(9)  << std::right << 21 << " ";
  *output_ << std::setw(4)  << std::right << 1  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(jet.momentum().Px()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(jet.momentum().Py()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(jet.momentum().Pz()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(jet.momentum().E())  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(jet.momentum().M())  << " "; 
  *output_ << std::setw(2)  << std::right << std::showpoint << 0. << " ";
  *output_ << std::setw(3)  << std::right << std::showpoint << 0.;
  *output_ << std::endl;
}

void LHEWriter::WriteMuon(const RecLeptonFormat& muon)
{
  if (muon.charge()>0) *output_ << std::setw(9)  << std::right << -13 << " ";
  else *output_ << std::setw(9)  << std::right << +13 << " ";
  *output_ << std::setw(4)  << std::right << 1  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(muon.momentum().Px()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(muon.momentum().Py()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(muon.momentum().Pz()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(muon.momentum().E())  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(muon.momentum().M())  << " "; 
  *output_ << std::setw(2)  << std::right << std::showpoint << 0. << " ";
  *output_ << std::setw(3)  << std::right << std::showpoint << 0.;
  *output_ << std::endl;
}

void LHEWriter::WriteElectron(const RecLeptonFormat& electron)
{
  if (electron.charge()>0) *output_ << std::setw(9)  << std::right << -11 << " ";
  else *output_ << std::setw(9)  << std::right << +11 << " ";
  *output_ << std::setw(4)  << std::right << 1  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(electron.momentum().Px()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(electron.momentum().Py()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(electron.momentum().Pz()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(electron.momentum().E())  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(electron.momentum().M())  << " "; 
  *output_ << std::setw(2)  << std::right << std::showpoint << 0. << " ";
  *output_ << std::setw(3)  << std::right << std::showpoint << 0.;
  *output_ << std::endl;
}

void LHEWriter::WriteTau(const RecTauFormat& tau)
{
  if (tau.charge()>0) *output_ << std::setw(9)  << std::right << -15 << " ";
  else *output_ << std::setw(9)  << std::right << +15 << " ";
  *output_ << std::setw(4)  << std::right << 1  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(tau.momentum().Px()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(tau.momentum().Py()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(tau.momentum().Pz()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(tau.momentum().E())  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(tau.momentum().M())  << " "; 
  *output_ << std::setw(2)  << std::right << std::showpoint << 0. << " ";
  *output_ << std::setw(3)  << std::right << std::showpoint << 0.;
  *output_ << std::endl;
}

void LHEWriter::WriteMET(const ParticleBaseFormat& met)
{
  *output_ << std::setw(9)  << std::right << 12 << " ";
  *output_ << std::setw(4)  << std::right << 1  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(4)  << std::right << 0  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(met.px()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(met.py()) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(0.) << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(met.pt())  << " ";
  *output_ << std::setw(18) << std::right << LHEWriter::FortranFormat_DoublePrecision(0.)  << " "; 
  *output_ << std::setw(2)  << std::right << std::showpoint << 0. << " ";
  *output_ << std::setw(3)  << std::right << std::showpoint << 0.;
  *output_ << std::endl;
}
