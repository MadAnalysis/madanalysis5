////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


#ifndef BTAGGER_H
#define BTAGGER_H

#include "SampleAnalyzer/Commons/Base/TaggerBase.h"

namespace MA5
{

class bTagger:public TaggerBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------

 protected :

  /// Mis-identification efficiency
  Float_t misid_ljet_;

  /// Mis-identification efficiency
  Float_t misid_cjet_;

  /// Apply misefficiency
  Bool_t doMisefficiency_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor without argument
    bTagger() 
    {
      misid_cjet_=0.0;
      misid_ljet_=0.0;
      doMisefficiency_=false;
    }

    /// Destructor
    virtual ~bTagger() {}

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

    /// Is this B hadron the last in the decay chain ?
    Bool_t IsLastBHadron(MCParticleFormat* part, EventFormat& myEvent);

    /// Set a parameter
    virtual bool SetParameter(const std::string& key, const std::string& value,std::string header);

    virtual std::string GetParameters();

};

}

#endif
