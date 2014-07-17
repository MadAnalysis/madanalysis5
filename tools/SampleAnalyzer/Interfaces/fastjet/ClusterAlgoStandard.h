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


#ifndef JETCLUSTERING_H
#define JETCLUSTERING_H


//SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/ClusterAlgoFastJet.h"

namespace MA5
{

class ClusterAlgoStandard: public ClusterAlgoFastJet
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

    /// jet-radius parameter
    Double_t R_;

    /// generalised kt parameter
    Double_t p_;

//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor with algorithm
    ClusterAlgoStandard(std::string Algo): ClusterAlgoFastJet(Algo) 
    {R_=0.5; p_=-1.;}

    /// Destructor
    virtual ~ClusterAlgoStandard() 
    { }

    /// Initialization
    virtual bool Initialize();

    /// Set parameter
    virtual bool SetParameter(const std::string& key, const std::string& value);

    /// Print parameters
    virtual void PrintParam();

    /// Accessor to the jet clusterer name
    virtual std::string GetName();

    /// Accessor to the jet clusterer parameters
    virtual std::string GetParameters();

};

}

#endif

