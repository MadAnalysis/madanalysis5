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
#include "SampleAnalyzer/Commons/Base/SubstructureBase.h"

#ifdef FASTJET_USE
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoStandard.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoSISCone.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFMidpoint.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFJetClu.h"
  #include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoGridJet.h"
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

#ifdef FASTJET_USE
    /// Jet collection configurations
    std::map<std::string, ClusterAlgoBase*> cluster_collection_;
#endif

    MAuint32 muon;
    MAuint32 electron;
    MAuint32 tauH;
    MAuint32 tauM;
    MAuint32 tauE;
    MAuint32 photon;

    //--------------------------------------------------------------------------
    //                              method members
    //--------------------------------------------------------------------------
  public :

    /// Constructor without argument
    JetClusterer (ClusterAlgoBase* algo) 
    {
      // Initializing tagger
      algo_        = algo;
#ifdef FASTJET_USE
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
    void LoadJetConfiguration(const std::map<std::string,std::string>& options)
    {
#ifdef FASTJET_USE
        MAbool IDflag = false;
        std::string new_jetid;
        std::map<std::string, std::string> clustering_params;

        // decide if its good to keep this jet
        MAbool save = true;
        ClusterAlgoBase* new_algo;
        // Loop over options
        for (std::map<std::string,std::string>::const_iterator
           it=options.begin();it!=options.end();it++)
        {
            std::string key = ClusterAlgoBase::Lower(it->first);
            MAbool result=false;
            if (key=="jetid")
            {
                // Check if JetID is used before
                std::map<std::string, ClusterAlgoBase* >::iterator 
                    jet_check = cluster_collection_.find(it->second);
                if (jet_check == cluster_collection_.end() && it->second != JetID_)
                {
                    new_jetid = it->second;
                    result    = true;
                    IDflag    = true;
                }
                else
                {
                    ERROR << "Jet ID '" << it->second 
                          << "' has already been defined. It will be ignored." << endmsg;
                    save = false;
                }
            }

            // Find the clustering algorithm 
            if (key=="algorithm")
            {
                if (it->second == "antikt")
                {
                    new_algo = new ClusterAlgoStandard("antikt");
                    result   = true;
                }
                else if (it->second == "cambridge")
                {
                    new_algo = new ClusterAlgoStandard("cambridge");
                    result   = true;
                }
                else if (it->second == "genkt")
                {
                    new_algo = new ClusterAlgoStandard("genkt");
                    result   = true;
                }
                else if (it->second == "kt")
                {
                    new_algo = new ClusterAlgoStandard("kt");
                    result   = true;
                }
                else if (it->second == "siscone")
                {
                    new_algo = new ClusterAlgoSISCone();
                    result   = true;
                }
                else if (it->second == "cdfmidpoint")
                {
                    new_algo = new ClusterAlgoCDFMidpoint();
                    result   = true;
                }
                else if (it->second == "cdfjetclu")
                {
                    new_algo = new ClusterAlgoCDFJetClu();
                    result   = true;
                }
                else if (it->second == "gridjet")
                {
                    new_algo = new ClusterAlgoGridJet();
                    result   = true;
                }
                else
                {
                    ERROR << "Unknown algorithm : " << it->second 
                          << ". It will be ignored." << endmsg;
                    result   = true;
                    save     = false;
                    break;
                }
            }
            // clustering algo -> keep the previous syntax 
            else if (key.find("cluster.")==0)
            {
                clustering_params.insert(std::pair<std::string,std::string>(key.substr(8),it->second));
                result = true;
            }

            // Other
            try
            {
              if (!result) throw EXCEPTION_WARNING("Parameter = "+key+" unknown. It will be skipped.","",0);
            }
            catch(const std::exception& e)
            {
              MANAGE_EXCEPTION(e);
            }
        }

        if (!IDflag)
        {
            ERROR << "Please define Jet ID. Jet will not be included in the analysis." << endmsg;
            save = false;
        }

        if (save)
        {
            cluster_collection_.insert(std::pair<std::string,ClusterAlgoBase*>(new_jetid,new_algo));
            for (std::map<std::string, std::string>::const_iterator
                 it=clustering_params.begin();it!=clustering_params.end();it++)
            { cluster_collection_[new_jetid]->SetParameter(it->first,it->second); }
            std::string algoname = cluster_collection_[new_jetid]->GetName();
            std::string params   = cluster_collection_[new_jetid]->GetParameters();
            INFO << "      - Adding Jet ID : " << new_jetid << endmsg;
            INFO << "            with algo : " << algoname << ", " << params << endmsg;
            cluster_collection_[new_jetid]->Initialize();
        }
#else
        ERROR << "FastJet has not been enabled. Can not add jets to the analysis." << endmsg;
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
