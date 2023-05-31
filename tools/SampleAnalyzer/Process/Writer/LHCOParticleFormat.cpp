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


// STL headers
#include <sstream>
#include <iomanip>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/LHCOParticleFormat.h"


using namespace MA5;

// -----------------------------------------------------------------------------
// Header
// -----------------------------------------------------------------------------
const std::string LHCOParticleFormat::header = "  #  typ    eta     phi        pt    jmas    ntrk    btag  had/em   dum1   dum2";


// -----------------------------------------------------------------------------
// Print a particle line
// -----------------------------------------------------------------------------
void LHCOParticleFormat::Print(MAuint32 num, std::ostream* out)
{
  *out << std::setw(3) << std::right << num;
  *out << "  ";
  *out << std::setw(2) << std::right << id;
  *out << "  ";
  *out << std::fixed;
  MAuint32 pres = out->precision();
  out->precision(3);
  MAfloat64 myeta = eta;
  if (myeta>9) myeta=9.999;
  else if (myeta<-9) myeta=-9.999;
  *out << std::setw(6) << std::right << myeta;  // -X.YYY
  *out << "  ";
  *out << std::setw(6) << std::right << phi;    // -X.YYY
  *out << "  ";
  *out << std::setw(8) << std::right << pt;     // XXXX.YYY 
  *out << "  ";
  *out << std::setw(6) << std::right << jmass;  // -X.YYY
  *out << "  ";
  MAfloat64 myntrk = ntrk;
  if (myntrk>9) myntrk=9.999;
  else if (myntrk<-9) myntrk=-9.999;
  *out << std::setw(6) << std::right << myntrk;   // -X.YYY
  *out << "   ";
  *out << std::setw(5) << std::right << btag;   // X.YYY
  *out << "   ";
  out->precision(2);
  *out << std::setw(5) << std::right << hadem;  // XX.YY
  *out << "  ";
  *out << std::setw(5) << std::right << 0.;     // -X.YY
  *out << "  ";
  *out << std::setw(5) << std::right << 0.;     // -X.YY
  out->precision(pres);
  *out << std::endl;
}


// -----------------------------------------------------------------------------
// Print the event header
// -----------------------------------------------------------------------------
void LHCOParticleFormat::WriteEventHeader(MAuint32 numEvent,
                                          std::ostream* out)
{
  // Particle number
  *out << std::setw(3) << std::right << 0;
  *out << "  ";

  // Event number
  *out << std::setw(10) << std::right << numEvent+1;
  *out << "  ";

  // Trigger word
  *out << std::setw(6) << std::right << 0;
  *out << std::endl;
}
