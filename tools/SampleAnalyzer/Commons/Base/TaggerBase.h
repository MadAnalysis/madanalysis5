////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


#ifndef TAGGERBASE_H
#define TAGGERBASE_H


// STL headers
#include <algorithm>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Commons/Service/RandomService.h"


namespace MA5
{

class TaggerBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  protected :

    /// Method used
    MAint32 Method_;

    /// Delta R max
    MAfloat32 DeltaRmax_;

    /// Is the tagging exclusive ?
    MAbool Exclusive_;

    /// Efficiency
    MAfloat32 Efficiency_;

    /// Applying efficiency
    MAbool doEfficiency_;

//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor without argument
    TaggerBase() 
    {
      Method_=1; 
      DeltaRmax_=0.5; 
      Exclusive_=false;
      Efficiency_=1.;
      doEfficiency_=false;
    }

    /// Destructor
    virtual ~TaggerBase()
    {}

    /// Matching using dr
    virtual void Method1(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Matching using the history
    virtual void Method2(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Matching using a jet preselection with the history before calculating dr
    virtual void Method3(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Matching general method
    virtual void Execute(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Accessor to the selected method
    MAint32 GetMethod() 
    {return Method_;}

    /// 
    MAbool IsLast(MCParticleFormat* part, EventFormat& myEvent);

    /// Set a parameter
    virtual MAbool SetParameter(const std::string& key, const std::string& value, std::string header="");

    /// Function for identification
    MAbool IsIdentified() const
    {
      // no efficiency = default
      if (!doEfficiency_) return true;

      // applying efficiency
      MAdouble64 value = RANDOM->flat();
      if (value < Efficiency_) return true;
      else return false;
    }

    virtual std::string GetParameters()=0;

};

}

#endif
