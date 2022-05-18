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

// FastJet headers
#include "fastjet/contrib/EnergyCorrelator.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/substructure/EnergyCorrelator.h"

namespace MA5 {
    namespace Substructure {

        EnergyCorrelator::~EnergyCorrelator()
        {
            delete _EC;
        }

        void EnergyCorrelator::Initialize(
                MAuint32 N,
                MAfloat32 beta,
                EnergyCorrelator::Measure measure,
                EnergyCorrelator::Strategy strategy
        )
        {
            fastjet::contrib::EnergyCorrelator::Measure measure_;
            fastjet::contrib::EnergyCorrelator::Strategy strategy_;

            if (measure == EnergyCorrelator::Measure::pt_R)
                measure_ = fastjet::contrib::EnergyCorrelator::Measure::pt_R;
            else if (measure == EnergyCorrelator::Measure::E_theta)
                measure_ = fastjet::contrib::EnergyCorrelator::Measure::E_theta;
            else
                measure_ = fastjet::contrib::EnergyCorrelator::Measure::E_inv;

            if (strategy == EnergyCorrelator::Strategy::storage_array)
                strategy_ = fastjet::contrib::EnergyCorrelator::Strategy::storage_array;
            else strategy_ = fastjet::contrib::EnergyCorrelator::Strategy::slow;

            _EC = new fastjet::contrib::EnergyCorrelator(N, beta, measure_, strategy_);
        }

        // Method to execute with a single jet
        MAdouble64 EnergyCorrelator::Execute(const RecJetFormat* jet) const
        { return (*_EC)(jet->pseudojet()); }
    }
}