/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   SelfAssemblingRecoFrame.h
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jan
///
//   This file is part of RestFrames.
//
//   RestFrames is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   RestFrames is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with RestFrames. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////

#ifndef RSelfAssemblingFrame_H
#define RSelfAssemblingFrame_H

#include "SampleAnalyzer/Commons/RestFrames/DecayRecoFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/State.h"

namespace RestFrames {

  class VisibleRecoFrame;
  
  ///////////////////////////////////////////////
  // SelfAssemblingRecoFrame class
  ///////////////////////////////////////////////
  class SelfAssemblingRecoFrame : public DecayRecoFrame {
  public:
    SelfAssemblingRecoFrame(const std::string& sname, 
			    const std::string& stitle);
    virtual ~SelfAssemblingRecoFrame();

    virtual void Clear();

    //virtual bool ClearEventRecursive();
    //virtual bool AnalyzeEventRecursive();

    void RemoveChildFrame(RestFrame& frame);

    const RestFrame& GetFrame(const RFKey& key) const;

  protected:
    virtual bool ResetRecoFrame();
    virtual bool ReconstructFrame();

  private:
    bool m_NewEvent;

    RestFrameList m_ChildFrames_UnAssembled;

    StateList m_VisibleStates;
    RestFrames::RFList<VisibleRecoFrame> m_VisibleFrames;
    RestFrames::RFList<DecayRecoFrame>   m_DecayFrames;
    int m_Nvisible;
    int m_Ndecay;

    void ClearNewFrames();

    bool m_IsAssembled;
    void Disassemble();
    void Assemble();
    void AssembleRecursive(RestFrame& frame, 
			   std::vector<RestFrame*>& frames, 
			   std::vector<MA5::MALorentzVector>& Ps);
    
    DecayRecoFrame& GetNewDecayFrame(const std::string& sname, 
				     const std::string& stitle);
    
    VisibleRecoFrame& GetNewVisibleFrame(const std::string& sname, 
					 const std::string& stitle);
  };

}

#endif
