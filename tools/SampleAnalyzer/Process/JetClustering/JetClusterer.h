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
#include "SampleAnalyzer/Process/JetClustering/bTagger.h"
#include "SampleAnalyzer/Process/JetClustering/cTagger.h"
#include "SampleAnalyzer/Process/JetClustering/TauTagger.h"
#include "SampleAnalyzer/Process/JetClustering/NullSmearer.h"
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"

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
      myBtagger_   = 0;
      myCtagger_   = 0;
      myTautagger_ = 0;
      mySmearer_   = 0;
      ExclusiveId_ = false;
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
