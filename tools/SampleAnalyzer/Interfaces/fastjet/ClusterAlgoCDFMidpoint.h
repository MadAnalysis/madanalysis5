////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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
 * @file ClusterAlgoCDFMidpoint.h
 * @brief CDF midpoint cone algorithm (FastJet).
 */

#ifndef JETCLUSTERINGCDFMIDPOINT_H
#define JETCLUSTERINGCDFMIDPOINT_H


//SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoPlugin.h"

namespace MA5
{

/** @brief Jet clustering with the CDF midpoint cone algorithm. */
class ClusterAlgoCDFMidpoint: public ClusterAlgoPlugin
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

    /** @brief Cone radius. */
    MAfloat64 R_;

    /** @brief Overlap threshold. */
    MAfloat64 OverlapThreshold_;

    /** @brief Seed threshold. */
    MAfloat64 SeedThreshold_;

    /** @brief Cone area fraction. */
    MAfloat64 ConeAreaFraction_;

//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /** @brief Constructor (default parameters). */
    ClusterAlgoCDFMidpoint() {R_=0.5; OverlapThreshold_=0.75; SeedThreshold_=1.0; ConeAreaFraction_=1.0;}

    /** @brief Destructor. */
    virtual ~ClusterAlgoCDFMidpoint() {}

    /**
     * @brief Create the FastJet jet definition.
     *
     * @return true.
     */
    virtual MAbool Initialize();

    /**
     * @brief Set a parameter (`r`, `ptmin`, `overlapthreshold`, `seedthreshold` and `conearea`).
     *
     * @param key parameter name (lower case).
     * @param value value.
     * @return false for an unknown parameter.
     */
    virtual MAbool SetParameter(const std::string& key, const std::string& value);

    /** @brief Print the parameters. */
    virtual void PrintParam();

    /**
     * @brief Accessor to the name of the algorithm.
     *
     * @return the name.
     */
    virtual std::string GetName();

    /**
     * @brief Accessor to the parameters.
     *
     * @return a printable summary.
     */
    virtual std::string GetParameters();
};

}

#endif
