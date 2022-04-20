////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

#include "SampleAnalyzer/Interfaces/HEPTopTagger/HTT.h"

using namespace MA5;

// -----------------------------------------------------------------------
// main program
// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
    std::cout << "BEGIN-SAMPLEANALYZER-TEST" << std::endl;
    MA5::Substructure::HTT tagger;
    MA5::Substructure::HTT::InputParameters param;
    param.do_optimalR = false;
    param.reclustering_algorithm = Substructure::kt;
    INFO << "initializing HTT " << endmsg;
    tagger.Initialize(param);
    INFO << "HTT initialized" << endmsg;
    tagger.get_settings();
    std::cout << "END-SAMPLEANALYZER-TEST" << std::endl;
    return 0;
}
