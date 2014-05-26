////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


#ifndef GENERATOR_INFO_H
#define GENERATOR_INFO_H


namespace MA5
{

namespace MA5GEN
{
  enum GeneratorType { UNKNOWN=0,
                       MG5=1, MA5=2,
                       PYTHIA6=3, PYTHIA8=4, 
                       HERWIG6=5, HERWIGPP=6,
                       DELPHES=7, DELPHESMA5TUNE=8,
                       CALCHEP=9 };
}

namespace MA5FORMAT
{
  enum SampleFormatType { UNKNOWN=0, 
                          LHE=1, SIMPLIFIED_LHE=2, 
                          STDHEP=3, HEPMC=4, LHCO=5,
                          DELPHES=6, DELPHESMA5TUNE=7 };
}

}

#endif
