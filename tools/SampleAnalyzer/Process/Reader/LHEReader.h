////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


#ifndef LHE_READER_h
#define LHE_READER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"


namespace MA5
{

class LHEReader : public ReaderTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:

  MAbool firstevent_;
  std::vector< std::pair<MAint32,MAint32> > mothers_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  //! Constructor without argument
  LHEReader()
  { firstevent_=false; }

  //! Destructor
  virtual ~LHEReader()
  { }

  /// Initialize
  virtual MAbool Initialize(const std::string& rawfilename,
                          const Configuration& cfg)
  { 
    firstevent_=false;
    return ReaderTextBase::Initialize(rawfilename,cfg);
  }

  /// Finalize
  virtual MAbool Finalize()
  { return ReaderTextBase::Finalize(); }

  //! Read the header
  virtual MAbool ReadHeader(SampleFormat& mySample);

  //! Finalize the header
  virtual MAbool FinalizeHeader(SampleFormat& mySample);

  //! Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);

  //! Finalize the event
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);


 private:

  //! Fill the header from text line 
  void FillHeaderProcessLine(const std::string& line, SampleFormat& mySample);
  void FillHeaderInitLine   (const std::string& line, SampleFormat& mySample);

  //! Fill the event from text line 
  void FillEventInitLine(const std::string& line, EventFormat& myFormat);
  void FillEventParticleLine(const std::string& line, EventFormat& myFormat);
  void FillWeightLine(const std::string& line, EventFormat& myEvent);

};

}

#endif
