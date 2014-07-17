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
#include <iomanip>
#include <cstdlib>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Writer/LHEParticleFormat.h"

using namespace MA5;


// -----------------------------------------------------------------------------
// FortranFormat_SimplePrecision
// -----------------------------------------------------------------------------
std::string LHEParticleFormat::FortranFormat_SimplePrecision(Float_t value,UInt_t precision)
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
  if (std::abs(exponent)<10) str << "0";
  str << std::abs(exponent);
  str >> word;
  if (!negative) return word;
  else return "-"+word;
}


// -----------------------------------------------------------------------------
// FortranFormat_DoublePrecision
// -----------------------------------------------------------------------------
std::string LHEParticleFormat::FortranFormat_DoublePrecision(Double_t value,UInt_t precision)
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
  if (std::abs(exponent)<10) str << "0";
  str << std::abs(exponent);
  str >> word;
  if (!negative) return word;
  else return "-"+word;
}


// -----------------------------------------------------------------------------
// Print a particle line
// -----------------------------------------------------------------------------
void LHEParticleFormat::Print(UInt_t num, std::ostream* out)
{
  *out << std::setw(9)  << std::right << id       << " ";
  *out << std::setw(4)  << std::right << status   << " ";
  *out << std::setw(4)  << std::right << mother1  << " ";
  *out << std::setw(4)  << std::right << mother2  << " ";
  *out << std::setw(4)  << std::right << color1   << " ";
  *out << std::setw(4)  << std::right << color2   << " ";
  *out << std::setw(18) << std::right << FortranFormat_DoublePrecision(px) << " ";
  *out << std::setw(18) << std::right << FortranFormat_DoublePrecision(py) << " ";
  *out << std::setw(18) << std::right << FortranFormat_DoublePrecision(pz) << " ";
  *out << std::setw(18) << std::right << FortranFormat_DoublePrecision(e)  << " ";
  *out << std::setw(18) << std::right << FortranFormat_DoublePrecision(m)  << " ";
  *out << std::setw(2)  << std::right << std::showpoint << ctau << " ";
  *out << std::setw(3)  << std::right << std::showpoint << spin;
  *out << std::endl;
}


