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
#include <cmath>

// SampleHeader headers
#include "SampleAnalyzer/Process/Reader/LHEReader.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


using namespace MA5;

// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
MAbool LHEReader::ReadHeader(SampleFormat& mySample)
{
  // Initiliaze MC
  mySample.InitializeMC();

  // Declaring a new string for line
  std::string line;

  // Generator tags
  MAbool tag_calchep = false;
  MAbool tag_mg5 = false;
  MAbool tag_ma5 = false;
  MAbool tag_simplified_pythia = false;
  MAbool tag_simplified_ma5    = false;

  // Read line by line the file until tag <header>
  // Note from Benj: the header tags are optional according to LHE standards
  //                 the init tags are alsways the last ones before the events
  MAbool EndOfLoop=false, GoodInit = false;

  while(!GoodInit)
  {
    MAbool HeaderFound = false, InitFound = false;
    do
    {
      if (!ReadLine(line)) return false;
      HeaderFound = (line.find("<header>")!=std::string::npos);
      InitFound = (line.find("<init>")!=std::string::npos);
      EndOfLoop = HeaderFound || InitFound;
    }
    while(!EndOfLoop);

    // Read line by line the file until tag </header>
    // Store the header
    if(HeaderFound)
    {
      EndOfLoop=false;
      do 
      { 
        if (!ReadLine(line,false)) return false;
        EndOfLoop = (line.find("</header>")!=std::string::npos);
        if (EndOfLoop) continue;
        else mySample.AddHeader(line);
        if ( (line.find("<MGGenerationInfo>")!=std::string::npos) ||
             (line.find("<mgversion>")!=std::string::npos)        ||
             (line.find("<MG5ProcCard>")!=std::string::npos)         )
          tag_mg5=true;
        if ( (line.find("<MA5Format> LHE format </MA5Format>")!=std::string::npos) )
          tag_ma5=true;
        if ( (line.find("<name>CalcHEP</name>")!=std::string::npos) ) tag_calchep=true;
        if ( (line.find("<MGPythiaCard>")!=std::string::npos) ||
             (line.find("<mgpythiacard>")!=std::string::npos) ) 
          tag_simplified_pythia=true;
        if ( (line.find("<MA5Format> Simplified LHE format </MA5Format>")!=std::string::npos) )
          tag_simplified_ma5=true;
      }
      while(!EndOfLoop);
    }

    if(InitFound)
    {
      // Read line by line the file until tag </init>
      EndOfLoop=false;
      MAbool first=true;
      do
      {
        if (!ReadLine(line)) return false;
        EndOfLoop = (line.find("</init>")!=std::string::npos);
        if (!EndOfLoop)
        {
          if (first) FillHeaderInitLine(line,mySample);
          else FillHeaderProcessLine(line,mySample);
        }
        first=false;
      }
      while(!EndOfLoop);
      GoodInit = true;
    }
  }

  // Read line by line the file until tag <event>
  EndOfLoop=false;
  do
  {
    if (!ReadLine(line)) return false;
    if ( (line.find("<MGGenerationInfo>")!=std::string::npos) ||
         (line.find("<mgversion>")!=std::string::npos)        ||
         (line.find("<MG5ProcCard>")!=std::string::npos)         )
      tag_mg5=true;
    if ( (line.find("<MA5Format> LHE format </MA5Format>")!=std::string::npos) )
      tag_ma5=true;
    if ( (line.find("<MGPythiaCard>")!=std::string::npos) ||
         (line.find("<mgpythiacard>")!=std::string::npos) ) 
      tag_simplified_pythia=true;
    if ( (line.find("<MA5Format> Simplified LHE format </MA5Format>")!=std::string::npos) )
      tag_simplified_ma5=true;
    EndOfLoop = (line.find("<event>")!=std::string::npos);
  }
  while(!EndOfLoop);


  // Determining sample format 
  if (tag_simplified_pythia || tag_simplified_ma5) 
  {
    mySample.SetSampleFormat(MA5FORMAT::SIMPLIFIED_LHE);
  }
  else
  {
    mySample.SetSampleFormat(MA5FORMAT::LHE);
  }


  // Determining generator format 
  if (tag_ma5 || tag_simplified_ma5) // must be treated before mg5 
  {
    mySample.SetSampleGenerator(MA5GEN::MA5);
  }
  else if (tag_simplified_pythia)
  {
    mySample.SetSampleGenerator(MA5GEN::PYTHIA6);
  }
  else if (tag_mg5)
  {
    mySample.SetSampleGenerator(MA5GEN::MG5);
  }
  else if (tag_calchep)
  {
    mySample.SetSampleGenerator(MA5GEN::CALCHEP);
  }
  else 
  {
    mySample.SetSampleGenerator(MA5GEN::UNKNOWN);
  }


  // Normal end
  firstevent_=true;
  return true;
}


