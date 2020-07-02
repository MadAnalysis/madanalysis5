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


#ifndef MABoost_h
#define MABoost_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

class MABoost
{

 public :

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  MAdouble64 bx_;
  MAdouble64 by_;
  MAdouble64 bz_;

  MAdouble64 b2_;
  MAdouble64 gamma_;
  MAdouble64 gamma2_;

  
  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  // Constructors
  MABoost()
  {
    bx_=0.; by_=0.;   bz_=0;
    b2_=0;  gamma_=0; gamma2_=0;
  }

  MABoost(MAdouble64 bx, MAdouble64 by, MAdouble64 bz)
  { setBoostVector(bx,by,bz); }
  
  // Destructor
  ~MABoost()
  {}

  // Setting the boost vector
  void setBoostVector(MAdouble64 bx, MAdouble64 by, MAdouble64 bz)
  {
    // boost component
    bx_ = bx;
    by_ = by;
    bz_ = bz;

    // intermediate results
    b2_     = bx_*bx_ + by_*by_ + bz_*bz_;
    gamma_  = 1.0 / std::sqrt(1.0 - b2_);
    gamma2_ = b2_ > 0 ? (gamma_ - 1.0)/b2_ : 0.0;
  }
  
  // Setting the boost vector
  void setBoostVector(const MALorentzVector& q)
  { 
    try
    {
      if (q.T()==0) throw EXCEPTION_WARNING("Energy equal to zero. Impossible to compute the boost.","",0);
      setBoostVector(q.X()/q.T(),q.Y()/q.T(),q.Z()/q.T());
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      MABoost();
    }    
  }

  // Boost a MALorentzVector
  void boost(MALorentzVector& p) const
  {
   MAdouble64 bp = bx_*p.X() + by_*p.Y() + bz_*p.Z();
   p.SetX(p.X() + gamma2_*bp*bx_ + gamma_*bx_*p.T());
   p.SetY(p.Y() + gamma2_*bp*by_ + gamma_*by_*p.T());
   p.SetZ(p.Z() + gamma2_*bp*bz_ + gamma_*bz_*p.T());
   p.SetT(gamma_*(p.T() + bp));
  }

  // Operator *
  MALorentzVector operator* (const MALorentzVector& q) const
  {
    MALorentzVector q2 = q;
    boost(q2);
    return q2;
  }
  
};
 
}

#endif
