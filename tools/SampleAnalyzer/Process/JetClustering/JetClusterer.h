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


#ifndef JET_CLUSTERER_H
#define JET_CLUSTERER_H

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Base/ClusterAlgoBase.h"
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"
#include "SampleAnalyzer/Commons/Base/SFSTaggerBase.h"

#ifdef MA5_FASTJET_MODE
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoStandard.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoSISCone.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFMidpoint.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoCDFJetClu.h"
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoGridJet.h"
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"
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
        /// SFS smearer
        SmearerBase* mySmearer_;
        /// b/c/tau tagger
        SFSTaggerBase* myTagger_;

        /// pointer to tagger options
        SFSTaggerBaseOptions* myTaggerOptions_;

        /// Print SFS banner
        MAbool SFSbanner_;

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
            substructure_collection_.clear();
#endif
            mySmearer_   = 0;
            myTagger_    = 0;
            myTaggerOptions_ = 0;
            SFSbanner_ = true;
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
            if (SFSbanner_) {PrintSFSBanner(); SFSbanner_ = false;}
        }

        /// Generic Loader for tagger module
        void LoadTagger(SFSTaggerBase* tagger)
        {
            myTagger_ = tagger;
            myTagger_->Initialize();
            myTagger_->SetOptions(*myTaggerOptions_);
            if (SFSbanner_) {PrintSFSBanner(); SFSbanner_ = false;}
        }

        // Load additional Jets
        MAbool LoadJetConfiguration(std::map<std::string,std::string> options);

        /// Accessor to the jet clusterer name
        std::string GetName()
        {
            if (algo_==0) return "NotDefined";
            else return algo_->GetName();
        }

        /// Accessor to the tagger parameters
        void TaggerParameters()
        { myTagger_->PrintParam(); }

        /// Print parameters
        void PrintParam()
        { algo_->PrintParam(); }

        /// Accessor to the jet clusterer parameters
        std::string GetParameters()
        { return algo_->GetParameters(); }

        /// SFS Banner
        void PrintSFSBanner()
        {
            INFO << "   <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
            INFO << "   <>                                                              <>" << endmsg;
            INFO << "   <>     Simplified Fast Detector Simulation in MadAnalysis 5     <>" << endmsg;
            INFO << "   <>            Please cite arXiv:2006.09387 [hep-ph]             <>" << endmsg;
            if (mySmearer_->isPropagatorOn()) // cite particle propagator module
            {
                INFO << "   <>                                                              <>" << endmsg;
                INFO << "   <>            Particle Propagation in MadAnalysis 5             <>" << endmsg;
                INFO << "   <>            Please cite arXiv:2112.05163 [hep-ph]             <>" << endmsg;
                INFO << "   <>                                                              <>" << endmsg;
            }
            INFO << "   <>         https://madanalysis.irmp.ucl.ac.be/wiki/SFS          <>" << endmsg;
            INFO << "   <>                                                              <>" << endmsg;
            INFO << "   <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>" << endmsg;
        }

    private:
        MAbool IsLast(const MCParticleFormat* part, EventFormat& myEvent);
        void GetFinalState(const MCParticleFormat* part, std::set<const MCParticleFormat*>& finalstates);

    };

}

#endif
