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


// STL headers
#include <sstream>

// SampleHeader headers
#include "SampleAnalyzer/Process/Reader/HEPMCReader.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


using namespace MA5;


// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
MAbool HEPMCReader::ReadHeader(SampleFormat& mySample)
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
MAbool HEPMCReader::FinalizeHeader(SampleFormat& mySample)
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

  // Allocating memory for all particles
  myEvent.mc()->particles_.reserve(nparts_max_);

  MAbool eventOnGoing=false;

  // Read the saved line
  if (savedline_!="") 
  {
    FillEvent(savedline_, myEvent, mySample);
    eventOnGoing=true;
    savedline_="";
  }

  MAbool endEvent=false;
  
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
MAbool HEPMCReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // Compute max numbers of particles & vertices
  if (myEvent.mc()->particles_.size()>nparts_max_) nparts_max_=myEvent.mc()->particles_.size();


  // Fill vertices information
  for (std::map<MAint32,HEPVertex>::iterator it=vertices_.begin();
       it!=vertices_.end(); it++)
  {
    // Decay position & lifetime
    for (MAuint32 i=0;i<it->second.out_.size();i++)
    {
        MCParticleFormat* part = &(myEvent.mc()->particles_[it->second.out_[i]]);
        part->decay_vertex_.SetXYZT(it->second.x_,it->second.y_,it->second.z_,it->second.ctau_);
    }

    // Mother+daughter relations
    for (MAuint32 i=0;i<it->second.in_.size();i++)
    {
      for (MAuint32 j=0;j<it->second.out_.size();j++)
      {
        MCParticleFormat* mum = &(myEvent.mc()->particles_[it->second.in_[i]]);
        MCParticleFormat* dau = &(myEvent.mc()->particles_[it->second.out_[j]]);

        // Deal with HERWIG initial particle : initial part = part whose mother is itself 
        if (mum!=dau)
        {
          // Safety: be sure to have not 2 same daughters
          MAbool found=false;
          for (MAuint32 h=0;h<mum->daughters().size();h++)
          {
            if (mum->daughters()[h]==dau) {found=true; break;}
          }
          if (!found) mum -> daughters().push_back(dau);

          // Safety: be sure to have not 2 same mothers
          found=false;
          for (MAuint32 h=0;h<dau->mothers().size();h++)
          {
            if (dau->mothers()[h]==mum) {found=true; break;}
          }
          if (!found) dau -> mothers().push_back(mum);
        }
      }
    }
  }
  vertices_.clear();


  // Computing met, mht, ... 
  for (MAuint32 i=0; i<myEvent.mc()->particles_.size();i++)
  {
    MCParticleFormat& part = myEvent.mc()->particles_[i];

    // MET, MHT, TET, THT
    if (part.statuscode()==1 && !PHYSICS->Id->IsInvisible(part))
    {
      myEvent.mc()->MET_ -= part.momentum();
      myEvent.mc()->TET_ += part.pt();
      if (PHYSICS->Id->IsHadronic(part))
      {
        myEvent.mc()->MHT_ -= part.momentum();
        myEvent.mc()->THT_ += part.pt(); 
        myEvent.mc()->Meff_ += part.pt(); 
      }
    }
  }

  // Finalize event
  myEvent.mc()->MET_.momentum().SetPz(0.);
  myEvent.mc()->MET_.momentum().SetE(myEvent.mc()->MET_.momentum().Pt());
  myEvent.mc()->MHT_.momentum().SetPz(0.);
  myEvent.mc()->MHT_.momentum().SetE(myEvent.mc()->MHT_.momentum().Pt());
  myEvent.mc()->Meff_ += myEvent.mc()->MET_.pt();

  // Normal end 
  return true; 
}


//------------------------------------------------------------------------------
// FillWeightNames
//------------------------------------------------------------------------------
MAbool HEPMCReader::FillWeightNames(const std::string& line)
{
  // Splitting line in words
  std::stringstream str;
  str << line ;

  // Getting the first word
  std::string firstWord;
  str >> firstWord;

  // Extracting the number of weights
  MAuint32 nweights;
  str >> nweights;

  // Storing weight names
  std::vector<std::string> weight_names(nweights);

  // Filling weight names
  for (MAuint32 i=0;i<weight_names.size();i++)
  {
    std::string tmp;
    str >> tmp;
    if (tmp=="") continue;
    
    if (tmp[0]=='"' && tmp[tmp.size()-1]=='"') tmp=tmp.substr(1,tmp.size()-2);
    weight_names[i]=tmp;
  }

  // Ok
  return true;
}


