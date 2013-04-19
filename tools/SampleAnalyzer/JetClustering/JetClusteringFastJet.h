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


#ifndef JET_CLUSTERING_FASTJET_H
#define JET_CLUSTERING_FASTJET_H

#ifdef FASTJET_USE

//SampleAnalyser headers
#include "SampleAnalyzer/DataFormat/EventFormat.h"
#include "SampleAnalyzer/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Service/Physics.h"
#include "SampleAnalyzer/Service/PDGService.h"
#include "SampleAnalyzer/JetClustering/JetClustererBase.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>


namespace MA5
{

class JetClusteringFastJet: public JetClustererBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  protected :

    /// Jet clustering algorithm
    fastjet::JetAlgorithm JetAlgorithm_;

    /// Jet definition
    fastjet::JetDefinition JetDefinition_;

//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor with algorithm
    JetClusteringFastJet(const fastjet::JetAlgorithm& Algo)
    { JetAlgorithm_=Algo; }

    /// Destructor
    virtual ~JetClusteringFastJet() 
    { }

    /// Jet clustering
    bool Execute(SampleFormat& mySample, EventFormat& myEvent);

    /// Initialization
    virtual bool Initialize(const std::map<std::string,std::string>& options)=0;

};

}

#endif
#endif
