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
 * @file SortingService.h
 * @brief Sorting of particle collections and selection of the n-th ranked particle (SORTER).
 */

#ifndef SORT_SERVICE_h
#define SORT_SERVICE_h


// STL headers
#include <vector>
#include <algorithm>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"


/** @brief Shortcut to the SortingService singleton. */
#define SORTER MA5::SortingService::getInstance()


namespace MA5
{

/** @brief Observable used to order a collection (decreasing order). */
enum OrderingObservable{Eordering, Pordering, PTordering, 
                        ETordering, PXordering, PYordering,
                        PZordering, ETAordering};

/** @brief Comparison predicates (decreasing order) used by SortingService::sort. */
struct PointerComparison
{
  template<typename T>
  /**
   * @brief Order two particles by decreasing energy.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool ESortPredicate(T* part1, 
                             T* part2)
  { return part1->e() > part2->e(); }

  template<typename T>
  /**
   * @brief Order two particles by decreasing transverse energy.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool ETSortPredicate(T* part1, 
                              T* part2)
  { return part1->et() > part2->et(); }

  template<typename T>
  /**
   * @brief Order two particles by decreasing momentum magnitude.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool PSortPredicate(T* part1, 
                             T* part2)
  { return part1->p() > part2->p(); }

  template<typename T>
  /**
   * @brief Order two particles by decreasing transverse momentum.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool PTSortPredicate(T* part1, 
                              T* part2)
  { return part1->pt() > part2->pt(); }

  template<typename T>
  /**
   * @brief Order two particles by decreasing pseudorapidity.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool ETASortPredicate(T* part1, 
                               T* part2)
  { return part1->eta() > part2->eta(); }

  template<typename T>
  /**
   * @brief Order two particles by decreasing x component of the momentum.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool PXSortPredicate(T* part1, 
                              T* part2)
  { return part1->px() > part2->px(); }

  template<typename T>
  /**
   * @brief Order two particles by decreasing y component of the momentum.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool PYSortPredicate(T* part1, 
                       T* part2)
  { return part1->py() > part2->py(); }

  template<typename T>
  /**
   * @brief Order two particles by decreasing z component of the momentum.
   *
   * @tparam T particle type.
   * @param part1 first particle.
   * @param part2 second particle.
   * @return true if part1 comes first.
   */
  static MAbool PZSortPredicate(T* part1, 
                              T* part2)
  { return part1->pz() > part2->pz(); }

};

/** @brief Singleton sorting particle collections. */
class SortingService
{
  // -------------------------------------------------------------
  //                      data members
  // -------------------------------------------------------------
  /** @brief Unique instance. */
  static SortingService* service_;
  // -------------------------------------------------------------

  //                      method members
  // -------------------------------------------------------------

public:
  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static SortingService* getInstance()
  {
    if (service_==0) service_ = new SortingService;
    return service_;
  }

  /**
   * @brief Sort a collection of particle pointers in decreasing order of an observable.
   *
   * @tparam T particle type.
   * @param parts collection (sorted in place).
   * @param obs ordering observable.
   */
  template<typename T> static void sort(std::vector<T*>& parts,
            OrderingObservable obs=PTordering)
  {
    // FIXME: Pordering is not handled: the collection is left unsorted.
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

  /**
   * @brief Select the particle of a given rank.
   *
   * @param ref collection (copied, then sorted).
   * @param rank rank: 1, 2, ... from the first; -1, -2, ... from the last.
   * @param obs ordering observable.
   * @return a vector with the selected particle, or an empty vector if the rank is 0 or larger than the collection.
   */
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

  /**
   * @brief Select the particle of a given rank.
   *
   * @param ref collection (copied, then sorted).
   * @param rank rank: 1, 2, ... from the first; -1, -2, ... from the last.
   * @param obs ordering observable.
   * @return a vector with the selected particle, or an empty vector if the rank is 0 or larger than the collection.
   */
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
