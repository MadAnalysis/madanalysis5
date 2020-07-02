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


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
using namespace MA5;


MALorentzVector::MALorentzVector()
: p_(), e_(0.0)
{}

MALorentzVector::MALorentzVector(MAdouble64 x, MAdouble64 y, MAdouble64 z, MAdouble64 t)
: p_(x,y,z), e_(t)
{}

MALorentzVector::MALorentzVector(const MAVector3 & p, MAdouble64 e)
: p_(p), e_(e)
{}

MALorentzVector::MALorentzVector(const MALorentzVector & q)
: p_(q.Vect()), e_(q.E())
{}

MALorentzVector::~MALorentzVector()
{}

const MAdouble64& MALorentzVector::operator() (MAuint8 i) const
{
  if (i==0) return p_(0);
  else if (i==1) return p_(1);
  else if (i==2) return p_(2);
  else if (i==3) return e_;
  else
  {
      std::cout << "Error : operator()() bad index (%d) returning 0"
          << i << std::endl;
  }
  return e_;
}

MAdouble64& MALorentzVector::operator() (MAuint8 i)
{
  if (i==0) return p_(0);
  else if (i==1) return p_(1);
  else if (i==2) return p_(2);
  else if (i==3) return e_;
  else
  {
      std::cout << "Error : operator()() bad index (%d) returning 0"
                << i << std::endl;
  }
  return e_;
}


