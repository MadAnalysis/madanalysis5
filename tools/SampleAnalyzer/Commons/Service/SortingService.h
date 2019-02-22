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


#ifndef SORT_SERVICE_h
#define SORT_SERVICE_h


// STL headers
#include <vector>
#include <algorithm>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


#define SORTER MA5::SortingService::getInstance()


namespace MA5
{

enum OrderingObservable{Eordering, Pordering, PTordering, 
                        ETordering, PXordering, PYordering,
                        PZordering, ETAordering};

struct PointerComparison
{
  template<typename T>
  static MAbool ESortPredicate(T* part1, 
                             T* part2)
  { return part1->e() > part2->e(); }

  template<typename T>
  static MAbool ETSortPredicate(T* part1, 
                              T* part2)
  { return part1->et() > part2->et(); }

  template<typename T>
  static MAbool PSortPredicate(T* part1, 
                             T* part2)
  { return part1->p() > part2->p(); }

  template<typename T>
  static MAbool PTSortPredicate(T* part1, 
                              T* part2)
  { return part1->pt() > part2->pt(); }

  template<typename T>
  static MAbool ETASortPredicate(T* part1, 
                               T* part2)
  { return part1->eta() > part2->eta(); }

  template<typename T>
  static MAbool PXSortPredicate(T* part1, 
                              T* part2)
  { return part1->px() > part2->px(); }

  template<typename T>
  static MAbool PYSortPredicate(T* part1, 
                       T* part2)
  { return part1->py() > part2->py(); }

  template<typename T>
  static MAbool PZSortPredicate(T* part1, 
                              T* part2)
  { return part1->pz() > part2->pz(); }

};

class SortingService
{
  // -------------------------------------------------------------
  //                      data members
  // -------------------------------------------------------------
  static SortingService* service_;
  // -------------------------------------------------------------

  //                      method members
  // -------------------------------------------------------------

public:
  /// GetInstance
  static SortingService* getInstance()
  {
    if (service_==0) service_ = new SortingService;
    return service_;
  }

  /// sort particle
  template<typename T> static void sort(std::vector<T*>& parts,
            OrderingObservable obs=PTordering)
  {
    if (obs==PTordering) 
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PTSortPredicate<T>);
    else if (obs==ETordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETSortPredicate<T>);
    else if (obs==Eordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ESortPredicate<T>);
    else if (obs==ETAordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETASortPredicate<T>);
    else if (obs==PXordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PXSortPredicate<T>);
    else if (obs==PYordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PYSortPredicate<T>);
    else if (obs==PZordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PZSortPredicate<T>);
  }

  /// rank filter
  static std::vector<const MCParticleFormat*> 
  rankFilter(std::vector<const MCParticleFormat*> ref, MAint16 rank,
             OrderingObservable obs=PTordering)
  {
    // rejecting case where rank equal to zero
    try
    {
      if (rank==0) throw EXCEPTION_WARNING("Rank equal to 0 is not possible. Allowed values are 1,2,3,... and -1,-2,-3,...","",0);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      return std::vector<const MCParticleFormat*>();
    }    

    // Number of particle is not correct
    if ( (static_cast<MAint32>(ref.size()) - 
          static_cast<MAint32>(std::abs(rank)))<0 ) 
      return std::vector<const MCParticleFormat*>();

    // Sorting reference collection of particles
    sort(ref,obs);

    // Keeping the only particle
    std::vector<const MCParticleFormat*> parts(1);
    if (rank>0) parts[0]=ref[rank-1];
    else parts[0]=ref[ref.size()+rank];

    // Saving tmp
    return parts;
  }

  /// rank filter
  static std::vector<const RecParticleFormat*> 
  rankFilter(std::vector<const RecParticleFormat*> ref, MAint16 rank,
             OrderingObservable obs=PTordering)
  {
    // rejecting case where rank equal to zero
    try
    {
      if (rank==0) throw EXCEPTION_WARNING("Rank equal to 0 is not possible. Allowed values are 1,2,3,... and -1,-2,-3,...","",0);
    }
    catch(const std::exception& e)
    {
      MANAGE_EXCEPTION(e);
      return std::vector<const RecParticleFormat*>();
    }    

    // Number of particle is not correct
    if ( (static_cast<MAint32>(ref.size()) - 
          static_cast<MAint32>(std::abs(rank)))<0 ) 
      return std::vector<const RecParticleFormat*>();

    // Sorting reference collection of particles
    sort(ref,obs);

    // Keeping the only particle
    std::vector<const RecParticleFormat*> parts(1);
    if (rank>0) parts[0]=ref[rank-1];
    else parts[0]=ref[ref.size()+rank];

    // Saving tmp
    return parts;
  }

};

}
#endif
