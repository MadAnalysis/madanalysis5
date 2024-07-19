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

#ifndef HEPDATA_H
#define HEPDATA_H

// STL Headers
#include <string>

namespace MA5
{
  class Efficiency1D
  {
    private:
      bool init_;
      std::vector<double> bin_edges_;
      std::vector<double> efficiencies_;

    public:

      // Constructor without argument
      Efficiency1D() { init_=false;}

      // Constructor with CSV filename
      Efficiency1D(std::string filename) { init_=false; ReadCSV(filename); }

      // Destructor
      ~Efficiency1D() { bin_edges_.clear(); efficiencies_.clear(); }

      // Accessors
      bool Initialised() { return init_; }

      // Methods
      void ReadCSV(std::string filename);
      double Get(const double x);
  };
}

#endif