// -----------------------------------------------------------------------------
// FinalizeHeader
// -----------------------------------------------------------------------------
MAbool LHEReader::FinalizeHeader(SampleFormat& mySample)
{
  // Computing xsection an its error for the sample
  MAfloat64 xsection = 0.;
  MAfloat64 xerror   = 0.;
  for (MAuint32 i=0;i<mySample.mc()->processes().size();i++)
  {
    xsection += mySample.mc()->processes()[i].xsectionMean();
    xerror   += mySample.mc()->processes()[i].xsectionError() *
      mySample.mc()->processes()[i].xsectionError();
  }

  // Filling xsection and its error
  mySample.mc()->setXsection(xsection);
  mySample.mc()->setXsectionError(std::sqrt(xerror));

  // Normal end 
  return true;
}


// -----------------------------------------------------------------------------
// ReadEvent
// -----------------------------------------------------------------------------
StatusCode::Type LHEReader::ReadEvent(EventFormat& myEvent, SampleFormat& mySample)
{
  // Initiliaze MC
  myEvent.InitializeMC();

  // Declaring a new string for line
  std::string line;
  MAbool EndOfEvent=false;
  MAbool event_block=false;
  MAbool event_header=false;
  MAbool multiweight_block=false;

  // Loop over the LHE lines
  while(!EndOfEvent)
  {
    // Read the line
    if (!firstevent_ && !ReadLine(line)) return StatusCode::FAILURE;
    // Detect tags
    if (line.find("<event>")!=std::string::npos || firstevent_)
    {
      event_block=true;
      event_header=true;
      firstevent_=false;
      continue;
    }
    else if (line.find("</event>")!=std::string::npos)
    {
      event_block=false;
      EndOfEvent=true;
      continue;
    }
    else if (line.find("<rwgt>")!=std::string::npos || line.find("mgrwt")!=std::string::npos)
    {
      multiweight_block=true;
      continue;
    }
    else if (line.find("</rwgt>")!=std::string::npos || line.find("/mgrwt")!=std::string::npos)
    {
      multiweight_block=false;
      continue;
    }

    // Actions
    if (event_block && !multiweight_block)
    {
      if (event_header)
      {
        FillEventInitLine(line,myEvent);
        event_header=false;
      }
      else FillEventParticleLine(line,myEvent);
    }
    else if (event_block && multiweight_block)
    {
      FillWeightLine(line,myEvent);
    }
  }

    /*
  // Read line by line the file until tag <event>
  if (!firstevent_)
  {
    EndOfLoop=false;
    do 
    { 
      if (!ReadLine(line)) return StatusCode::FAILURE;
      EndOfLoop = (line.find("<event>")!=std::string::npos);
    }
    while(!EndOfLoop);
  }

  // Read the particles
  EndOfLoop=false;
  firstevent_=false;
  MAbool first=true;
  do 
  { 
    if (!ReadLine(line)) return StatusCode::FAILURE;
    if (line.find("<rwgt>")!=std::string::npos) 
    {
      MAbool EndReweighting = false;
      do
      { 
        if (!ReadLine(line)) return StatusCode::FAILURE;
        EndReweighting = (line.find("</rwgt>")!=std::string::npos);
        FillWeightLine(line,myEvent);
      }
      while(!EndReweighting);
      if (!ReadLine(line)) return StatusCode::FAILURE;
    }
    EndOfLoop = (line.find("</event>")!=std::string::npos);
    if (!EndOfLoop)
    {
      if (first) FillEventInitLine(line,myEvent);
      else FillEventParticleLine(line,myEvent);
    }
    first=false;
  }
  while(!EndOfLoop);
    */

  // Normal end
  return StatusCode::KEEP;
}


