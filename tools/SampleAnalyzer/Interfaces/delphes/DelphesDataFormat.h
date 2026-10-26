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
 * @file DelphesDataFormat.h
 * @brief Branches and arrays of a Delphes ROOT tree.
 */

#ifndef DELPHES_DATA_FORMAT_h
#define DELPHES_DATA_FORMAT_h


// STL headers
#include <iostream>
#include <vector>
#include <map>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

// ROOT headers
#include <TTree.h>
#include <TClonesArray.h>


namespace MA5
{

/**
 * @brief Branches and TClonesArray buffers of a Delphes ROOT file (used by DelphesTreeReader).
 *
 * Standard Delphes branches are used, except when the MA5-tuned branches (JetMA5,
 * ElectronMA5, MuonMA5, PhotonMA5) are found; see InitializeBranch().
 */
struct DelphesDataFormat
{
  /// Pointers to data
  TClonesArray* Jet_;
  TClonesArray* FatJet_;
  TClonesArray* Electron_;
  TClonesArray* Photon_;
  TClonesArray* Muon_;
  TClonesArray* MET_;
  TClonesArray* HT_;
  TClonesArray* GenParticle_;
  TClonesArray* Track_;
  TClonesArray* Vertex_;
  TClonesArray* Tower_;
  TClonesArray* Event_;
  TClonesArray* Weight_;
  TClonesArray* EFlowTrack_;
  TClonesArray* EFlowPhoton_;
  TClonesArray* EFlowNeutral_;

  /// Pointers to branches
  TBranch* branchFatJet_;
  TBranch* branchJet_;
  TBranch* branchElectron_;
  TBranch* branchPhoton_;
  TBranch* branchMuon_;
  TBranch* branchMET_;
  TBranch* branchHT_;
  TBranch* branchGenParticle_;
  TBranch* branchTrack_;
  TBranch* branchVertex_;
  TBranch* branchTower_;
  TBranch* branchEvent_;
  TBranch* branchWeight_;
  TBranch* branchEFlowTrack_;
  TBranch* branchEFlowPhoton_;
  TBranch* branchEFlowNeutral_;

  /** @brief List of collections (unused). */
  std::vector<std::pair<std::string,std::string> > collections_;

  /** @brief Whether the file was produced with an MA5-tuned Delphes card. */
  MAbool delphesMA5card_;

  /** @brief Constructor (all pointers set to 0). */
  DelphesDataFormat();

  /** @brief Destructor (deletes the arrays). */
  ~DelphesDataFormat();

  /**
   * @brief Load a tree entry in all the available branches.
   *
   * @param treeEntry local entry number in the tree.
   * @return false if the reading of a branch failed.
   */
  MAbool GetEntry(MAint64 treeEntry);

  /** @brief Create the arrays of all the available branches. */
  void InitializeData();

  /**
   * @brief Find the branches of a Delphes tree (warnings are issued for the missing main branches).
   *
   * @param tree Delphes tree.
   */
  void InitializeBranch(TTree* tree);

  /**
   * @brief Create the array associated with a branch and set the branch address.
   *
   * @param branch branch (may be 0).
   * @param array array to allocate.
   * @return false if the branch is 0 or its class is unknown.
   */
  MAbool InitializeData(TBranch*& branch,TClonesArray*& array);

};

}

#endif
