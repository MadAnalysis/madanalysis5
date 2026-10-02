////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file MARotation3euler.h
 * @brief Rotation defined by Euler angles.
 */

#ifndef MARotation3euler_h
#define MARotation3euler_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MAMatrix.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

/** @brief Rotation defined by the Euler angles (phi, theta, psi). */
class MARotation3euler
{

 public :

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  /** @brief Rotation matrix. */
  MAMatrix m_; 


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /** @brief Constructor (empty matrix: setAngles() must be called before use). */
  MARotation3euler()
  {}

  /**
   * @brief Constructor.
   *
   * @param phi first Euler angle.
   * @param theta second Euler angle.
   * @param psi third Euler angle.
   */
  MARotation3euler(MAdouble64 phi, MAdouble64 theta, MAdouble64 psi)
  { setAngles(phi,theta,psi); }
  
  /** @brief Destructor. */
  ~MARotation3euler()
  {}

  /**
   * @brief Set the Euler angles (and compute the rotation matrix).
   *
   * @param phi first Euler angle.
   * @param theta second Euler angle.
   * @param psi third Euler angle.
   */
  void setAngles(MAdouble64 phi, MAdouble64 theta, MAdouble64 psi)
  {
    MAdouble64 cphi   = std::cos(phi);
    MAdouble64 sphi   = std::sin(phi);
    MAdouble64 ctheta = std::cos(theta);
    MAdouble64 stheta = std::sin(theta);
    MAdouble64 cpsi   = std::cos(psi);
    MAdouble64 spsi   = std::sin(psi);
    m_.setDim(3,3);
    m_[0][0] =  cpsi   * cphi    - spsi * sphi * ctheta;
    m_[0][1] =  cpsi   * sphi    + spsi * ctheta * cphi; 
    m_[0][2] =  spsi   * stheta;
    m_[1][0] = -spsi   * cphi    - cpsi * ctheta * sphi;
    m_[1][1] = -spsi   * sphi    + cpsi * ctheta * cphi;
    m_[1][2] =  cpsi   * stheta;
    m_[2][0] =  stheta * sphi;
    m_[2][1] = -stheta * cphi;
    m_[2][2] =  ctheta;
  }
  
  /**
   * @brief Rotate the spatial part of a four-vector in place.
   *
   * @param q four-vector.
   */
  void rotate(MALorentzVector& q) const
  { rotate(q.Vect()); }

  /**
   * @brief Rotate a three-vector in place.
   *
   * @param p three-vector.
   */
  void rotate(MAVector3& p) const
  {
    p = operator*(p);
  }

  /**
   * @brief Rotated copy of a four-vector.
   *
   * @param q four-vector.
   * @return the rotated four-vector.
   */
  MALorentzVector operator* (const MALorentzVector& q) const
  {
    return MALorentzVector(operator*(q.Vect()),q.E());
  }
  
  /**
   * @brief Rotated copy of a three-vector.
   *
   * @param p three-vector.
   * @return the rotated vector (see the FIXME).
   */
  MAVector3 operator* (const MAVector3& p) const
  {
    MAVector3 result;
    for (MAuint32 i=0;i<3;i++)
      for (MAuint32 j=0;j<3;j++)
      {
        // FIXME: '=' instead of '+=': only the last term of the matrix-vector product is kept.
        result[i] = m_[i][j]*p[j];
      }
    return result;
  }
  
};
 
}

#endif
