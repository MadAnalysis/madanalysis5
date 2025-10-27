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

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/HEPData.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"

// STL Headers
#include <cfloat>
#include <fstream>

using namespace MA5;

// -----------------------------------------------------------------------------
// Reading a 1D CSV file (and storing the information)
// -----------------------------------------------------------------------------
void Efficiency1D::ReadCSV(std::string filename)
{
  // stream for the CSV file + safety
  std::ifstream csv_stream(filename);
  if(!csv_stream.is_open())
  {
    WARNING << "Could not open CSV file " << filename << endmsg;
    return;
  }

  // Reading the file
  std::string line;
  while (std::getline(csv_stream, line))
  {
    // Should skip the line
    if(line.empty() || line[0]=='#' || std::isalpha(static_cast<unsigned char>(line[0])))
      continue;

    // Useful line -> get the fields
    std::stringstream line_stream(line);
    std::string field;
    std::vector<double> fields;
    while (std::getline(line_stream, field, ','))
      fields.push_back(std::stod(field));

    // Storing the bin edge + the efficiency value
    if (fields.size() <= 6)
    {
      bin_edges_.push_back(fields[1]);
      efficiencies_.push_back(fields[3]);
    }
  }
  bin_edges_.push_back(DBL_MAX);

  // Exit -> everything was fine
  init_ = true;
}


// -----------------------------------------------------------------------------
// Getting the effienciency for a given x value
// -----------------------------------------------------------------------------
double Efficiency1D::Get(const double x)
{
  // Safety
  if(!init_) return 0.0;

  // Iterator to the first bin edge larger than x
  auto it = std::lower_bound(bin_edges_.begin(), bin_edges_.end(), x);

  // Three cases (underflow, overflow, normal)
  if (it == bin_edges_.begin()) return 0.0;
  if (it == bin_edges_.end()) { return efficiencies_[bin_edges_.size()-2]; }
  return efficiencies_[std::distance(bin_edges_.begin(), it) - 1];
}
