/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   ReconstructionFrame.h
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

#ifndef ReconstructionFrame_H
#define ReconstructionFrame_H

#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/Group.h"

namespace RestFrames {
  
  ///////////////////////////////////////////////
  // ReconstructionFrame class
  ///////////////////////////////////////////////
  class ReconstructionFrame : public RestFrame {
  public:
    ReconstructionFrame(const std::string& sname, const std::string& stitle);
    ReconstructionFrame();
    virtual ~ReconstructionFrame();

    /// \brief Clears ReconstructionFrame of all connections to other objects
    virtual void Clear();

    /// \brief Add a child RestFrame to this frame
    ///
    /// \param frame    RestFrame to be added as child
    ///
    /// Method for adding a RestFrame *frame* as a child 
    /// of this frame. *frame* will not be added as a child
    /// if it is already listed as a child.
    virtual void AddChildFrame(RestFrame& frame);

    /// \brief Remove a child of this frame 
    ///
    /// \param frame     child frame to be removed
    ///
    /// Method for removing a child RestFrame from the
    /// list of children of this frame (if it is in that list).
    virtual void RemoveChildFrame(RestFrame& frame);

    /// \brief Remove all the children of this frame
    ///
    /// Method for removing all the children of this frame. 
    /// No child left behind.
    void RemoveChildFrames();
    
    /// \brief Set the parent frame for this frame
    ///
    /// \param frame     parent frame
    ///
    /// Method for connecting a child frame to its parent frame
    /// Empty default sets parent frame to none
    virtual void SetParentFrame(RestFrame& frame = 
				RestFrame::Empty());

    /// \brief Returns the parent of this frame
    ///
    /// Returns the parent frame of this frame.
    /// If the parent frame is not set, an empty
    /// frame is returned.
    virtual ReconstructionFrame const& GetParentFrame() const;

    /// \brief Get the frame of the *i* th child
    virtual ReconstructionFrame& GetChildFrame(int i = 0) const;

    virtual void SetGroup(Group& group = Group::Empty());
    Group& GetGroup() const;
    GroupList GetListGroups() const;

    static ReconstructionFrame& Empty();

  protected:
    bool InitializeAnalysisRecursive();
    bool ClearEventRecursive();
    bool AnalyzeEventRecursive();

    virtual bool ResetRecoFrame();
    virtual bool ReconstructFrame();

    virtual StateList const& GetChildStates(int i = 0) const;
    virtual StateList const& GetChildStates(const RestFrame& child) const;
    
  private:
    Group* m_GroupPtr;
    mutable std::map<const RestFrame*, StateList > m_ChildStates;
    
    bool InitializeVisibleStates();
    bool InitializeGroupStates();
    void FillListGroupsRecursive(GroupList& groups) const;

  };

}

#endif
