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

#ifndef RESTFRAMESHELPER_H
#define RESTFRAMESHELPER_H

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/RestFrames/RestFrames.h"

using namespace RestFrames;

namespace MA5
{
  class RestFramesHelper
  {
    private:
      std::vector<RFBase *> RF_Objects;
      std::map<std::string, LabRecoFrame *> RF_LabFrames;
      std::map<std::string, DecayRecoFrame *> RF_DecayFrames;
      std::map<std::string, VisibleRecoFrame *> RF_VisFrames;
      std::map<std::string, InvisibleRecoFrame *> RF_InvFrames;
      std::map<std::string, CombinatoricGroup *> RF_CombGroups;
      std::map<std::string, InvisibleGroup *> RF_InvGroups;
      std::map<std::string, InvisibleJigsaw *> RF_InvJigsaws;
      std::map<std::string, MinMassesCombJigsaw *> RF_CombJigsaws;

    public:
      /// Constructor
      RestFramesHelper() {}

      /// Destructor
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

      // Declaration of new reference frames and combinatoric groups
      void addLabFrame(const std::string &name)
      {
        LabRecoFrame *frame = new LabRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_LabFrames[name] = frame;
      }
      void addDecayFrame(const std::string &name)
      {
        DecayRecoFrame *frame = new DecayRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_DecayFrames[name] = frame;
      }
      void addVisibleFrame(const std::string &name)
      {
        VisibleRecoFrame *frame = new VisibleRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_VisFrames[name] = frame;
      }
      void addInvisibleFrame(const std::string &name)
      {
        InvisibleRecoFrame *frame = new InvisibleRecoFrame(name, name);
        RF_Objects.push_back(frame);
        RF_InvFrames[name] = frame;
      }
      void addCombinatoricGroup(const std::string &name)
      {
        CombinatoricGroup *group = new CombinatoricGroup(name, name);
        RF_Objects.push_back(group);
        RF_CombGroups[name] = group;
      }
      void addInvisibleGroup(const std::string &name)
      {
        InvisibleGroup *group = new InvisibleGroup(name, name);
        RF_Objects.push_back(group);
        RF_InvGroups[name] = group;
      }
      enum InvJigsawType { kSetMass, kSetRapidity, kContraBoost };
      void addInvisibleJigsaw(const std::string &name, InvJigsawType type)
      {
        InvisibleJigsaw *jigsaw = nullptr;
        if (type == kSetMass) jigsaw = new SetMassInvJigsaw(name, name);
        if (type == kSetRapidity) jigsaw = new SetRapidityInvJigsaw(name, name);
        if (type == kContraBoost) jigsaw = new ContraBoostInvJigsaw(name, name);
        RF_Objects.push_back(jigsaw);
        RF_InvJigsaws[name] = jigsaw;
      }
      enum CombJigsawType { kMinMasses };
      void addCombinatoricJigsaw(const std::string &name, CombJigsawType type)
      {
        MinMassesCombJigsaw *jigsaw = nullptr;
        if (type == kMinMasses) jigsaw = new MinMassesCombJigsaw(name, name);
        RF_Objects.push_back(jigsaw);
        RF_CombJigsaws[name] = jigsaw;
      }

      // Accessors to reference frames and combinatoric groups
      LabRecoFrame *getLabFrame(const std::string &name) { return RF_LabFrames[name]; }
      CombinatoricGroup *getCombinatoricGroup(const std::string &name) { return RF_CombGroups[name]; }
      VisibleRecoFrame *getVisibleFrame(const std::string &name) { return RF_VisFrames[name]; }
      DecayRecoFrame *getDecayFrame(const std::string &name) { return RF_DecayFrames[name]; }
      InvisibleRecoFrame *getInvisibleFrame(const std::string &name) { return RF_InvFrames[name]; }
      InvisibleGroup *getInvisibleGroup(const std::string &name) { return RF_InvGroups[name]; }
      InvisibleJigsaw *getInvisibleJigsaw(const std::string &name) { return RF_InvJigsaws[name]; }
      MinMassesCombJigsaw *getCombinatoricJigsaw(const std::string &name) { return RF_CombJigsaws[name]; }
  };

}
#endif
