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
 * @file gz_file.h
 * @brief Wrapper of a zlib file handle (hides zlib.h from the other headers).
 */

#ifndef GZ_FILE_H
#define GZ_FILE_H


// ZLib headers
#include "zlib.h"


namespace MA5
{

// -------------------------------------------------------------
//                      CLASS GZ_FILE
// -------------------------------------------------------------
/** @brief Wrapper of a zlib file handle. */
class gz_file
{

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:

  /** @brief zlib handle of the compressed file. */
  gzFile file; // file handle for compressed file

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
  
 public:

  /**
   * @brief Accessor to the zlib handle.
   *
   * @return the handle.
   */
  gzFile& get()
  { return file; }

};

}

#endif

