////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

/// STL headers
#include <iostream>
#include <cassert>

#define EPSILON 1E-3

/// Fastjet headers
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

/// SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Interfaces/substructure/SoftDrop.h"
#include "SampleAnalyzer/Interfaces/substructure/Filter.h"
#include "SampleAnalyzer/Interfaces/substructure/EnergyCorrelator.h"
#include "SampleAnalyzer/Interfaces/substructure/Nsubjettiness.h"

using namespace MA5;

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

    /// Test Soft Drop
    std::cout << "    * TESTING SOFTDROP" << std::endl;
    Substructure::SoftDrop softDrop;
    softDrop.Initialize(2.0, 0.1);
    const RecJetFormat * softdrop_jet = softDrop.Execute(Ma5Jet[0]);
    double sn = softdrop_jet->m();
    assert(std::fabs(24.8832 - sn) < EPSILON);
    std::cout << "    * SOFTDROP PASSED" << std::endl;

    /// Test Jet Filtering
    std::cout << "    * TESTING JET FILTERING" << std::endl;
    Substructure::Filter jetFiltering;
    jetFiltering.Initialize(0.2, Substructure::SelectorPtFractionMin(0.03));
    const RecJetFormat * filteredJet = jetFiltering.Execute(Ma5Jet[0]);
    assert(std::fabs(389.646 - filteredJet->pt()) < EPSILON);
    std::cout << "    * JET FILTERING PASSED" << std::endl;

    /// Test Energy Correlator
    std::cout << "    * TESTING ENERGY CORRELATOR" << std::endl;
    Substructure::EnergyCorrelator EC;
    EC.Initialize(1, 0.1, Substructure::EnergyCorrelator::Measure::pt_R,
                  Substructure::EnergyCorrelator::Strategy::storage_array);
    double ec = EC.Execute(Ma5Jet[0]);
    assert(std::fabs(438.954 - ec) < EPSILON);
    std::cout << "    * ENERGY CORRELATOR PASSED " << std::endl;

    /// Test Nsubjettiness
    std::cout << "    * TESTING NSUBJETTINESS" << std::endl;
    Substructure::Nsubjettiness nsub;
    nsub.Initialize(1,Substructure::Nsubjettiness::KT_Axes,
                    Substructure::Nsubjettiness::NormalizedMeasure, 0.1, 0.2);
    double tau1 = nsub.Execute(Ma5Jet[0]);
    assert(std::fabs(0.916149 - tau1) < EPSILON);
    std::cout << "    * NSUBJETTINESS PASSED " << std::endl;

    std::cout << "END-SAMPLEANALYZER-TEST" << std::endl;
    return 0;
}
