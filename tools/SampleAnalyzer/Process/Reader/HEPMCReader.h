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


#ifndef HEPMC_READER_h
#define HEPMC_READER_h


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Reader/ReaderTextBase.h"


namespace MA5
{

class HEPMCReader : public ReaderTextBase
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  MAbool firstevent_;
  MAbool endevent_;
  MAbool saved_;
  MAbool EndOfFile_;
  MAbool warnmother_;
  MAint32 partcode_;
  MAint32 vertcode_;
  MAfloat32 energy_unit_;
  MAfloat32 length_unit_;
  std::string savedline_;     // last saved line
  MAbool firstHeavyIons_;
  MAuint64 nparts_max_;
  MAuint64 nvertices_max_;

  struct HEPVertex
  {
    MAfloat64 ctau_;
    MAfloat64 id_;
    MAfloat64 x_;
    MAfloat64 y_;
    MAfloat64 z_;
    MAint32 barcode_;
    std::vector<MAuint32> in_;
    std::vector<MAuint32> out_;
    HEPVertex()
    { ctau_=0; id_=0; x_=0; y_=0; z_=0; barcode_=0; }
  };

  std::map<MAint32,HEPVertex> vertices_;
  MAint32 currentvertex_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

  /// Constructor without argument
  HEPMCReader()
  { 
    firstevent_=false; 
    firstHeavyIons_=true;
    nparts_max_=0;
    nvertices_max_=0;
  }

  /// Destructor
  virtual ~HEPMCReader()
  { }
  
  /// Read the header
  virtual MAbool ReadHeader(SampleFormat& mySample);
  
  /// Finalize the header
  virtual MAbool FinalizeHeader(SampleFormat& mySample);
  
  /// Read the event
  virtual StatusCode::Type ReadEvent(EventFormat& myEvent, SampleFormat& mySample);
  
  /// Finalize the event
  virtual MAbool FinalizeEvent(SampleFormat& mySample, EventFormat& myEvent);
  
 private:
  
  MAbool FillEvent(const std::string& line, EventFormat& myEvent, SampleFormat& mySample);
  void   FillEventInformations(const std::string& line, EventFormat& myEvent);
  void   FillCrossSection(const std::string& line, SampleFormat& mySample);
  void   FillUnits(const std::string& line);
  void   FillEventPDFInfo(const std::string& line, SampleFormat& mySample, EventFormat& myEvent);
  void   FillEventParticleLine(const std::string& line, EventFormat& myEvent);
  void   FillEventVertexLine(const std::string& line, EventFormat& myEvent);
  MAbool FillWeightNames(const std::string& line);
  MAbool FillHeavyIons(const std::string& line);

};

}

#endif
