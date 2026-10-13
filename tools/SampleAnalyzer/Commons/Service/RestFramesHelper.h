////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2024 Jack Araz, Eric Conte & Benjamin Fuks
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
//  Inspired from the Helpers developed by the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

/**
 * @file RestFramesHelper.h
 * @brief Helper owning the RestFrames objects (frames, groups, jigsaws) of an expert-mode analysis.
 */

#ifndef RESTFRAMESHELPER_H
#define RESTFRAMESHELPER_H

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/RestFrames/RestFrames.h"

using namespace RestFrames;

namespace MA5
{
  /**
   * @brief Factory and owner of RestFrames objects, accessible by name.
   *
   * All the objects created with the add* methods are deleted by the destructor.
   */
  class RestFramesHelper
  {
    private:
      /** @brief All the objects created by the helper (owned). */
      std::vector<RFBase *> RF_Objects;
      /** @brief Objects of each type, indexed by name. */
      std::map<std::string, LabRecoFrame *> RF_LabFrames;
      std::map<std::string, DecayRecoFrame *> RF_DecayFrames;
      std::map<std::string, VisibleRecoFrame *> RF_VisFrames;
      std::map<std::string, InvisibleRecoFrame *> RF_InvFrames;
      std::map<std::string, CombinatoricGroup *> RF_CombGroups;
      std::map<std::string, InvisibleGroup *> RF_InvGroups;
      std::map<std::string, InvisibleJigsaw *> RF_InvJigsaws;
      std::map<std::string, MinMassesCombJigsaw *> RF_CombJigsaws;

    public:
      /** @brief Constructor. */
      RestFramesHelper() {}

      /** @brief Destructor (deletes all the created objects). */
      ~RestFramesHelper()
      {
        int N = RF_Objects.size();
        for (int i = 0; i < N; i++) delete RF_Objects[i];
        RF_Objects.clear();
        RF_LabFrames.clear();
        RF_DecayFrames.clear();
        RF_VisFrames.clear();
        RF_InvFrames.clear();
        RF_InvGroups.clear();
        RF_CombGroups.clear();
        RF_InvJigsaws.clear();
        RF_CombJigsaws.clear();
      }

      /**
       * @brief Create a laboratory frame.
       *
       * @param name name (also used as title).
       */
      void addLabFrame(const std::string &name)
      {
        LabRecoFrame *frame = new LabRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_LabFrames[name] = frame;
      }
      /**
       * @brief Create a decay frame.
       *
       * @param name name (also used as title).
       */
      void addDecayFrame(const std::string &name)
      {
        DecayRecoFrame *frame = new DecayRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_DecayFrames[name] = frame;
      }
      /**
       * @brief Create a visible frame.
       *
       * @param name name (also used as title).
       */
      void addVisibleFrame(const std::string &name)
      {
        VisibleRecoFrame *frame = new VisibleRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_VisFrames[name] = frame;
      }
      /**
       * @brief Create an invisible frame.
       *
       * @param name name (also used as title).
       */
      void addInvisibleFrame(const std::string &name)
      {
        InvisibleRecoFrame *frame = new InvisibleRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_InvFrames[name] = frame;
      }
      /**
       * @brief Create a combinatoric group.
       *
       * @param name name (also used as title).
       */
      void addCombinatoricGroup(const std::string &name)
      {
        CombinatoricGroup *group = new CombinatoricGroup(name, name);
        RF_Objects.push_back(group);
        RF_CombGroups[name] = group;
      }
      /**
       * @brief Create an invisible group.
       *
       * @param name name (also used as title).
       */
      void addInvisibleGroup(const std::string &name)
      {
        InvisibleGroup *group = new InvisibleGroup(name, name);
        RF_Objects.push_back(group);
        RF_InvGroups[name] = group;
      }
      /** @brief Types of invisible jigsaws. */
      enum InvJigsawType { kSetMass, kSetRapidity, kContraBoost };
      /**
       * @brief Create an invisible jigsaw.
       *
       * @param name name (also used as title).
       * @param type type of jigsaw.
       */
      void addInvisibleJigsaw(const std::string &name, InvJigsawType type)
      {
        InvisibleJigsaw *jigsaw = nullptr;
        if (type == kSetMass) jigsaw = new SetMassInvJigsaw(name, name);
        if (type == kSetRapidity) jigsaw = new SetRapidityInvJigsaw(name, name);
        if (type == kContraBoost) jigsaw = new ContraBoostInvJigsaw(name, name);
        RF_Objects.push_back(jigsaw);
        RF_InvJigsaws[name] = jigsaw;
      }
      /** @brief Types of combinatoric jigsaws. */
      enum CombJigsawType { kMinMasses };
      /**
       * @brief Create a combinatoric jigsaw.
       *
       * @param name name (also used as title).
       * @param type type of jigsaw.
       */
      void addCombinatoricJigsaw(const std::string &name, CombJigsawType type)
      {
        MinMassesCombJigsaw *jigsaw = nullptr;
        if (type == kMinMasses) jigsaw = new MinMassesCombJigsaw(name, name);
        RF_Objects.push_back(jigsaw);
        RF_CombJigsaws[name] = jigsaw;
      }

      /**
       * @brief Get a laboratory frame by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      LabRecoFrame *getLabFrame(const std::string &name) { return RF_LabFrames[name]; }
      /**
       * @brief Get a combinatoric group by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      CombinatoricGroup *getCombinatoricGroup(const std::string &name) { return RF_CombGroups[name]; }
      /**
       * @brief Get a visible frame by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      VisibleRecoFrame *getVisibleFrame(const std::string &name) { return RF_VisFrames[name]; }
      /**
       * @brief Get a decay frame by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      DecayRecoFrame *getDecayFrame(const std::string &name) { return RF_DecayFrames[name]; }
      /**
       * @brief Get a invisible frame by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      InvisibleRecoFrame *getInvisibleFrame(const std::string &name) { return RF_InvFrames[name]; }
      /**
       * @brief Get a invisible group by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      InvisibleGroup *getInvisibleGroup(const std::string &name) { return RF_InvGroups[name]; }
      /**
       * @brief Get a invisible jigsaw by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      InvisibleJigsaw *getInvisibleJigsaw(const std::string &name) { return RF_InvJigsaws[name]; }
      /**
       * @brief Get a combinatoric jigsaw by name.
       *
       * @param name name.
       * @return the object (null if unknown: the map inserts a null entry).
       */
      MinMassesCombJigsaw *getCombinatoricJigsaw(const std::string &name) { return RF_CombJigsaws[name]; }
  };

}
#endif
