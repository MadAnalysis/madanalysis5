////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2024 Jack Araz, Eric Conte & Benjamin Fuks
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
 * @file HEPData.h
 * @brief One-dimensional efficiency maps read from HEPData CSV files.
 */

#ifndef HEPDATA_H
#define HEPDATA_H

// STL Headers
#include <string>
#include <vector>

namespace MA5
{
  /** @brief One-dimensional binned efficiency read from a HEPData CSV file. */
  class Efficiency1D
  {
    private:
      /** @brief Has the map been read? */
      bool init_;
      /** @brief Lower bin edges (DBL_MAX is appended as last edge). */
      std::vector<double> bin_edges_;
      /** @brief Efficiency of each bin. */
      std::vector<double> efficiencies_;

    public:

      /** @brief Constructor (empty map). */
      Efficiency1D() { init_=false;}

      /**
       * @brief Constructor reading a CSV file.
       *
       * @param filename path of the CSV file.
       */
      Efficiency1D(std::string filename) { init_=false; ReadCSV(filename); }

      /** @brief Destructor. */
      ~Efficiency1D() { bin_edges_.clear(); efficiencies_.clear(); }

      /**
       * @brief Has the map been read?
       *
       * @return true if initialised.
       */
      bool Initialised() { return init_; }

      /**
       * @brief Read a HEPData CSV file (columns: x, x_low, x_high, efficiency, ...).
       *
       * Empty lines, comments (#) and lines starting with a letter are skipped.
       *
       * @param filename path of the CSV file.
       */
      void ReadCSV(std::string filename);
      /**
       * @brief Efficiency for a given value.
       *
       * @param x value.
       * @return the efficiency of the bin containing x (0 below the first edge or if not initialised).
       */
      double Get(const double x);
  };
}

#endif
