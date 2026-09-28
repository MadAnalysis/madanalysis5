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
 * @file ClusterAlgoFastJet.h
 * @brief Base class of the FastJet-based clustering algorithms.
 */

#ifndef JET_CLUSTERING_FASTJET_H
#define JET_CLUSTERING_FASTJET_H

// FastJet headers
#include <fastjet/ClusterSequence.hh>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Base/ClusterAlgoBase.h"
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"

// STL headers
#include <set>
#include <map>
#include <vector>
#include <string>


/** @brief Forward declaration of the FastJet jet definition. */
namespace fastjet
{
  class JetDefinition;
}


namespace MA5
{

/** @brief Base class of the clustering algorithms based on FastJet. */
class ClusterAlgoFastJet: public ClusterAlgoBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  protected :

    /** @brief FastJet jet definition (owned). */
    fastjet::JetDefinition* JetDefinition_;

    /** @brief Cluster sequence of the last clustering (kept alive for the substructure tools). */
    std::shared_ptr<fastjet::ClusterSequence> clust_seq;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /**
     * @brief Constructor.
     *
     * @param algo name of the algorithm.
     */
    ClusterAlgoFastJet(std::string algo);

    /** @brief Destructor (deletes the jet definition). */
    virtual ~ClusterAlgoFastJet(); 

    /**
     * @brief Cluster the primary jets of an event and compute MHT, THT, TET and Meff.
     *
     * The inputs are the hadrons stored in RecEventFormat::cluster_inputs(). Jets are
     * smeared if jet smearing is on; only the jets above Ptmin_ are stored, but all
     * jets contribute to the global observables.
     *
     * @param mySample current sample.
     * @param myEvent event (the primary jets are filled).
     * @param smearer SFS smearer.
     * @return true.
     */
    virtual MAbool Execute(SampleFormat& mySample, EventFormat& myEvent,
                           SmearerBase* smearer);

    /**
     * @brief Create the FastJet jet definition (implemented by the derived classes).
     *
     * @return false in case of error.
     */
    virtual MAbool Initialize()=0;

    /**
     * @brief Cluster an additional jet collection from the same inputs.
     *
     * @param myEvent event.
     * @param JetID identifier of the jet collection.
     * @return true.
     */
    virtual MAbool Cluster(EventFormat& myEvent, std::string JetID);
};

}

#endif
