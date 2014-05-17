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


#ifndef JETCLUSTERINGCDFMIDPOINT_H
#define JETCLUSTERINGCDFMIDPOINT_H


//SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/JetClusteringPlugin.h"

namespace MA5
{

class JetClusteringCDFMidpoint: public JetClusteringPlugin
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :

    /// Radius
    Double_t R_;

    /// Overlap Threshold
    Double_t OverlapThreshold_;

    /// Seed Threshold
    Double_t SeedThreshold_;

    /// Cone Area Fraction
    Double_t ConeAreaFraction_;

//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor without argument
    JetClusteringCDFMidpoint() {R_=0.5; OverlapThreshold_=0.75; SeedThreshold_=1.0; ConeAreaFraction_=1.0;}

    /// Destructor
    virtual ~JetClusteringCDFMidpoint() {}

    /// Initialization
    virtual bool Initialize(const std::map<std::string,std::string>& options);

    /// Print parameters
    virtual void PrintParam();

    /// Accessor to the jet clusterer name
    virtual std::string GetName();

    /// Accessor to the jet clusterer parameters
    virtual std::string GetParameters();
};

}

#endif
