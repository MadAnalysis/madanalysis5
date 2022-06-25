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

/// STL headers
#include <iostream>
#include <cassert>

#define EPSILON 1E-3

/// Fastjet headers
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

/// SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/HEPTopTagger/HTT.h"

using namespace MA5;

// -----------------------------------------------------------------------
// main program
// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
    std::cout << "BEGIN-SAMPLEANALYZER-TEST" << std::endl;

    /// Collect data
    std::ifstream fin("../Test/Substructure/input.dat",std::ifstream::in);
    std::vector<fastjet::PseudoJet> input_clusters;

    while(!fin.eof()){
        double x,y,z,e;
        fastjet::PseudoJet p;
        fin >> x >> y >> z >> e;
        if(!fin.eof()){
            p.reset(x/1000., y/1000., z/1000., e/1000.);
            input_clusters.push_back(p);
        }
    }
    std::cout << "    * ReadEvent: " << input_clusters.size() << " particles are read." << std::endl;

    //  jet definition
    fastjet::JetDefinition jet_def(fastjet::cambridge_algorithm,1.5);
    fastjet::ClusterSequence clust_seq(input_clusters, jet_def);
    std::vector<fastjet::PseudoJet> jets = sorted_by_pt(clust_seq.inclusive_jets(200.));

    std::vector<const RecJetFormat *> Ma5Jet;
    Ma5Jet.reserve(jets.size());
    for (auto &jet: jets)
    {
        RecJetFormat* current_jet = new RecJetFormat(jet);
        Ma5Jet.push_back(current_jet);
    }

    std::cout << "    * TESTING HEPTOPTAGGER" << std::endl;
    MA5::Substructure::HTT tagger;
    MA5::Substructure::HTT::InputParameters param;
    param.max_subjet = 30.;
    param.mass_drop = 0.8;
    param.filtering_R = 0.3;
    param.filt_N = 5;
    param.filtering_minpt = 30.;
    param.mode = Substructure::HTT::TWO_STEP_FILTER;
    tagger.Initialize(param);
    tagger.get_settings();

    tagger.Execute(Ma5Jet[1]);
    if (tagger.is_tagged()){
        std::cout << "    * Input fatjet: pT = " << Ma5Jet[1]->pt() << std::endl;
        std::cout << "    * Output: pT = " << tagger.top()->pt()
                  << " Mass = " << tagger.top()->m()  << std::endl;
        assert(std::fabs(tagger.top()->m() - 177.188) < EPSILON);
    }
    std::cout << "    * HEPTOPTAGGER PASSED " << std::endl;

    std::cout << "END-SAMPLEANALYZER-TEST" << std::endl;
    return 0;
}
