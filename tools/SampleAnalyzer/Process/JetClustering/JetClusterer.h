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
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"
#include "SampleAnalyzer/Commons/Base/SFSTaggerBase.h"

#ifdef MA5_FASTJET_MODE
#include "SampleAnalyzer/Interfaces/substructure/ClusterBase.h"
#endif

// STL headers
#include <locale>


namespace MA5 {

    class ClusterAlgoBase;

    class JetClusterer {
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

        /// @brief Exclusive id for tau-elec-photon-jet
        /// @code ExclusiveId_ = true; @endcode
        /// Exclusive algorithm: FS Leptons (photons) originated from hadronic decays
        /// will not be included in Lepton (photon) collection.
        /// @code ExclusiveId_ = false; @endcode
        /// Includive algorithm: All FS leptons (photons) will be included in
        /// their corresponding containers.
        MAbool ExclusiveId_;

        /// Primary Jet ID
        std::string JetID_;

#ifdef MA5_FASTJET_MODE
        /// Jet collection configurations
        std::map<std::string, ClusterAlgoBase*> cluster_collection_;

        // Jet collection configuration with VariableR
        std::map<std::string, Substructure::ClusterBase*> substructure_collection_;
#endif

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

        /// Constructor
        JetClusterer (ClusterAlgoBase* algo);

        /// Destructor
        ~JetClusterer();

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
        std::string GetName();

        /// Accessor to the tagger parameters
        void TaggerParameters();

        /// Print parameters
        void PrintParam();

        /// Accessor to the jet clusterer parameters
        std::string GetParameters();

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
