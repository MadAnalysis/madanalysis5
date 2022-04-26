////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef JET_CLUSTERER_H
#define JET_CLUSTERER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Base/ClusterAlgoBase.h"
#include "SampleAnalyzer/Process/JetClustering/bTagger.h"
#include "SampleAnalyzer/Process/JetClustering/cTagger.h"
#include "SampleAnalyzer/Process/JetClustering/TauTagger.h"
#include "SampleAnalyzer/Process/JetClustering/NullSmearer.h"
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"

#ifdef MA5_FASTJET_MODE
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoStandard.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoSISCone.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFMidpoint.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFJetClu.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoGridJet.h"
  #include "SampleAnalyzer/Interfaces/fastjet/VariableR.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterBase.h"
#endif

// STL headers
#include <map>
#include <algorithm>
#include <locale>


namespace MA5
{

  class JetClusterer
  {
    //--------------------------------------------------------------------------
    //                              data members
    //--------------------------------------------------------------------------
  protected :

    ClusterAlgoBase* algo_;
    bTagger*     myBtagger_;
    cTagger*     myCtagger_;
    TauTagger*   myTautagger_;
    SmearerBase* mySmearer_;

    /// Exclusive id for tau-elec-photon-jet
    MAbool ExclusiveId_;

    /// Primary Jet ID
    std::string JetID_;

#ifdef MA5_FASTJET_MODE
    /// Jet collection configurations
    std::map<std::string, ClusterAlgoBase*> cluster_collection_;

    // Jet collection configuration with VariableR
    std::map<std::string, Substructure::ClusterBase*> substructure_collection_;
#endif

    MAuint32 muon;
    MAuint32 electron;
    MAuint32 tauH;
    MAuint32 tauM;
    MAuint32 tauE;
    MAuint32 photon;

    // Track Isolation radius
    std::vector<MAfloat64> isocone_track_radius_;

    // Electron Isolation radius
    std::vector<MAfloat64> isocone_electron_radius_;

    // Muon Isolation radius
    std::vector<MAfloat64> isocone_muon_radius_;

    // Photon Isolation radius
    std::vector<MAfloat64> isocone_photon_radius_;

    //--------------------------------------------------------------------------
    //                              method members
    //--------------------------------------------------------------------------
  public :

    /// Constructor without argument
    JetClusterer (ClusterAlgoBase* algo) 
    {
      // Initializing tagger
      algo_        = algo;
#ifdef MA5_FASTJET_MODE
      cluster_collection_.clear();
#endif
      myBtagger_   = 0;
      myCtagger_   = 0;
      myTautagger_ = 0;
      mySmearer_   = 0;
      ExclusiveId_ = false;
      JetID_       = "Ma5Jet";
      muon=0;
      electron=0;
      tauH=0;
      tauM=0;
      tauE=0;
      photon=0;
      isocone_track_radius_.clear();
      isocone_electron_radius_.clear();
      isocone_muon_radius_.clear();
      isocone_photon_radius_.clear();
    }

    /// Destructor
    ~JetClusterer()
    { }

    /// Initialization
    MAbool Initialize(const std::map<std::string,std::string>& options);

    /// Jet clustering
    MAbool Execute(SampleFormat& mySample, EventFormat& myEvent);

    /// Finalization
    void Finalize();

    /// Generic loader for the smearer module
    void LoadSmearer(SmearerBase* smearer)
    {
        mySmearer_ = smearer;
        mySmearer_->Initialize();
    }

