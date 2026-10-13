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
 * @file MAMatrix.h
 * @brief Minimal dense matrix of doubles.
 */

#ifndef MAMatrix_h
#define MAMatrix_h


// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{

/** @brief Minimal dense matrix of doubles (vector of rows). */
class MAMatrix
{

 public :

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 protected:
  
  /** @brief Rows of the matrix. */
  std::vector<std::vector<MAdouble64> > m_;


  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :

  /** @brief Constructor (empty matrix). */
  MAMatrix()
  {}

  /**
   * @brief Constructor of an n x m null matrix.
   *
   * @param n number of rows.
   * @param m number of columns.
   */
  MAMatrix(MAuint16 n, MAuint16 m)
  {setDim(n,m);}
  
  /**
   * @brief Constructor of an n x n null matrix.
   *
   * @param n dimension.
   */
  MAMatrix(MAuint16 n)
  {setDim(n,n);}

  /** @brief Destructor. */
  ~MAMatrix()
  {}

  /**
   * @brief Set the dimensions (new elements are null; existing rows keep their size).
   *
   * @param n number of rows.
   * @param m number of columns.
   */
  void setDim(MAuint16 n, MAuint16 m)
  {
    m_.resize(n,std::vector<MAdouble64>(m,0.));
  }

  /**
   * @brief Access a row (read-only).
   *
   * @param i row index.
   * @return the row.
   */
  const std::vector<MAdouble64>& operator[] (MAuint16 i) const
  { return m_[i]; }

  /**
   * @brief Access a row.
   *
   * @param i row index.
   * @return the row.
   */
  std::vector<MAdouble64>& operator[] (MAuint16 i)
  { return m_[i]; }

  
};
 
}

#endif
