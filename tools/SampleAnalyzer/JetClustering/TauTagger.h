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


#ifndef TAUTAGGER_H
#define TAUTAGGER_H

//SampleAnalyser headers
#include "SampleAnalyzer/JetClustering/TaggerBase.h"


namespace MA5
{

class TauTagger: public TaggerBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
 protected :

  /// Mis-identification efficiency
  Float_t misid_ljet_;

  /// Apply misefficiency
  Bool_t doMisefficiency_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
 public :

    /// Constructor without argument
    TauTagger () 
    {
      misid_ljet_=0.0;
      doMisefficiency_=false;
    }

    /// Destructor
    ~TauTagger () { }

    /// Matching using dr
    virtual void Method1 (SampleFormat& mySample, EventFormat& myEvent);

    /// Matching using the history
    virtual void Method2 (SampleFormat& mySample, EventFormat& myEvent);

    /// Matching using a jet preselection with the history before calculating dr
    virtual void Method3 (SampleFormat& mySample, EventFormat& myEvent);

    /// Matching general method
    virtual void Execute(SampleFormat& mySample, EventFormat& myEvent)
    { 
      if (Method_==1) Method1(mySample,myEvent);
      else if (Method_==2) Method2(mySample,myEvent);
      else if (Method_==3) Method3(mySample,myEvent);
    }

    /// Fill the Tau format with the information from the jet format
    void Jet2Tau (RecJetFormat* myJet, RecTauFormat* myTau, EventFormat& myEvent);

    /// Set a parameter
    virtual void SetParameter(const std::string& key, const std::string& value,std::string header);

};

}

#endif
