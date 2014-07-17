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


//STL headers
#include <sstream>

//SampleHeader headers
#include "SampleAnalyzer/Process/Reader/HEPMCReader.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

using namespace MA5;

// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool HEPMCReader::ReadHeader(SampleFormat& mySample)
{
  // Reset the saved line 
  savedline_="";
  
  // Initialize MC
  mySample.InitializeMC();
  mySample.SetSampleFormat(MA5FORMAT::HEPMC);
  mySample.SetSampleGenerator(MA5GEN::UNKNOWN);
  warnmother_=true;

  // Skipping header line until first event line
  std::string firstWord;
  std::string line;

  while(firstWord!="E")
  {
    // Getting the next non-empty line
    if (!ReadLine(line)) return false;

    // Splitting the line in words
    std::stringstream str;
    str << line;

    // Extracting the first word
    str >> firstWord;
  }
  
  savedline_  = line;

  // Normal end
  return true;
}


// -----------------------------------------------------------------------------
// FinalizeHeader
// -----------------------------------------------------------------------------
bool HEPMCReader::FinalizeHeader(SampleFormat& mySample)
{
  return true;
}

// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type HEPMCReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  // Initializing MC event
  myEvent.InitializeMC();

  Bool_t eventOnGoing=false;

  // Read the saved line
  if (savedline_!="") 
  {
    FillEvent(savedline_, myEvent, mySample);
    eventOnGoing=true;
    savedline_="";
  }

  bool endEvent=false;
  
  // Loop over particle
  while(!endEvent)
  {
    std::string line;

    // Getting a line from the file
    if (!ReadLine(line))
    {
      if (eventOnGoing) return StatusCode::KEEP; else return StatusCode::FAILURE;
    }

    // Splitting the line in words
    std::stringstream str;
    str << line;

    // Extracting the first word
    std::string firstWord;
    str >> firstWord;

    // Is next event ?
    if (firstWord=="E")
    {
      savedline_  = line;
      return StatusCode::KEEP;
    }
    else
    {
      // Decoding the line
      endEvent=!FillEvent(line, myEvent, mySample);
      eventOnGoing=true;
    }
  }

  // Normal end 
  return StatusCode::KEEP;
}



// -----------------------------------------------------------------------------
// FinalizeEvent
// -----------------------------------------------------------------------------
bool HEPMCReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // Computing met, mht, ...
  for (unsigned int i=0; i<myEvent.mc()->particles_.size();i++)
  {
    MCParticleFormat& part = myEvent.mc()->particles_[i];

    // Setting mother
    if (part.extra1_!=part.extra2_)
    {
      unsigned int nmother=0;
      for (unsigned int j=0; j < myEvent.mc()->particles_.size();j++)
      {
        if (i==j) continue;
        if (part.extra1_ == myEvent.mc()->particles_[j].extra2_)
        {
          // set daughter
          myEvent.mc()->particles_[j].daughters_.push_back(&part);

          // set mother
          nmother++;
          if      (nmother==1) part.mother1_=&(myEvent.mc()->particles()[j]);
          else if (nmother==2) part.mother2_=&(myEvent.mc()->particles()[j]);
          else 
          { 
            if (warnmother_) 
            {
              WARNING << "Number of mothers greather than 2 : " << nmother << endmsg; 
              warnmother_=false; 
            }
          }
        }
      }
    }

    // MET, MHT, TET, THT
    if (part.statuscode()==1 && !PHYSICS->Id->IsInvisible(part))
    {
      myEvent.mc()->MET_ -= part.momentum();
      myEvent.mc()->TET_ += part.pt();
      if (PHYSICS->Id->IsHadronic(part))
      {
        myEvent.mc()->MHT_ -= part.momentum();
        myEvent.mc()->THT_ += part.pt(); 
      }
    }
  }

  // Finalize event
  myEvent.mc()->MET_.momentum().SetPz(0.);
  myEvent.mc()->MET_.momentum().SetE(myEvent.mc()->MET_.momentum().Pt());
  myEvent.mc()->MHT_.momentum().SetPz(0.);
  myEvent.mc()->MHT_.momentum().SetE(myEvent.mc()->MHT_.momentum().Pt());

  // Normal end 
  return true; 
}