// -----------------------------------------------------------------------------
// FinalizeEvent
// -----------------------------------------------------------------------------
MAbool LHEReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // Traditional LHE or simplified LHE ?
  MAbool simplified = (mySample.sampleFormat()==MA5FORMAT::SIMPLIFIED_LHE);

  // Mother-daughter relations
  for (MAuint32 i=0; i<mothers_.size();i++)
  {
    MCParticleFormat* part = &(myEvent.mc()->particles_[i]);
    MAint32& mothup1 = mothers_[i].first;
    MAint32& mothup2 = mothers_[i].second;

    if (mothup1>0)
    { 
      if (static_cast<MAuint32>(mothup1)<=myEvent.mc()->particles().size())
      {
        MCParticleFormat* mum = &(myEvent.mc()->particles()[static_cast<MAuint32>(mothup1-1)]);
        if (mum!=part)
        {
          part->mothers().push_back(mum);
          mum->daughters().push_back(part);
        }
      }
      else
      {
        std::stringstream str;
        str << "index=" << mothup1 << " but #particles=" << myEvent.mc()->particles().size();
        try
        {
          throw EXCEPTION_WARNING("internal problem with mother-daughter particles",str.str(),0);
        }
        catch(const std::exception& e)
        {
          MANAGE_EXCEPTION(e);
        }
      }
    }
    if (mothup2>0 && mothup1!=mothup2)
    {
      if (static_cast<MAuint32>(mothup2)<=myEvent.mc()->particles().size())
      {
        MCParticleFormat* mum = &(myEvent.mc()->particles()[static_cast<MAuint32>(mothup2-1)]);
        if (mum!=part)
        {
          part->mothers().push_back(mum);
          mum->daughters().push_back(part);
        }
      }
      else
      {
        std::stringstream str;
        str << "index=" << mothup2 << " but #particles=" << myEvent.mc()->particles().size();
        try
        {
          throw EXCEPTION_WARNING("internal problem with mother-daughter particles",str.str(),0);
        }
        catch(const std::exception& e)
        {
          MANAGE_EXCEPTION(e);
        }
      }
    }
  }
  mothers_.clear();

  // Global event observable
  for (MAuint32 i=0; i<myEvent.mc()->particles_.size();i++)
  {
    MCParticleFormat& part = myEvent.mc()->particles_[i];

    // MET in case of simplified LHE
    if ( ( (part.pdgid()==12 && part.statuscode()==1) || (part.statuscode()==1 && PHYSICS->Id->IsInvisible(part)) ) && simplified)
    {
      myEvent.mc()->MET_ += part.momentum();
    }

    // MET, MHT, TET, THT
    if (part.statuscode()==1 && !PHYSICS->Id->IsInvisible(part))
    {
      if (!simplified)
      {
        myEvent.mc()->MET_ -= part.momentum();
      }
      myEvent.mc()->TET_ += part.pt();
      if (PHYSICS->Id->IsHadronic(part))
      {
        myEvent.mc()->MHT_  -= part.momentum();
        myEvent.mc()->THT_  += part.pt(); 
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


// -----------------------------------------------------------------------------
// FillHeaderInitLine
// -----------------------------------------------------------------------------
void LHEReader::FillHeaderInitLine(const std::string& line, 
                                   SampleFormat& mySample)
{
  std::stringstream str;
  str << line;

  str >> mySample.mc()->beamPDGID_.first;
  str >> mySample.mc()->beamPDGID_.second;
  str >> mySample.mc()->beamE_.first;
  str >> mySample.mc()->beamE_.second;
  str >> mySample.mc()->beamPDFauthor_.first;
  str >> mySample.mc()->beamPDFauthor_.second;
  str >> mySample.mc()->beamPDFID_.first;
  str >> mySample.mc()->beamPDFID_.second;
  str >> mySample.mc()->weightMode_;
  // str >> mySample.mc()->nProcesses_; UNUSED
}


// -----------------------------------------------------------------------------
// FillHeaderProcessLine
// -----------------------------------------------------------------------------
void LHEReader::FillHeaderProcessLine(const std::string& line,
                                      SampleFormat& mySample)
{
  std::string tmpline=line;
  size_t posi = 0;
  while( (posi = tmpline.find("D", posi)) != std::string::npos) 
    tmpline=tmpline.replace(posi, 1, "E");
  posi=0;
  while( (posi = tmpline.find("d", posi)) != std::string::npos) 
    tmpline=tmpline.replace(posi, 1, "E");

  std::stringstream str;
  str << tmpline;

  // Get a new process
  ProcessFormat * proc = mySample.mc()->GetNewProcess();

  str >> proc->xsectionMean_;
  str >> proc->xsectionError_;
  str >> proc->weightMax_;
  str >> proc->processId_;
}


// -----------------------------------------------------------------------------
// FillEventInitLine
// -----------------------------------------------------------------------------
void LHEReader::FillEventInitLine(const std::string& line,
                                  EventFormat& myEvent)
{
  std::stringstream str;
  str << line;
  MAuint32 nparts;
  str >> nparts;
  str >> myEvent.mc()->processId_;
  str >> myEvent.mc()->weight_;
  str >> myEvent.mc()->scale_;
  str >> myEvent.mc()->alphaQED_;
  str >> myEvent.mc()->alphaQCD_;
  myEvent.mc()->particles_.reserve(nparts);
  mothers_.reserve(nparts);
}


// -----------------------------------------------------------------------------
// FillEventParticleLine
// -----------------------------------------------------------------------------
void LHEReader::FillEventParticleLine(const std::string& line,
                                      EventFormat& myEvent)
{
  std::string tmpline=line;
  size_t posi = 0;
  while( (posi = tmpline.find("D", posi)) != std::string::npos) 
    tmpline=tmpline.replace(posi, 1, "E");
  posi=0;
  while( (posi = tmpline.find("d", posi)) != std::string::npos) 
    tmpline=tmpline.replace(posi, 1, "E");

  std::stringstream str;
  str << tmpline;

  MAint32   color1;  // color 1 not stored 
  MAint32   color2;  // color 2 not stored
  MAfloat64 tmp;     // temporary
  MAfloat64 px;      // temporary variable to fill in LorentzVector
  MAfloat64 py;      // temporary variable to fill in LorentzVector
  MAfloat64 pz;      // temporary variable to fill in LorentzVector
  MAfloat64 e;       // temporary variable to fill in LorentzVector
  MAfloat64 ctau;    // temporary variable to fill in LorentzVector
  MAint32   mothup1; // mother1
  MAint32   mothup2; // mother2

  // Get a new particle
  MCParticleFormat * part = myEvent.mc()->GetNewParticle();

  str >> part->pdgid_;
  str >> part->statuscode_;
  str >> mothup1;
  str >> mothup2;
  str >> color1;
  str >> color2;
  str >> px;
  str >> py;
  str >> pz;
  str >> e; 
  str >> tmp;
  str >> ctau;
  str >> part->spin_;
  part->momentum_.SetPxPyPzE(px,py,pz,e);
  part->decay_vertex_.SetT(ctau);
  mothers_.push_back(std::make_pair(mothup1,mothup2));
}


// -----------------------------------------------------------------------------
// FillWeightLine
// -----------------------------------------------------------------------------
void LHEReader::FillWeightLine(const std::string& line,
                               EventFormat& myEvent)
{
  std::stringstream str;
  str << line;

  std::string tmp;
  str >> tmp;
  if (tmp!="<wgt") return;

  std::size_t found1 = line.find("\"");
  if (found1==std::string::npos) return;
  std::size_t found2 = line.find("\"",found1+1);
  if (found2==std::string::npos) return;
  std::string idstring = line.substr(found1+1,found2-found1-1);

  std::stringstream str2;
  str2<<idstring;
  MAuint32 id;
  str2>>id;
  
  found1 = line.find(">");
  if (found1==std::string::npos) return;
  found2 = line.find("<",found1+1);
  if (found2==std::string::npos) return;
  std::string valuestring = line.substr(found1+1,found2-found1-1);

  std::stringstream str3;
  str3<<valuestring;
  MAfloat64 value;
  str3>>value;

  myEvent.mc()->multiweights().Add(id,value);
}