//------------------------------------------------------------------------------
// FillHeavyIons
//------------------------------------------------------------------------------
MAbool HEPMCReader::FillHeavyIons(const std::string& line)
{
  try
  {
    if (line!="") if (firstHeavyIons_) throw EXCEPTION_WARNING("HeavyIons block is not read by SampleAnalyzer","",0);
  }
  catch(const std::exception& e)
  {
    MANAGE_EXCEPTION(e);
  }    
  firstHeavyIons_=false;
  return false;
}


//------------------------------------------------------------------------------
// FillEventHeader
//------------------------------------------------------------------------------
MAbool HEPMCReader::FillEvent(const std::string& line,
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
    try
    {
      throw EXCEPTION_WARNING("HEPMC linecode unknown","",0);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
    }    
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
  MAint32 tmp=0;

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
    std::vector<MAint64> randoms(static_cast<MAuint32>(tmp));
    for (MAuint32 i=0;i<randoms.size();i++) str >> randoms[i];
  }

  // Extracting weight lists
  str >> tmp;
  if (tmp>0)
  {
    MAuint32 nweights=static_cast<MAuint32>(tmp);
    for (MAuint32 i=0;i<nweights;i++)
    {
      MAfloat64 value;
      str >> value;
      if (i==0) myEvent.mc()->weight_=value;
      myEvent.mc()->multiweights().Add(i+1,value);
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
  MAfloat64 xsectmp=0;
  str >> xsectmp;

  // xsection error
  MAfloat64 xsectmp_err=0;
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

  MAfloat64 tmp;    // temporary variable to fill in LorentzVector

  // Get a new particle
  MCParticleFormat * part = myEvent.mc()->GetNewParticle();
  MAchar linecode;
  MAfloat64 px=0.;
  MAfloat64 py=0.;
  MAfloat64 pz=0.;
  MAfloat64 e=0.;
  MAuint32  partnum;
  MAint32   decay_barcode;

  str >> linecode;          // letter 'P'
  str >> partnum;           // particle number
  str >> part->pdgid_;      // pdgid
  str >> px;                // Lorentz-vector px
  str >> py;                // Lorentz-vector py
  str >> pz;                // Lorentz-vector pz
  str >> e;                 // Lorentz-vector e
  str >> tmp;               // Lorentz-vector mass
  str >> part->statuscode_; // statuscode
  str >> tmp;               // color flow
  str >> tmp;               // pointer to the production vertex
  str >> decay_barcode;     // pointer to the decay vertex
  // not loaded
  //  MAuint32 barcode;         // barcode = an integer which uniquely 
  //  str >> barcode;           //           identifies the GenParticle within the event.
  

  part->momentum_.SetPxPyPzE (px * energy_unit_,
                              py * energy_unit_,
                              pz * energy_unit_,
                              e  * energy_unit_);

  MAuint32 part_index = myEvent.mc()->particles_.size()-1;

  // Set production vertex
  std::pair<std::map<MAint32,HEPVertex>::iterator,MAbool> ret;
  ret = vertices_.insert(std::make_pair(currentvertex_,HEPVertex()));
  ret.first->second.out_.push_back(part_index);

  // Set decay vertex
  ret = vertices_.insert(std::make_pair(decay_barcode,HEPVertex()));
  ret.first->second.in_.push_back(part_index);

  // Ok
  return;
}

// -----------------------------------------------------------------------------
// FillEventVertexLine
// -----------------------------------------------------------------------------
void HEPMCReader::FillEventVertexLine(const std::string& line, EventFormat& myEvent)
{
  std::stringstream str;
  str << line;

  MAchar linecode;
  MAint32 barcode;
  HEPVertex vertex;

  str >> linecode;      // character 'V'
  str >> barcode;       // barcode
  str >> vertex.id_;    // id
  str >> vertex.x_;     // x
  str >> vertex.y_;     // y
  str >> vertex.z_;     // z
  str >> vertex.ctau_;  // ctau

    // Adding this vertex to the vertex collection
  std::pair<std::map<MAint32,HEPVertex>::iterator,MAbool> res = vertices_.insert(std::make_pair(barcode,vertex));
  if (!res.second)
  {
    res.first->second.id_   = vertex.id_;
    res.first->second.x_    = vertex.x_;
    res.first->second.y_    = vertex.y_;
    res.first->second.z_    = vertex.z_;
    res.first->second.ctau_ = vertex.ctau_;
  }

  // Set the current vertex barcode
  currentvertex_ = barcode;
}

