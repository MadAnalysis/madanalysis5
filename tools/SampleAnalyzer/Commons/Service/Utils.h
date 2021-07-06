////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
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


#ifndef UTILITY_SERVICE_H
#define UTILITY_SERVICE_H


// STL headers
#include <set>
#include <string>
#include <algorithm>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/MCconfig.h"
#include "SampleAnalyzer/Commons/Service/RECconfig.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Service/TransverseVariables.h"
#include "SampleAnalyzer/Commons/Service/Identification.h"
#include "SampleAnalyzer/Commons/Service/Isolation.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

    // Example: std::vector<const RecJetFormat*> signaljets = filter<RecJetFormat>(event.rec()->jets(), ptmin, etamax);
    template<class Type>
    std::vector<const Type *> filter(std::vector<Type> objects, MAfloat64 ptmin, MAfloat64 etamax)
    {
      // Helper function to select subset of objects passing pt and eta selections
      std::vector<const Type *> filtered;
      for(MAuint32 i=0; i<objects.size(); i++)
      {
        if(objects[i].pt() < ptmin) continue;
        if(fabs(objects[i].eta()) > etamax) continue;
        filtered.push_back(&(objects[i]));
      }
      return filtered;
    }


    // Overlap Removal
    template<typename T1, typename T2> std::vector<const T1*>
      OverlapRemoval(std::vector<const T1*> &v1, std::vector<const T2*> &v2,
      const MAdouble64 &drmin)
    {
      // Determining with objects should be removed
      std::vector<bool> mask(v1.size(),false);
      for (MAuint32 j=0;j<v1.size();j++)
        for (MAuint32 i=0;i<v2.size();i++)
          if (v2[i]->dr(v1[j]) < drmin)
          {
            mask[j]=true;
            break;
          }

      // Building the cleaned container
      std::vector<const T1*> cleaned_v1;
      for (MAuint32 i=0;i<v1.size();i++)
        if (!mask[i]) cleaned_v1.push_back(v1[i]);

      return cleaned_v1;
    }

}
#endif
