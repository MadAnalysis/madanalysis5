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
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


namespace MA5
{

    //===========//
    // Filtering //
    //===========//

    // Example: std::vector<RecJetFormat> signaljets = filter(event.rec()->jets(), ptmin, etamax);
    template<class Type>
    std::vector<const Type *> filter(const std::vector<Type>& objects, MAfloat64 ptmin,
                                     MAfloat64 absetamax=20., MAfloat64 absetamin=-1.)
    {
        std::vector<const Type*> filtered;
        for(auto & obj: objects)
        {
            if(obj.pt() < ptmin || obj.abseta() > absetamax || obj.abseta() < absetamin) continue;
            filtered.push_back(&obj);
        }
        return filtered;
    }

    // Example:  std::vector<RecJetFormat> filtered_jets = filter_select(event.rec()->jets(),
    //                                        [] (RecJetFormat jet) { return jet->pt()>50.; });
    template<class Type, typename FN>
    std::vector<const Type *> filter_select(const std::vector<Type>& objects, FN func)
    {
        std::vector<const Type *> filtered;
        for (auto & obj: objects)
            if (func(&obj)) filtered.push_back(&obj);

        return filtered;
    }

    // Example:  std::vector<const RecJetFormat *> filtered_jets = filter_select(signaljets,
    //                                        [] (const RecJetFormat* jet) { return jet->pt()>50.; });
    template<class Type, typename FN>
    std::vector<const Type *> filter_select(std::vector<const Type *>& objects, FN func)
    {
        std::vector<const Type *> filtered;
        for (auto & obj: objects)
            if (func(obj)) filtered.push_back(obj);

        return filtered;
    }

    //=================//
    // Overlap Removal //
    //=================//

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

    // Example:  signaljets = conditional_removal(signaljets,signalel,
    //            [] (const RecJetFormat * jet, const RecLeptonFormat * el) {return jet->dr(el) > 0.2;});
    template<typename T1, typename T2, typename FN>
    std::vector<const T1*> conditional_removal(
        std::vector<const T1*> &v1, std::vector<const T2*> &v2, FN func
    )
    {
        // Determining with objects should be removed
        std::vector<bool> mask(v1.size(),false);
        for (MAuint32 j=0;j<v1.size();j++)
            for (MAuint32 i=0;i<v2.size();i++)
                if (func(v1[j], v2[i]))
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
