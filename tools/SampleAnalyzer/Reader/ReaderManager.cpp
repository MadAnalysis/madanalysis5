////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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
#include "SampleAnalyzer/Reader/ReaderManager.h"
#include "SampleAnalyzer/Service/LogService.h"
#include "SampleAnalyzer/Reader/LHEReader.h"
#include "SampleAnalyzer/Reader/LHCOReader.h"
#include "SampleAnalyzer/Reader/STDHEPreader.h"
#include "SampleAnalyzer/Reader/HEPMCReader.h"
#ifdef FAC_USE
  #include "SampleAnalyzer/Reader/ROOTReader.h"
#endif

using namespace MA5;

// -----------------------------------------------------------------------------
// BuildTable
// -----------------------------------------------------------------------------
void ReaderManager::BuildTable()
{
  // Adding LHE reader
  LHEReader* lhe = new LHEReader();
  Add("lhe",lhe);
#ifdef ZIP_USE
  Add("lhe.gz",lhe);
#endif

  LHCOReader* lhco = new LHCOReader();
  Add("lhco",lhco);
#ifdef ZIP_USE
  Add("lhco.gz",lhco);
#endif

  STDHEPreader* stdhep = new STDHEPreader();
  Add("hep",stdhep);
#ifdef ZIP_USE
  Add("hep.gz",stdhep);
#endif

  HEPMCReader* hepmc = new HEPMCReader();
  Add("hepmc",hepmc);
#ifdef ZIP_USE
  Add("hepmc.gz",hepmc);
#endif

#ifdef FAC_USE
  ROOTReader* root = new ROOTReader();
  Add("root",root);
#ifdef ZIP_USE
  Add("root.gz",root);
#endif
#endif
}


// -----------------------------------------------------------------------------
// GetByFileExtension
// -----------------------------------------------------------------------------
ReaderBase* ReaderManager::GetByFileExtension(std::string filename)
{
  // Set the extension in lower case
  std::transform(filename.begin(), filename.end(),
                 filename.begin(), std::ptr_fun<int, int>(std::tolower));

 // Loop over names
  for (std::map<std::string, UInt_t>::const_iterator
         it = Names_.begin(); it != Names_.end(); it++)
  {
    // easy case to reject
    if (filename.size()<it->first.size()) continue;

    // find pattern in filename
    if (it->first.compare(0,
                          it->first.size(),
                          filename,
                          filename.size()-it->first.size(),
                          it->first.size())==0)
    {
      return Objects_[it->second];
    }
  }

  // No reader found : return null pointer
  return 0;
}
