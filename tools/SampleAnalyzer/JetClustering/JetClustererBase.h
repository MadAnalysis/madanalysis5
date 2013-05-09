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


#ifndef JET_CLUSTERING_BASE_H
#define JET_CLUSTERING_BASE_H

//SampleAnalyser headers
#include "SampleAnalyzer/DataFormat/EventFormat.h"
#include "SampleAnalyzer/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Service/Physics.h"
#include "SampleAnalyzer/JetClustering/bTagger.h"
#include "SampleAnalyzer/JetClustering/cTagger.h"
#include "SampleAnalyzer/JetClustering/TauTagger.h"

//STL headers
#include <map>
#include <algorithm>
#include <locale>


namespace MA5
{

  class JetClustererBase
  {
    //--------------------------------------------------------------------------
    //                              data members
    //--------------------------------------------------------------------------
  protected :

    /// Pt min for the jets
    Double_t Ptmin_;

    /// Is the jet clustering exclusive ?
    Bool_t Exclusive_;

    /// Exclusive id for tau-elec-jet
    Bool_t ExclusiveId_;

    /// Tagger
    bTagger*    myBtagger_;
    cTagger*    myCtagger_;
    TauTagger*  myTAUtagger_;


    //--------------------------------------------------------------------------
    //                              method members
    //--------------------------------------------------------------------------
  public :

    /// Constructor without argument
    JetClustererBase () 
    {
      // Initializing common parameters
      Ptmin_       = 0.;
      Exclusive_   = false;
      ExclusiveId_ = false;

      // Initializing tagger
      myBtagger_   = 0;
      myCtagger_   = 0;
      myTAUtagger_ = 0;
    }

    /// Destructor
    virtual ~JetClustererBase()
    { }

    /// Jet clustering
    virtual bool Execute(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Initialization
    virtual bool Initialize(const std::map<std::string,std::string>& options)=0;

    /// Finalization
    virtual void Finalize()
    { TaggerFinalize(); }

    /// Print parameters
    virtual void PrintParam()=0;

    /// Accessor to the jet clusterer name
    virtual std::string GetName()=0;

    /// Accessor to the jet clusterer parameters
    virtual std::string GetParameters()=0;

    std::string bParameters()
    { return myBtagger_->GetParameters(); }

    std::string tauParameters()
    { return myTAUtagger_->GetParameters(); }


  protected:


    /// Tagger initialization
    void TaggerInitialize()
    {
      myBtagger_   = new bTagger();
      myCtagger_   = new cTagger();
      myTAUtagger_ = new TauTagger();
    }

    /// Tagger finalization
    void TaggerFinalize()
    {
      if (myBtagger_!=0)   delete myBtagger_;
      if (myCtagger_!=0)   delete myCtagger_;
      if (myTAUtagger_!=0) delete myTAUtagger_;
    }

    /// Putting the string in lower case
    static std::string Lower(const std::string& word)
    {
      std::string result;
      std::transform(word.begin(), word.end(), 
                     std::back_inserter(result), 
                     (int (*)(int))std::tolower);
      return result;
    }

    /// 
    void SettingsCommonPart(const std::string& key, const std::string& value)
    {
      // exclusive_id
      if (key=="exclusive_id")
      {
        int tmp=0;
        std::stringstream str;
        str << value;
        str >> tmp;
        if (tmp==1) ExclusiveId_=true;
        else if (tmp==0) ExclusiveId_=false;
        else
        {
          WARNING << "'exclusive_id' must be equal to 0 or 1. "
                  << "Using default value 'exclusive_id' = " 
                  << ExclusiveId_ << endmsg;
        }
      }

      // b-tagging
      else if (key.find("bjet_id.")==0)
      {
        myBtagger_->SetParameter(key.substr(8),value,"bjet_id.");
      }

      // tau-tagging
      else if (key.find("tau_id.")==0)
      {
        myTAUtagger_->SetParameter(key.substr(7),value,"tau_id.");
      }

      // Other
      else WARNING << "Parameter " << key	<< " unknown." << endmsg;
    }


  };

}

#endif