//------------------------------------------------------------------------------
// FillWeightNames
//------------------------------------------------------------------------------
Bool_t HEPMCReader::FillWeightNames(const std::string& line)
{
  // Splitting line in words
  std::stringstream str;
  str << line ;

  // Getting the first word
  std::string firstWord;
  str >> firstWord;

  // Extracting the number of weights
  int nweights;
  str >> nweights;
  if (nweights<0)
  {
    ERROR << "Number of weights is not correct: " 
          << nweights << endmsg;
    return false;
  }
  if (nweights>=2)
  {
    WARNING << nweights << " event-weights are defined. "
            << "Only the first one will be used." << endmsg;
  }

  // Storing weight names
  weightnames_.clear();
  weightnames_.resize(static_cast<unsigned int>(nweights));

  // Filling weight names
  for (unsigned int i=0;i<weightnames_.size();i++) str >> weightnames_[i];

  return true;
}


//------------------------------------------------------------------------------
// FillHeavyIons
//------------------------------------------------------------------------------
Bool_t HEPMCReader::FillHeavyIons(const std::string& line)
{
  if (line!="") if (firstHeavyIons_) 
     WARNING << "WARNING: HeavyIons block is not "
             << "read by SampleAnalyzer" << endmsg;
  firstHeavyIons_=false;
  return false;
}


//------------------------------------------------------------------------------
// FillEventHeader
//------------------------------------------------------------------------------
Bool_t HEPMCReader::FillEvent(const std::string& line,
                              EventFormat& myEvent, 
                              SampleFormat& mySample)
{
  // Splitting line in words
  std::stringstream str;
  str << line ;

  // Getting the first word
  std::string firstWord;
  str >> firstWord;

  // Event global info
  if(firstWord=="E") FillEventInformations(line, myEvent);

  // Weight names
  else if (firstWord=="N") FillWeightNames(line);

  // Event units
  else if (firstWord=="U") FillUnits(line);

  // Cross section
  else if (firstWord=="C") FillCrossSection(line,mySample);

  // HeavyIon line
  else if (firstWord=="H") FillHeavyIons(line);

  // PDF Info
  else if (firstWord=="F") FillEventPDFInfo(line,mySample,myEvent);

  // Vertex line
  else if (firstWord=="V") FillEventVertexLine(line,myEvent);

  // Particle Line
  else if (firstWord=="P") FillEventParticleLine(line,myEvent);

  // End
  else if (firstWord=="HepMC::IO_GenEvent-END_EVENT_LISTING") return false;

  // Other cases
  else
  {
    // ignore other cases
    WARNING << "HEPMC linecode unknown" << endmsg;
  }

  // Normal end
  return true;
}

// -----------------------------------------------------------------------------
// FillEventInformations
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventInformations(const std::string& line,
                                  EventFormat& myEvent)
{
  std::stringstream str;
  str << line;
  std::string firstc;
  int tmp=0;

  // Filling general info
  str >> firstc;                   // character 'E'
  str >> tmp;                      // event number
  str >> tmp;                      // number of multi particle interactions
  str >> myEvent.mc()->scale_;     // event scale
  str >> myEvent.mc()->alphaQCD_;  // alpha QCD
  str >> myEvent.mc()->alphaQED_;  // alpha QED
  str >> myEvent.mc()->processId_; // signal process id
  str >> tmp;                      // barcode for signal process vertex
  str >> tmp;                      // number of vertices in this event
  str >> tmp;                      // barcode for beam particle 1
  str >> tmp;                      // barcode for beam particle 2

  // Extracting random state list
  str >> tmp;
  if (tmp>0)
  {
    std::vector<Long64_t> randoms(static_cast<unsigned int>(tmp));
    for (unsigned int i=0;i<randoms.size();i++) str >> randoms[i];
  }

  // Extracting weight lists
  str >> tmp;
  if (tmp>0)
  {
    std::vector<double> weights(static_cast<unsigned int>(tmp));
    for (unsigned int i=0;i<weights.size();i++)
    {
      if (i==0) str >> myEvent.mc()->weight_;
      str >> weights[i];
    }
  }

}

// -----------------------------------------------------------------------------
// FillUnits
// -----------------------------------------------------------------------------
void HEPMCReader::FillUnits(const std::string& line)
{
  std::stringstream str;
  str << line;
  std::string tmp;
  
  // character 'N'
  str >> tmp; 

  // momentum units
  str >> tmp;
  if (tmp=="GEV") energy_unit_=1;
  else if (tmp=="MEV") energy_unit_=0.001;
  else if (tmp=="KEV") energy_unit_=0.000001;
  else 
  {
    std::cout << "ERROR: energy unit is unknown: " << tmp << std::endl;
  }

  // length units
  str >> tmp;
  if (tmp=="MM") length_unit_=1;
  else if (tmp=="CM") length_unit_=0.1;
  else 
  {
    std::cout << "ERROR: length unit is unknown: " << tmp << std::endl;
  }
  
}


