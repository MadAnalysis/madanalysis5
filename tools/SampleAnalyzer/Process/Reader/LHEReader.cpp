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
#include <cmath>

// SampleHeader headers
#include "SampleAnalyzer/Process/Reader/LHEReader.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

using namespace MA5;

// -----------------------------------------------------------------------------
// ReadHeader
// -----------------------------------------------------------------------------
bool LHEReader::ReadHeader(SampleFormat& mySample)
{
  // Initiliaze MC
  mySample.InitializeMC();

  // Declaring a new string for line
  std::string line;

  // Generator tags
  Bool_t tag_calchep = false;
  Bool_t tag_mg5 = false;
  Bool_t tag_ma5 = false;
  Bool_t tag_simplified_pythia = false;
  Bool_t tag_simplified_ma5    = false;

  // Read line by line the file until tag <header>
  // Note from Benj: the header tags are optional according to LHE standards
  //                 the init tags are alsways the last ones before the events
  bool EndOfLoop=false, GoodInit = false;

  while(!GoodInit)
  {
    bool HeaderFound = false, InitFound = false;
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
      bool first=true;
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
bool LHEReader::FinalizeHeader(SampleFormat& mySample)
{
  // Computing xsection an its error for the sample
  Double_t xsection = 0.;
  Double_t xerror   = 0.;
  for (unsigned int i=0;i<mySample.mc()->processes().size();i++)
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
  bool EndOfLoop=false;

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
  bool first=true;
  do 
  { 
    if (!ReadLine(line)) return StatusCode::FAILURE;
    if(line.find("<rwgt>")!=std::string::npos) 
    {
       bool EndReweighting = false;
       do
       { 
         if (!ReadLine(line)) return StatusCode::FAILURE;
         EndReweighting = (line.find("</rwgt>")!=std::string::npos);
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

  // Normal end
  return StatusCode::KEEP;
}


// -----------------------------------------------------------------------------
// FinalizeEvent
// -----------------------------------------------------------------------------
bool LHEReader::FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent)
{
  // Traditional LHE or simplified LHE ?
  Bool_t simplified = (mySample.sampleFormat()==MA5FORMAT::SIMPLIFIED_LHE);

  // Mother pointer assignment
  for (unsigned int i=0; i<myEvent.mc()->particles_.size();i++)
  {
    MCParticleFormat& part = myEvent.mc()->particles_[i];

    // MET in case of simplified LHE
    if ( ( part.pdgid()==12 || (part.statuscode()==1 && PHYSICS->Id->IsInvisible(part)) ) && simplified)
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
        myEvent.mc()->MHT_ -= part.momentum();
        myEvent.mc()->THT_ += part.pt(); 
      }
    }

    // assigning the correct address for the mother particles
    unsigned int index1=myEvent.mc()->particles_[i].mothup1_;
    unsigned int index2=myEvent.mc()->particles_[i].mothup2_;
    if (index1!=0) // at least one mother
    {
      if (index1>=myEvent.mc()->particles_.size() )
      {
        WARNING << "mother index is greater to nb of particles" << endmsg;
        WARNING << " - index1 = " << index1 << endmsg;
        WARNING << " - particles.size() " << myEvent.mc()->particles_.size() << endmsg;
        WARNING << "This event is skipped." << endmsg;
        return false;
      }
      myEvent.mc()->particles_[i].mother1_ = &myEvent.mc()->particles_[index1-1];
      myEvent.mc()->particles_[index1-1].daughters_.push_back(&myEvent.mc()->particles_[i]);

      if (index2!=0)
      {
        if (index2>=myEvent.mc()->particles_.size())
        {
          WARNING << "mother index is greater to nb of particles" << endmsg;
          WARNING << " - index2 = " << index2 << endmsg;
          WARNING << " - particles.size() " << myEvent.mc()->particles_.size() << endmsg;
          WARNING << "This event is skipped." << endmsg;
          return false;
        }
        myEvent.mc()->particles_[i].mother2_ = &myEvent.mc()->particles_[index2-1];
        myEvent.mc()->particles_[index2-1].daughters_.push_back(&myEvent.mc()->particles_[i]);
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

  str >> myEvent.mc()->nparts_;
  str >> myEvent.mc()->processId_;
  str >> myEvent.mc()->weight_;
  str >> myEvent.mc()->scale_;
  str >> myEvent.mc()->alphaQED_;
  str >> myEvent.mc()->alphaQCD_;
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

  signed int 	color1;	// color 1 not stored 
  signed int	color2;	// color 2 not stored
  double   		tmp;	  // temporary variable to fill in LorentzVector

  // Get a new particle
  MCParticleFormat * part = myEvent.mc()->GetNewParticle();

  str >> part->pdgid_;
  str >> part->statuscode_;
  str >> part->mothup1_;
  str >> part->mothup2_;
  str >> color1;
  str >> color2;
  str >> tmp; part->momentum_.SetPx(tmp); 
  str >> tmp; part->momentum_.SetPy(tmp);
  str >> tmp; part->momentum_.SetPz(tmp);
  str >> tmp; part->momentum_.SetE(tmp);
  str >> tmp;
  str >> part->ctau_;
  str >> part->spin_;
}

