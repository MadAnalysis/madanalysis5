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
 * @file ClusterAlgoStandard.h
 * @brief Kt, anti-kt, Cambridge/Aachen and generalised-kt algorithms (pp or e+e- versions) (FastJet).
 */

#ifndef JETCLUSTERING_H
#define JETCLUSTERING_H


//SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoFastJet.h"

namespace MA5
{

/** @brief Jet clustering with the kt, anti-kt, Cambridge/Aachen and generalised-kt algorithms (pp or e+e- versions). */
class ClusterAlgoStandard: public ClusterAlgoFastJet
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

    /** @brief Jet radius. */
    MAfloat64 R_;

    /** @brief Exponent of the generalised-kt algorithm. */
    MAfloat64 p_;

    /** @brief Type of collisions (true: pp, false: e+e-). */
    MAbool collision_;
    
//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /**
     * @brief Constructor (R = 0.5, p = -1, pp collisions).
     *
     * @param Algo algorithm: kt, antikt, cambridge or genkt.
     */
    ClusterAlgoStandard(std::string Algo): ClusterAlgoFastJet(Algo) 
    {R_=0.5; p_=-1.; collision_=true;}

    /** @brief Destructor. */
    virtual ~ClusterAlgoStandard() 
    { }

    /**
     * @brief Create the FastJet jet definition.
     *
     * @return false for an unknown algorithm.
     */
    virtual MAbool Initialize();

    /**
     * @brief Set a parameter (`r`, `ptmin`, `p`, `collision` (pp/ee) and `exclusive`).
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

    /**
     * @brief Accessor to the type of collisions.
     *
     * @return "pp" or "ee".
     */
    std::string GetCollisionType() const;

};

}

#endif

