////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2014 Eric Conte, Benjamin Fuks
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

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"

#define SORTER MA5::SortingService::getInstance()


namespace MA5
{

enum OrderingObservable{Eordering, Pordering, PTordering, 
                        ETordering, PXordering, PYordering,
                        PZordering, ETAordering};

struct PointerComparison
{
  template<typename T>
  static bool ESortPredicate(T* part1, 
                             T* part2)
  { return part1->e() > part2->e(); }

  template<typename T>
  static bool ETSortPredicate(T* part1, 
                              T* part2)
  { return part1->et() > part2->et(); }

  template<typename T>
  static bool PSortPredicate(T* part1, 
                             T* part2)
  { return part1->p() > part2->p(); }

  template<typename T>
  static bool PTSortPredicate(T* part1, 
                              T* part2)
  { return part1->pt() > part2->pt(); }

  template<typename T>
  static bool ETASortPredicate(T* part1, 
                               T* part2)
  { return part1->eta() > part2->eta(); }

  template<typename T>
  static bool PXSortPredicate(T* part1, 
                              T* part2)
  { return part1->px() > part2->px(); }

  template<typename T>
  static bool PYSortPredicate(T* part1, 
                       T* part2)
  { return part1->py() > part2->py(); }

  template<typename T>
  static bool PZSortPredicate(T* part1, 
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
  static void sort(std::vector<const RecParticleFormat*>& parts,
            OrderingObservable obs=PTordering)
  {
    if (obs==PTordering) 
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PTSortPredicate<const RecParticleFormat>);
    else if (obs==ETordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETSortPredicate<const RecParticleFormat>);
    else if (obs==Eordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ESortPredicate<const RecParticleFormat>);
    else if (obs==ETAordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETASortPredicate<const RecParticleFormat>);
    else if (obs==PXordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PXSortPredicate<const RecParticleFormat>);
    else if (obs==PYordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PYSortPredicate<const RecParticleFormat>);
    else if (obs==PZordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PZSortPredicate<const RecParticleFormat>);
  }

  /// sort particle
  static void sort(std::vector<const MCParticleFormat*>& parts,
            OrderingObservable obs=PTordering)
  {
    if (obs==PTordering) 
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PTSortPredicate<const MCParticleFormat>);
    else if (obs==ETordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETSortPredicate<const MCParticleFormat>);
    else if (obs==Eordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ESortPredicate<const MCParticleFormat>);
    else if (obs==ETAordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::ETASortPredicate<const MCParticleFormat>);
    else if (obs==PXordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PXSortPredicate<const MCParticleFormat>);
    else if (obs==PYordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PYSortPredicate<const MCParticleFormat>);
    else if (obs==PZordering)
        std::sort(parts.begin(),parts.end(),
                  PointerComparison::PZSortPredicate<const MCParticleFormat>);
  }

  /// Sorting electrons
  static void sort(std::vector<const RecLeptonFormat*>& parts,
            OrderingObservable obs=PTordering)
  {
    // Converting to RecParticleFormat
    std::vector<const RecParticleFormat*> ConvertedLeptons;
    for(unsigned int ii=0; ii<parts.size(); ii++)
      ConvertedLeptons.push_back(parts[ii]);

    // Sorting the converted particles
    sort(ConvertedLeptons,obs);

    // Converting the sorted vector
    parts.resize(0);
    for(unsigned int ii=0; ii<ConvertedLeptons.size(); ii++)
      parts.push_back((const RecLeptonFormat*)(ConvertedLeptons[ii]));
  }

  /// Sorting jets
  static void sort(std::vector<const RecJetFormat*>& parts,
            OrderingObservable obs=PTordering)
  {
    // Converting to RecParticleFormat
    std::vector<const RecParticleFormat*> ConvertedJets;
    for(unsigned int ii=0; ii<parts.size(); ii++)
      ConvertedJets.push_back(parts[ii]);

    // Sorting the converted particles
    sort(ConvertedJets,obs);

    // Converting the sorted vector
    parts.resize(0);
    for(unsigned int ii=0; ii<ConvertedJets.size(); ii++)
      parts.push_back((const RecJetFormat*)(ConvertedJets[ii]));
  }

   /// Sorting taus
  static void sort(std::vector<const RecTauFormat*>& parts,
            OrderingObservable obs=PTordering)
  {
    // Converting to RecParticleFormat
    std::vector<const RecParticleFormat*> ConvertedTaus;
    for(unsigned int ii=0; ii<parts.size(); ii++)
      ConvertedTaus.push_back(parts[ii]);

    // Sorting the converted particles
    sort(ConvertedTaus,obs);

    // Converting the sorted vector
    parts.resize(0);
    for(unsigned int ii=0; ii<ConvertedTaus.size(); ii++)
      parts.push_back((const RecTauFormat*)(ConvertedTaus[ii]));
  }

  /// rank filter
  static std::vector<const MCParticleFormat*> 
  rankFilter(std::vector<const MCParticleFormat*> ref, Short_t rank,
             OrderingObservable obs=PTordering)
  {
    // rejecting case where rank equal to zero
    if (rank==0)
    {
      WARNING << "Rank equal to 0 is not possible. "
              << "Allowed values are 1,2,3,... and -1,-2,-3,..." << endmsg;
      return std::vector<const MCParticleFormat*>();
    }

    // Number of particle is not correct
    if ( (static_cast<Int_t>(ref.size()) - 
          static_cast<Int_t>(std::abs(rank)))<0 ) 
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
  rankFilter(std::vector<const RecParticleFormat*> ref, Short_t rank,
             OrderingObservable obs=PTordering)
  {
    // rejecting case where rank equal to zero
    if (rank==0)
    {
      WARNING << "Rank equal to 0 is not possible. "
              << "Allowed values are 1,2,3,... and -1,-2,-3,..." << endmsg;
      return std::vector<const RecParticleFormat*>();
    }

    // Number of particle is not correct
    if ( (static_cast<Int_t>(ref.size()) - 
          static_cast<Int_t>(std::abs(rank)))<0 ) 
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
