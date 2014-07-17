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


#ifndef CTAGGER_H
#define CTAGGER_H

#include "SampleAnalyzer/Commons/Base/TaggerBase.h"

namespace MA5
{

class cTagger:public TaggerBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor without argument
    cTagger() {}

    /// Destructor
    virtual ~cTagger () {}

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

    /// Is this C hadron the last in the decay chain ?
    Bool_t IsLastCHadron(MCParticleFormat* part, EventFormat& myEvent);

    virtual std::string GetParameters();

};

}

#endif