// -----------------------------------------------------------------------------
// FillCrossSection
// -----------------------------------------------------------------------------
void HEPMCReader::FillCrossSection(const std::string& line, 
                                   SampleFormat& mySample)
{
  // Splitting the line in words
  std::stringstream str;
  str << line;
  
  // First character
  std::string firstc;
  str >> firstc;

  // xsection mean
  Double_t xsectmp=0;
  str >> xsectmp;

  // xsection error
  Double_t xsectmp_err=0;
  str >> xsectmp_err;

  // saving xsection mean & error
  if (mySample.mc()!=0)
  {
    mySample.mc()->setXsectionMean(xsectmp);
    mySample.mc()->setXsectionError(xsectmp_err);
  }
}

// -----------------------------------------------------------------------------
// FillEventPDFInfo
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventPDFInfo(const std::string& line, 
                                   SampleFormat& mySample,
                                   EventFormat& myEvent)
{
  std::stringstream str;
  str << line;
  std::string firstc;
  str >> firstc;
  str >> mySample.mc()->beamPDGID_.first;
  str >> mySample.mc()->beamPDGID_.second;
  str >> mySample.mc()->beamPDFID_.first;
  str >> mySample.mc()->beamPDFID_.second;
  str >> myEvent.mc()->x_.first;
  str >> myEvent.mc()->x_.second;
  str >> myEvent.mc()->PDFscale_;
  str >> myEvent.mc()->xpdf_.first;
  str >> myEvent.mc()->xpdf_.second;
}

// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventParticleLine(const std::string& line,
                                        EventFormat& myEvent)
{
  std::stringstream str;
  str << line;

  double tmp;    // temporary variable to fill in LorentzVector

  // Get a new particle
  MCParticleFormat * part = myEvent.mc()->GetNewParticle();
  char linecode;
  str >> linecode;
  UInt_t partnum;
  str >> partnum;
  str >> part->pdgid_;
  str >> tmp; part->momentum_.SetPx(tmp*energy_unit_);
  str >> tmp; part->momentum_.SetPy(tmp*energy_unit_);
  str >> tmp; part->momentum_.SetPz(tmp*energy_unit_);
  str >> tmp; part->momentum_.SetE (tmp*energy_unit_);
  str >> tmp; 
  str >> part->statuscode_;
  str >> tmp; 
  str >> tmp; 
  str >> part->extra2_;
  part->extra1_=current_vertex_.barcode_;
}

// -----------------------------------------------------------------------------
// FillEventVertexLine
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventVertexLine(const std::string& line, EventFormat& myEvent)
{
  std::stringstream str;
  str << line;

  int tmp=0;
  char linecode;
  str >> linecode;                  // character 'V'
  str >> current_vertex_.barcode_;  // barcode
  str >> tmp;                       // id
  str >> tmp;                       // x
  str >> tmp;                       // y
  str >> tmp;                       // z
  str >> current_vertex_.ctau_;     // ctau
}


//--------------------------------------------------------------------------
// SetMother
//--------------------------------------------------------------------------
void HEPMCReader::SetMother(MCParticleFormat* const part, EventFormat& myEvent)
{
  /*  std::cout << current_vertex_.barcode_ << std::endl;

  // No history
  if (myEvent.mc()->particles().size()==0) return;

  //ERIC orphan special treatment 
  if (part->extra_==current_vertex_.barcode_) return;

  std::cout << "---------------------------------------------" << std::endl;
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    std::cout << "i=" << i << "\t" << myEvent.mc()->particles()[i].pdgid() << " from ";
    if (myEvent.mc()->particles()[i].mother1()!=0) std::cout <<  myEvent.mc()->particles()[i].mother1()->pdgid();
    std::cout << " extra=" << myEvent.mc()->particles()[i].extra_ << " current=" << current_vertex_.barcode_ << std::endl;
    std::cout << std::endl;
  }


  unsigned int nmother=0;
  for (unsigned int i=0;i<(myEvent.mc()->particles().size()-1);i++)
  {
    if(myEvent.mc()->particles()[i].extra_==current_vertex_.barcode_)
    {
      nmother++;
      if      (nmother==1) part->mother1_=&(myEvent.mc()->particles()[i]);
      else if (nmother==2) part->mother2_=&(myEvent.mc()->particles()[i]);
      else 
      { 
        if (warnmother_) 
        {
          WARNING << "Number of mothers greather than 2 : " << nmother << endmsg; 
          warnmother_=false; 
        }
      }
    }
  }

    if (part->pdgid()==-24) {std::cout << "MMMMHHH" << std::endl; ComingFromHadronDecay(part); }
  */
}