    // Load additional Jets
    MAbool LoadJetConfiguration(std::map<std::string,std::string> options)
    {
#ifdef MA5_FASTJET_MODE

        std::string new_jetid;
        std::string algorithm;
        if (options.find("algorithm") == options.end())
        {
            ERROR << "Jet configuration needs to have an `algorithm` option. Jet configuration ignored." << endmsg;
            return true;
        }
        else algorithm = options["algorithm"];
        if (options.find("JetID") == options.end())
        {
            ERROR << "Jet configuration needs to have a `JetID` option. Jet configuration ignored." << endmsg;
            return true;
        }
        if (substructure_collection_.find(options["JetID"]) != substructure_collection_.end() || \
                cluster_collection_.find(options["JetID"]) != cluster_collection_.end() )
        {
            ERROR << "Jet ID " + options["JetID"] + " already defined. Jet configuration ignored." << endmsg;
            return true;
        }

        if (algorithm != "VariableR")
        {
            std::map<std::string, std::string> clustering_params;

            // decide if its good to keep this jet
            ClusterAlgoBase* new_algo;
            // Loop over options
            for (const auto &it: options)
            {
                std::string key = ClusterAlgoBase::Lower(it.first);
                if (key=="jetid")
                {
                    // Check if JetID is used before
                    new_jetid = it.second;
                    continue;
                }

                // Find the clustering algorithm
                if (key=="algorithm")
                {
                    if (it.second == "antikt")           new_algo = new ClusterAlgoStandard("antikt");
                    else if (it.second == "cambridge")   new_algo = new ClusterAlgoStandard("cambridge");
                    else if (it.second == "genkt")       new_algo = new ClusterAlgoStandard("genkt");
                    else if (it.second == "kt")          new_algo = new ClusterAlgoStandard("kt");
                    else if (it.second == "siscone")     new_algo = new ClusterAlgoSISCone();
                    else if (it.second == "cdfmidpoint") new_algo = new ClusterAlgoCDFMidpoint();
                    else if (it.second == "cdfjetclu")   new_algo = new ClusterAlgoCDFJetClu();
                    else if (it.second == "gridjet")     new_algo = new ClusterAlgoGridJet();
                    else {
                        ERROR << "Unknown algorithm " << it.second << " ignored." << endmsg;
                        return true;
                    }
                    continue;
                }
                // clustering algo -> keep the previous syntax
                else if (key.find("cluster.")==0)
                {
                    clustering_params.insert(std::pair<std::string,std::string>(key.substr(8),it.second));
                    continue;
                }

                // Other
                try
                {
                  throw EXCEPTION_WARNING("Parameter = "+key+" unknown and thus skipped.","",0);
                }
                catch(const std::exception& e)
                {
                  MANAGE_EXCEPTION(e);
                  return false;
                }
            }

            cluster_collection_.insert(std::pair<std::string,ClusterAlgoBase*>(new_jetid,new_algo));
            for (const auto &it: clustering_params)
                cluster_collection_[new_jetid]->SetParameter(it.first, it.second);
            std::string algoname = cluster_collection_[new_jetid]->GetName();
            std::string params   = cluster_collection_[new_jetid]->GetParameters();
            INFO << "      - Adding Jet ID : " << new_jetid << endmsg;
            INFO << "            with algo : " << algoname << ", " << params << endmsg;
            cluster_collection_[new_jetid]->Initialize();
        }
        else if (algorithm == "VariableR")
        {
            for (std::string key: {"rho", "minR", "maxR", "PTmin", "clustertype", "strategy", "exclusive"})
            {
                if (options.find("cluster."+key) == options.end())
                {
                    ERROR << "Option 'cluster." + key + "' is missing. VariableR clustering ignored." << endmsg;
                    return true;
                }
            }
            MAfloat32 rho   = std::stof(options["cluster.rho"]);
            MAfloat32 minR  = std::stof(options["cluster.minR"]);
            MAfloat32 maxR  = std::stof(options["cluster.maxR"]);
            MAfloat32 ptmin = std::stof(options["cluster.PTmin"]);
            MAbool isExclusive = (options["cluster.exclusive"] == "1");

            Substructure::VariableR::ClusterType ctype = Substructure::VariableR::AKTLIKE;
            if (options["cluster.clustertype"] == "CALIKE")       ctype = Substructure::VariableR::CALIKE;
            else if (options["cluster.clustertype"] == "KTLIKE")  ctype = Substructure::VariableR::KTLIKE;
            else if (options["cluster.clustertype"] == "AKTLIKE") ctype = Substructure::VariableR::AKTLIKE;

            Substructure::VariableR::Strategy strategy = Substructure::VariableR::Best;
            if (options["cluster.strategy"] == "Best")         strategy = Substructure::VariableR::Best;
            else if (options["cluster.strategy"] == "N2Tiled") strategy = Substructure::VariableR::N2Tiled;
            else if (options["cluster.strategy"] == "N2Plain") strategy = Substructure::VariableR::N2Plain;
            else if (options["cluster.strategy"] == "NNH")     strategy = Substructure::VariableR::NNH;
            else if (options["cluster.strategy"] == "Native")  strategy = Substructure::VariableR::Native;

            Substructure::VariableR* variableR;
            variableR = new Substructure::VariableR(rho, minR, maxR, ctype, strategy, ptmin, isExclusive);

            substructure_collection_.insert(
                std::pair<std::string, Substructure::VariableR*>(options["JetID"], variableR)
            );

            std::string exclusive = isExclusive ? "True" : "False";
            INFO << "      - Adding Jet ID : " << options["JetID"] << endmsg;
            INFO << "            with algo : VariableR" << ", "
                 << "rho = " << options["cluster.rho"] << ", "
                 << "minR = " << options["cluster.minR"] << ", "
                 << "maxR = " << options["cluster.maxR"] << ", "
                 << "ptmin = " << options["cluster.PTmin"] << ", \n"
                 << "                                   "
                 << "isExclusive = " << exclusive << ", "
                 << "clustertype = " << options["cluster.clustertype"] << ", "
                 << "strategy = " << options["cluster.strategy"]
                 << endmsg;
        }
        else
        {
            ERROR << "Unknown algorithm: " << algorithm << endmsg;
            return false;
        }

        return true;
#else
        ERROR << "FastJet has not been enabled. Can not add jets to the analysis." << endmsg;
        return true;
#endif
    }

    /// Accessor to the jet clusterer name
    std::string GetName() 
    { 
      if (algo_==0) return "NotDefined";
      else return algo_->GetName();
    }

    /// Accessor to the b tagger parameters
    std::string bParameters()
    { return myBtagger_->GetParameters(); }

    /// Accessor to the tau tagger parameters
    std::string tauParameters()
    { return myTautagger_->GetParameters(); }

    /// Print parameters
    void PrintParam()
    { algo_->PrintParam(); }

    /// Accessor to the jet clusterer parameters
    std::string GetParameters()
    { return algo_->GetParameters(); }

 private:
    MAbool IsLast(const MCParticleFormat* part, EventFormat& myEvent);
    void GetFinalState(const MCParticleFormat* part, std::set<const MCParticleFormat*>& finalstates);

  };

}

#endif
