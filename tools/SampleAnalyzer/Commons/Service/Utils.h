////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file Utils.h
 * @brief Helpers for expert-mode analyses: vector concatenation, object filtering and overlap removal.
 */

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

    // ============================= //
    // ===== Vector operations ===== //
    // ============================= //

    /**
     * @brief Concatenate two vectors (the inputs are not modified).
     *
     * @tparam T element type.
     * @param A first vector.
     * @param B second vector.
     * @return a new vector containing A followed by B.
     */
    template <typename T>
    std::vector<T> operator+(const std::vector<T> &A, const std::vector<T> &B)
    {
        std::vector<T> AB;
        AB.reserve(A.size() + B.size());          // preallocate memory
        AB.insert(AB.end(), A.begin(), A.end());  // add A;
        AB.insert(AB.end(), B.begin(), B.end());  // add B;
        return AB;
    }

    /**
     * @brief Append a vector to another one.
     *
     * @tparam T element type.
     * @param A vector modified in place.
     * @param B vector to append.
     * @return A.
     */
    template <typename T>
    std::vector<T> &operator+=(std::vector<T> &A, const std::vector<T> &B)
    {
        A.reserve(A.size() + B.size());         // preallocate memory without erase original data
        A.insert(A.end(), B.begin(), B.end());  // add B;
        return A;                               // here A could be named AB
    }

    //===========//
    // Filtering //
    //===========//

    /**
     * @brief Select the objects passing kinematic thresholds.
     *
     * Example: `auto signaljets = filter(event.rec()->jets(), 25., 2.5);`
     *
     * @tparam Type object type.
     * @param objects objects.
     * @param ptmin minimum pT.
     * @param absetamax maximum |eta|.
     * @param absetamin minimum |eta|.
     * @return pointers to the selected objects.
     */
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

    /**
     * @brief Select the objects passing kinematic thresholds.
     *
     * @tparam Type object type.
     * @param objects pointers to the objects.
     * @param ptmin minimum pT.
     * @param absetamax maximum |eta|.
     * @param absetamin minimum |eta|.
     * @return pointers to the selected objects.
     */
    template<class Type>
    std::vector<const Type *> filter(std::vector<const Type *>& objects, MAfloat64 ptmin,
                                     MAfloat64 absetamax=20., MAfloat64 absetamin=-1.)
    {
        std::vector<const Type*> filtered;
        for(auto & obj: objects)
        {
            if(obj->pt() < ptmin || obj->abseta() > absetamax || obj->abseta() < absetamin) continue;
            filtered.push_back(obj);
        }
        return filtered;
    }

    /**
     * @brief Select the objects for which a predicate is true.
     *
     * Example: `filter_select(event.rec()->jets(), [] (const RecJetFormat* jet) { return jet->pt()>50.; });`
     * (the predicate receives a pointer).
     *
     * @tparam Type object type.
     * @tparam FN predicate type.
     * @param objects objects.
     * @param func predicate taking a pointer to an object.
     * @return pointers to the selected objects.
     */
    template<class Type, typename FN>
    std::vector<const Type *> filter_select(const std::vector<Type>& objects, FN func)
    {
        std::vector<const Type *> filtered;
        for (auto & obj: objects)
            if (func(&obj)) filtered.push_back(&obj);

        return filtered;
    }

    /**
     * @brief Select the objects for which a predicate is true.
     *
     * @tparam Type object type.
     * @tparam FN predicate type.
     * @param objects pointers to the objects.
     * @param func predicate taking a pointer to an object.
     * @return pointers to the selected objects.
     */
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

    /**
     * @brief Remove from v1 the objects closer than drmin to any object of v2.
     *
     * @tparam T1 type of the first collection.
     * @tparam T2 type of the second collection.
     * @param v1 collection to clean.
     * @param v2 reference collection.
     * @param drmin minimum DeltaR.
     * @return the cleaned collection.
     */
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

    /**
     * @brief Remove from v1 the objects for which a predicate is true with any object of v2.
     *
     * Example:
     * @code
     * signaljets = conditional_removal(signaljets, signalel,
     *     [] (const RecJetFormat * jet, const RecLeptonFormat * el) {return jet->dr(el) > 0.2;});
     * @endcode
     *
     * @tparam T1 type of the first collection.
     * @tparam T2 type of the second collection.
     * @tparam FN predicate type.
     * @param v1 collection to clean.
     * @param v2 reference collection.
     * @param func predicate (object of v1, object of v2) -> remove?
     * @return the cleaned collection.
     */
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
