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


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Plot/PlotManager.h"

// STL headers
#include <map>
#include <sstream>

#ifdef YODA_USE
#include "YODA/Histo.h"
#include "YODA/WriterYODA.h"
#endif

using namespace MA5;

/// Write the counters in a Text file
void PlotManager::Write_TextFormat(SAFWriter& output)
{
  for (MAuint32 i=0;i<plots_.size();i++)
    plots_[i]->Write_TextFormat(output.GetStream());
}

/// Write the counters in a YODA file
void PlotManager::Write_YODA(std::string yodaname) const
{
  #ifdef YODA_USE
  std::vector<std::shared_ptr<YODA::Estimate1D>> yodaHistos;

  // collect histograms as YODA
  for (const PlotBase *plot : plots_){
    yodaHistos.push_back(plot->ToYODA());
  }

  // write histograms
  YODA::Writer& yodaWriter = YODA::WriterYODA::create();
  yodaWriter.write(yodaname, yodaHistos);
  #endif
}
