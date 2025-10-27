/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2018, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RestFrame.cc
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2014 Jan
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

#include "SampleAnalyzer/Commons/RestFrames/RestFrame.h"
#include "SampleAnalyzer/Commons/RestFrames/ReconstructionFrame.h"
#include "SampleAnalyzer/Commons/Vector/MABoost.h"

namespace RestFrames {

  int RestFrame::m_class_key = 0;

  RestFrame::RestFrame() : RFBase() { 
    m_Type = kVanillaFrame; 
    m_Log.SetSource("SampleAnalyzer/Commons/RestFrame "+GetName());
  }
  
  RestFrame::~RestFrame(){ 
    Clear(); 
  }

  RestFrame::RestFrame(const std::string& sname, const std::string& stitle)
    : RFBase(sname, stitle, RestFrame::m_class_key++) 
  {
    m_Log.SetSource("RestFrame "+GetName());
    m_Type = kVanillaFrame;
    m_ParentFramePtr = nullptr;
    m_ParentBoost.SetXYZ(0.,0.,0.);
  }

  void RestFrame::Clear(){
    SetParentFrame();
    RemoveChildFrames();
  }
  
  RestFrame& RestFrame::Empty(){
    return ReconstructionFrame::Empty();
  }

  ConstRestFrameList const& RestFrame::EmptyList(){
    return m_EmptyList;
  }

  MA5::MAVector3 RestFrame::m_Axis = MA5::MAVector3(0.,0.,1.);

  void RestFrame::SetAxis(const MA5::MAVector3& axis){
    RestFrame::m_Axis = axis;
  }

  MA5::MAVector3 const& RestFrame::GetAxis(){
    return RestFrame::m_Axis;
  }

  RestFrameList RestFrame::operator + (RestFrame& frame){
    RestFrameList list;
    list.Add(frame);
    list.Add(*this);
    return list;
  }

  RestFrameList RestFrame::operator + (const RestFrameList& frames){
    RestFrameList list = frames;
    list.Add(*this);
    return list;
  }

  FrameType RestFrame::GetType() const { 
    return m_Type; 
  }

  bool RestFrame::IsVisibleFrame() const { 
    return m_Type == kVisibleFrame; 
  }

  bool RestFrame::IsInvisibleFrame() const { 
    return m_Type == kInvisibleFrame; 
  }

  bool RestFrame::IsDecayFrame() const { 
    return m_Type == kDecayFrame; 
  }

  bool RestFrame::IsLabFrame() const { 
    return m_Type == kLabFrame; 
  }

  bool RestFrame::IsRecoFrame() const { 
    return m_Ana == kRecoFrame; 
  }

  bool RestFrame::IsGenFrame() const { 
    return m_Ana == kGenFrame; 
  }

  std::string RestFrame::PrintString(LogType type) const {
    std::string output = RFBase::PrintString(type);
    if(IsLabFrame())
      output += "   Frame Type: Lab \n";
    if(IsDecayFrame())
      output += "   Frame Type: Decay \n";
    if(IsVisibleFrame())
      output += "   Frame Type: Visible \n";
    if(IsInvisibleFrame())
      output += "   Frame Type: Invisible \n";
    if(IsGenFrame())
      output += "   Ana Type: Generator \n";
    if(IsRecoFrame())
      output += "   Ana Type: Reconstruction \n";
    return output;
  }

  bool RestFrame::IsSoundBody() const {
    if(RFBase::IsSoundBody()) return true;
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++)
      if(!GetChildFrame(i)){
	m_Log << LogWarning << "Empty child frame:";
	m_Log << Log(GetChildFrame(i)) << LogEnd;
	return SetBody(false);
      }
    return SetBody(true);
  }

  bool RestFrame::InitializeTreeRecursive() {
    if(!IsSoundBody()) return false;
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++){
      if(!GetChildFrame(i).InitializeTreeRecursive()){
	m_Log << LogWarning;
	m_Log << "Problem with recursive tree structure from frame: ";
	m_Log << Log(GetChildFrame(i)) << LogEnd;
	return SetBody(false);
      }
    }
    return true;
  }

  void RestFrame::RemoveChildFrame(RestFrame& frame){
    SetBody(false);
    bool contains = m_ChildFrames.Contains(frame);
    m_ChildBoosts.erase(&frame);
    m_ChildFrames.Remove(frame);
    if(contains)
      frame.SetParentFrame();
  }

  void RestFrame::RemoveChildFrames(){
    SetBody(false);
    while(GetNChildren() > 0)
      RemoveChildFrame(m_ChildFrames[0]);
    m_ChildFrames.Clear();
    m_ChildBoosts.clear();
  }

  void RestFrame::SetParentFrame(RestFrame& frame){
    if(IsEmpty()) return;
    
    SetBody(false);
    
    RestFrame* prevPtr = m_ParentFramePtr;
    if(m_ParentFramePtr)
      if(*m_ParentFramePtr != frame){
	m_ParentFramePtr = nullptr;
	prevPtr->RemoveChildFrame(*this);
      }
    if(!frame)
      m_ParentFramePtr = nullptr;
    else
      m_ParentFramePtr = &frame;
  }
  
  void RestFrame::AddChildFrame(RestFrame& frame){
    if(IsEmpty()) return;
    
    SetBody(false);

    if(!frame){
      m_Log << LogWarning;
      m_Log << "Cannot add empty frame as child.";
      m_Log << LogEnd;
      return;
    }
    if(frame.IsLabFrame()){
      m_Log << LogWarning;
      m_Log << "Cannot add LabFrame frame as child:";
      m_Log << Log(frame) << LogEnd;
      return;
    }
    if(!m_ChildFrames.Add(frame)){
      m_Log << LogWarning;
      m_Log << "Unable to add child frame:";
      m_Log << Log(frame) << LogEnd;
      return;
    }
    frame.SetParentFrame(*this);
    m_ChildBoosts[&frame] = m_Empty3Vector;
  }

  void RestFrame::AddChildFrames(const RestFrameList& frames){
    int N = frames.GetN();
    for(int i = 0; i < N; i++)
      AddChildFrame(frames[i]);
  }

  int RestFrame::GetNChildren() const { 
    return m_ChildFrames.GetN();
  }

  RestFrame& RestFrame::GetChildFrame(int i) const {
    int Nchild = GetNChildren();
    if(i >= Nchild || i < 0){
      m_Log << LogWarning;
      m_Log << "Cannot GetChildFrame(" << i << "). ";
      m_Log << "No " << i << "th child" << LogEnd;
    }
    return m_ChildFrames[i];
  }

  RestFrame const& RestFrame::GetParentFrame() const {
    if(m_ParentFramePtr)
      return *m_ParentFramePtr;
    else 
      return RestFrame::Empty();
  }

  RestFrameList const& RestFrame::GetChildFrames() const {
    return m_ChildFrames;
  }

  RestFrame const& RestFrame::GetLabFrame() const {
    if(IsLabFrame()) 
      return *this;
    
    if(!GetParentFrame()){
      m_Log << LogWarning;
      m_Log << "Unable to find LabFrame above this frame. ";
      m_Log << "No parent frame set" << LogEnd;
      return RestFrame::Empty();
    } 
    return m_ParentFramePtr->GetLabFrame();
  } 

  RestFrame const& RestFrame::GetSiblingFrame() const {
    if(!IsSoundBody()) 
      return RestFrame::Empty();

    if(IsLabFrame()) 
      return RestFrame::Empty();

    if(!GetParentFrame())
      return RestFrame::Empty();

    int Nsib = GetParentFrame().GetNChildren();
    for(int s = 0; s < Nsib; s++){
      if(IsSame(m_ParentFramePtr->GetChildFrame(s))) 
	continue;
      return m_ParentFramePtr->GetChildFrame(s);
    }
    return RestFrame::Empty(); 
  }

  int RestFrame::GetNDescendants() const {
    if(!IsSoundBody()) return 0.;

    int Nchild = GetNChildren();
    if(Nchild == 0) return 1;
    int Nd = 0;
    for(int i = 0; i < Nchild; i++){
      Nd += GetChildFrame(i).GetNDescendants();
    }
    return Nd;
  }

  void RestFrame::SetChildBoostVector(RestFrame& frame, const MA5::MAVector3& boost) {
    if(!m_ChildFrames.Contains(frame)){
      m_Log << LogWarning;
      m_Log << "Unable to set child's boost vector. ";
      m_Log << "Frame is not among children:";
      m_Log << Log(frame) << LogEnd;
      return;
    }
    m_ChildBoosts[&frame] = boost;
    frame.SetParentBoostVector(-1.*boost);
  }

  void RestFrame::SetParentBoostVector(const MA5::MAVector3& boost) {
    if(!GetParentFrame()){
      m_Log << LogWarning;
      m_Log << "Unable to set parent boost vector. ";
      m_Log << "No parent frame set.";
      m_Log << LogEnd;
      return;
    }
    m_ParentBoost = boost;
  }

  MA5::MAVector3 const& RestFrame::GetChildBoostVector(RestFrame& frame) const {
    if(!m_ChildFrames.Contains(frame)){
      m_Log << LogWarning;
      m_Log << "Unable to get child's boost vector. ";
      m_Log << "Frame is not among children:";
      m_Log << Log(frame) << LogEnd;
      return m_Empty3Vector;
    }
    return m_ChildBoosts[&frame];
  }

  MA5::MAVector3 const& RestFrame::GetParentBoostVector() const {
    return m_ParentBoost;
  }

  RestFrameList RestFrame::GetListFrames(FrameType type) const {
    RestFrameList frames;
    FillListFramesRecursive(frames,type);
    return frames;
  }

  RestFrameList RestFrame::GetListVisibleFrames() const {
    return GetListFrames(kVisibleFrame);
  }

  RestFrameList RestFrame::GetListInvisibleFrames() const {
    return GetListFrames(kInvisibleFrame);
  }
  
  void RestFrame::FillListFramesRecursive(RestFrameList& frames, FrameType type) const {
    if(frames.Contains(*this)) return;
    if(type == GetType() || type == kLabFrame) frames.Add((RestFrame&)(*m_This));
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++)
      GetChildFrame(i).FillListFramesRecursive(frames, type);
  }

  bool RestFrame::IsCircularTree(std::vector<RFKey>& keys) const {
    int Nkey = keys.size();
    for(int i = 0; i < Nkey; i++){
      if(keys[i] == GetKey()){
	m_Log << LogWarning;
	m_Log << "This RestFrame appears more than once in the tree:";
	m_Log << Log(*this) << LogEnd;
	return true;
      }
    }
    keys.push_back(GetKey());
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++)
      if(GetChildFrame(i).IsCircularTree(keys))
	return true;
       
    return false;
  }

  bool RestFrame::FindPathToFrame(const RestFrame& dest_frame,
				  const RestFrame& prev_frame, 
				  std::vector<const MA5::MAVector3*>& boosts) const {
    if(IsSame(dest_frame)) return true;
  
    std::vector<const RestFrame*> try_frames;
    std::vector<const MA5::MAVector3*> try_boosts;

    if(!GetParentFrame().IsEmpty()){
      try_frames.push_back(&GetParentFrame());
      try_boosts.push_back(&GetParentBoostVector());
    }
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++){
      try_frames.push_back(&GetChildFrame(i));
      try_boosts.push_back(&GetChildBoostVector(GetChildFrame(i)));
    }

    int Ntry = try_frames.size();
    for(int i = 0; i < Ntry; i++){
      const RestFrame* nextPtr = try_frames[i];
      if(nextPtr->IsSame(prev_frame)) continue;
      boosts.push_back(try_boosts[i]);
      if(nextPtr->FindPathToFrame(dest_frame,*this,boosts)) 
	return true;
      boosts.pop_back();
    }
    return false;
  }

  void RestFrame::SetFourVector(const MA5::MALorentzVector& V, 
				const RestFrame& frame){
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return;
    }
    if(!frame)
      m_ProdFramePtr = &GetLabFrame();
    else 
      m_ProdFramePtr = &frame; 

    m_P = V; 
  }

  //////////////////////////////
  // User Analysis functions
  //////////////////////////////

  RFCharge RestFrame::GetCharge() const {
    RFCharge charge;
    
    if(!IsSoundBody()){
      UnSoundBody(RF_FUNCTION);
      return charge;
    }

    int Nchild = GetNChildren();
    if(Nchild == 0) return charge;
    for(int i = 0; i < Nchild; i++){
      charge += GetChildFrame(i).GetCharge();
    }
    return charge;
  }

  double RestFrame::GetMass() const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }
    return m_P.M();
  }

  MA5::MALorentzVector RestFrame::GetFourVector(const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return m_Empty4Vector;
    }

    if(!frame)
      return GetFourVector(GetLabFrame());
   
    if(!GetProductionFrame()){
      m_Log << LogWarning;
      m_Log << "Unable to get four vector. ";
      m_Log << "Production frame is not defined." << LogEnd;
      return m_Empty4Vector;
    }

    MA5::MALorentzVector V = m_P;
    if(V.E() <= 0.)
      return m_Empty4Vector;
    if(frame == GetProductionFrame()) return V;

    std::vector<const MA5::MAVector3*> boosts;
    if(!GetProductionFrame().
       FindPathToFrame(frame, RestFrame::Empty(), boosts)){
      m_Log << LogWarning;
      m_Log << "Unable to get four vector. ";
      m_Log << "Cannot find a path to frame " << frame.GetName();
      m_Log << " from frame " << GetProductionFrame().GetName() << LogEnd;
      return m_Empty4Vector;
    }
  
    int Nboost = boosts.size();
    MA5::MABoost Booster;
    for(int i = 0; i < Nboost; i++)
    {
      MA5::MAVector3 bvector = -1.*(*boosts[i]);
      Booster.setBoostVector(bvector.X(), bvector.Y(), bvector.Z());
      Booster.boost(V);
    }
    return V;
  }

  MA5::MALorentzVector RestFrame::GetVisibleFourVector(const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return m_Empty4Vector;
    }

    if(!frame){
      if(!GetLabFrame())
	return m_Empty4Vector;
      else
	return GetVisibleFourVector(GetLabFrame());
    }
    
    if(IsVisibleFrame())
      return GetFourVector(frame);

    MA5::MALorentzVector V(0.,0.,0.,0.);
    int Nc = GetNChildren();
    for(int c = 0; c < Nc; c++){
      RestFrameList frames = GetChildFrame(c).GetListVisibleFrames();
      int Nf = frames.GetN();
      for(int f = 0; f < Nf; f++) 
	V += frames[f].GetFourVector(frame);
    }
    return V;
  }

  MA5::MALorentzVector RestFrame::GetInvisibleFourVector(const RestFrame& frame) const {
   if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return m_Empty4Vector;
   }

   if(!frame){
     if(!GetLabFrame())
       return m_Empty4Vector;
     else
       return GetInvisibleFourVector(GetLabFrame());
   }

    if(IsInvisibleFrame())
      return GetFourVector(frame);
   
   MA5::MALorentzVector V(0.,0.,0.,0.);
   int Nc = GetNChildren();
   for(int c = 0; c < Nc; c++){
     RestFrameList frames = GetChildFrame(c).GetListInvisibleFrames();
     int Nf = frames.GetN();
     for(int f = 0; f < Nf; f++) 
       V += frames[f].GetFourVector(frame);
   }
   return V;
  }

  double RestFrame::GetEnergy(const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }
    return GetFourVector(frame).E();
  }

  double RestFrame::GetMomentum(const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }
    return GetFourVector(frame).P();
  }

  double RestFrame::GetTransverseMomentum(const RestFrame& frame,
					  const MA5::MAVector3& axis, 
					  const RestFrame& axis_frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    if(frame == axis_frame){
      MA5::MAVector3 V = GetFourVector(frame).Vect();
      return (V-V.Dot(axis.Unit())*axis.Unit()).Mag();
    }

    MA5::MALorentzVector Pthis  = GetFourVector(axis_frame);

    MA5::MALorentzVector Pframe;
    if(!frame || frame.IsLabFrame()){
      Pframe = axis_frame.GetFourVector(frame);
      Pframe.SetVectM(-Pframe.Vect(),Pframe.M());
    } else {
      Pframe = frame.GetFourVector(axis_frame);
    }

    MA5::MABoost Booster;
    Booster.setBoostVector(Pframe);
    MA5::MAVector3 boost_par = Booster.BoostVector();
    boost_par = -boost_par.Dot(axis.Unit())*axis.Unit();
    Booster.setBoostVector(boost_par.X(), boost_par.Y(), boost_par.Z());
    Booster.boost(Pthis);
    Booster.boost(Pframe);
    Booster.setBoostVector(-Pframe);
    Booster.boost(Pthis);
   
    MA5::MAVector3 V = Pthis.Vect();
    return (V-V.Dot(axis.Unit())*axis.Unit()).Mag();
  }

  MA5::MALorentzVector RestFrame::GetFourVector(const MA5::MALorentzVector& P, 
					   const RestFrame& def_frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return m_Empty4Vector;
    }

    if(IsSame(def_frame) || (!def_frame && IsLabFrame()))
      return P;

    MA5::MALorentzVector Pret = P;
    
    std::vector<const MA5::MAVector3*> boosts;
    if(!def_frame){
      if(!GetLabFrame().
	 FindPathToFrame(*this, RestFrame::Empty(), boosts)){
	m_Log << LogWarning;
	m_Log << "Unable to get four vector. ";
	m_Log << "Cannot find a path to frame " << GetName();
	m_Log << " from frame " << GetLabFrame().GetName() << LogEnd;
	return m_Empty4Vector;
      }
    } else {
       if(!def_frame.
	 FindPathToFrame(*this, RestFrame::Empty(), boosts)){
	m_Log << LogWarning;
	m_Log << "Unable to get four vector. ";
	m_Log << "Cannot find a path to frame " << GetName();
	m_Log << " from frame " << GetLabFrame().GetName() << LogEnd;
	return m_Empty4Vector;
       }
    }
    int Nboost = boosts.size();
    MA5::MABoost Booster;
    for(int i = 0; i < Nboost; i++)
    {
      MA5::MAVector3 bvector = -1.*(*boosts[i]);
      Booster.setBoostVector(bvector.X(), bvector.Y(), bvector.Z());
      Booster.boost(Pret);
    }
    return Pret;
  }

  double RestFrame::GetTransverseMomentum(const MA5::MALorentzVector& P,
					   const MA5::MAVector3& axis, 
					   const RestFrame& axis_frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    if(IsLabFrame() && (!axis_frame || axis_frame.IsLabFrame())){
      MA5::MAVector3 V = P.Vect();
      return (V-V.Dot(axis.Unit())*axis.Unit()).Mag();
    }

    MA5::MALorentzVector Pret = P;

    // move P to axis_frame
    if(!axis_frame.IsLabFrame() && !axis_frame.IsEmpty()){
      std::vector<const MA5::MAVector3*> boosts;
      if(!GetLabFrame().
	 FindPathToFrame(axis_frame, RestFrame::Empty(), boosts)){
	m_Log << LogWarning;
	m_Log << "Unable to get four vector. ";
	m_Log << "Cannot find a path to frame " << axis_frame.GetName();
	m_Log << " from frame " << GetLabFrame().GetName() << LogEnd;
	return 0.;
      }
      int Nboost = boosts.size();
      MA5::MABoost Booster;
      for(int i = 0; i < Nboost; i++)
      {
        MA5::MAVector3 bvector = -1.*(*boosts[i]);
        Booster.setBoostVector(bvector.X(), bvector.Y(), bvector.Z());
        Booster.boost(Pret);
      }
    }

    if(*this == axis_frame){
      MA5::MAVector3 V = Pret.Vect();
      return (V-V.Dot(axis.Unit())*axis.Unit()).Mag();
    }

    MA5::MALorentzVector Pthis;
    if(IsLabFrame()){
      Pthis = axis_frame.GetFourVector();
      Pthis.SetVectM(-Pthis.Vect(),Pthis.M());
    } else {
      Pthis = GetFourVector(axis_frame);
    }

    MA5::MABoost Booster;
    Booster.setBoostVector(Pthis);
    MA5::MAVector3 boost_par = Booster.BoostVector();
    boost_par = -boost_par.Dot(axis.Unit())*axis.Unit();
    Booster.setBoostVector(boost_par.X(), boost_par.Y(), boost_par.Z());

    Booster.boost(Pret);
    Booster.boost(Pthis);
    Booster.setBoostVector(-Pthis);
    Booster.boost(Pret);
   
    MA5::MAVector3 V = Pret.Vect();
    return (V-V.Dot(axis.Unit())*axis.Unit()).Mag();
  }
  

  int RestFrame::GetFrameDepth(const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    if(!frame) return -1;
    if(IsSame(frame)) return 0.;
    int Nchild = GetNChildren();
    for(int i = 0; i < Nchild; i++){
      int depth = GetChildFrame(i).GetFrameDepth(frame);
      if(depth >= 0) return depth+1;
    }
    return -1;
  }

  RestFrame const& RestFrame::GetFrameAtDepth(int depth, const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return RestFrame::Empty();
    }

    if(!frame || depth < 1) 
      return RestFrame::Empty();

    int N = GetNChildren();
    for(int i = 0; i < N; i++){
      RestFrame& child = GetChildFrame(i);
      if(child.GetListFrames().Contains(frame)){
	if(depth == 1) return child;
	else return child.GetFrameAtDepth(depth-1,frame);
      }
    }
    return RestFrame::Empty();
  }

  double RestFrame::GetVisibleShape() const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }
    double Psum = 0.;
    double Xsum = 0.;
    int N = GetNChildren();
    for(int i = 0; i < N; i++)
      Psum += GetChildFrame(i).GetVisibleFourVector(*this).P();
    for(int i = 0; i < N-1; i++){
      MA5::MAVector3 P1 = GetChildFrame(i).GetVisibleFourVector(*this).Vect();
      for(int j = i+1; j < N; j++){
	MA5::MAVector3 P2 = GetChildFrame(j).GetVisibleFourVector(*this).Vect();
	Xsum += (P1.Mag()+P2.Mag())*(P1.Mag()+P2.Mag())-(P1-P2).Mag2();
      }
    }
    if(Psum > 0.)
      return sqrt(Xsum)/Psum;
    else 
      return 0.;
  }

  double RestFrame::GetSumVisibleMomentum() const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    double ret = 0.;
    int N = GetNChildren();
    for(int i = 0; i < N; i++)
      ret += GetChildFrame(i).GetVisibleFourVector(*this).P();
  
    return ret;
  }

  double RestFrame::GetSumInvisibleMomentum() const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    double ret = 0.;
    int N = GetNChildren();
    for(int i = 0; i < N; i++)
      ret += GetChildFrame(i).GetVisibleFourVector(*this).P();
  
    return ret;
  }

  RestFrame const& RestFrame::GetProductionFrame() const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return RestFrame::Empty();
    }

    if(m_ProdFramePtr)
      return *m_ProdFramePtr;
    else 
      return RestFrame::Empty();
  }

  MA5::MAVector3 RestFrame::GetBoostInParentFrame() const {
    MA5::MAVector3 V(0.,0.,0.);

    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return V;
    }

    if(!GetParentFrame()) return V;
    return -1.*GetParentBoostVector();
  }

  double RestFrame::GetGammaInParentFrame() const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    MA5::MAVector3 vbeta = GetBoostInParentFrame();
    double beta = std::min(1.,vbeta.Mag());
    return 1./sqrt(1.-beta*beta);
  }

  double RestFrame::GetCosDecayAngle(const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    MA5::MAVector3 V1 = GetParentBoostVector().Unit();
    if(IsLabFrame())
      V1 = RestFrame::GetAxis();
    MA5::MAVector3 V2;
    if(!frame.IsEmpty())
      V2 = frame.GetFourVector(*this).Vect().Unit();
    else 
      if(GetNChildren() < 1) 
	return 0.;
      else 
	V2 = GetChildFrame(0).GetFourVector(*this).Vect().Unit();
    
    return V1.Dot(V2);
  }

  MA5::MAVector3 RestFrame::GetDecayPlaneNormalVector(const RestFrame& frame) const {
    MA5::MAVector3 n = RestFrame::GetAxis();

    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return n;
    }

    if(!frame)
      if(GetNChildren() < 1) 
	return n;

    MA5::MAVector3 V1, V2;
    if(!IsLabFrame()){
      if(!frame)
	V1 = GetChildFrame(0).GetFourVector(GetParentFrame()).Vect();
      else
	V1 = frame.GetFourVector(GetParentFrame()).Vect();
      V2 = GetFourVector(GetParentFrame()).Vect();
    } else {
      if(!frame)
	V1 = GetChildFrame(0).GetFourVector(*this).Vect().Unit();
      else
	V1 = frame.GetFourVector(*this).Vect().Unit();
      V2 = n;
    }
    MA5::MAVector3 ret = V1.Cross(V2);
    if(ret.Mag() > 0)
      return ret.Unit();

    std::vector<MA5::MAVector3> tries;
    tries.push_back(MA5::MAVector3(1.,0.,0.));
    tries.push_back(MA5::MAVector3(0.,1.,0.));
    tries.push_back(MA5::MAVector3(0.,0.,1.));
    for(int i = 0; i < 3; i++){
      V2 = tries[i];
      ret = V1.Cross(V2);
      if(ret.Mag() > 0)
	return ret.Unit();
    }

    return n;		 
  }

  double RestFrame::GetDeltaPhiDecayPlanes(const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    if(!frame) return 0.;
    if(GetNChildren() < 1) return 0.;

    MA5::MAVector3 vNorm_frame = frame.GetDecayPlaneNormalVector();
    MA5::MAVector3 vNorm_this = GetDecayPlaneNormalVector();
    double dphi = vNorm_this.Angle(vNorm_frame);

    if(frame.GetFourVector(*this).Vect().Cross(vNorm_frame).Dot(vNorm_this) < 0.){
     // dphi = TMath::Pi()*2. - dphi;
     dphi = M_PI*2. - dphi;
    }

    return dphi;
  }
 
  double RestFrame::GetDeltaPhiDecayAngle(const MA5::MAVector3& axis, const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    if(IsLabFrame())
      return 0.;

    MA5::MALorentzVector Pthis   = GetFourVector(frame);
    MA5::MALorentzVector Pchild  = GetChildFrame(0).GetFourVector(frame);
    MA5::MABoost Booster;

    Booster.setBoostVector(Pthis);
    MA5::MAVector3 boost_par = Booster.BoostVector();
    boost_par = -boost_par.Dot(axis.Unit())*axis.Unit();
    Booster.setBoostVector(boost_par.X(), boost_par.Y(), boost_par.Z());
    Booster.boost(Pthis);
    Booster.boost(Pchild);
    Booster.setBoostVector(Pthis);
    MA5::MAVector3 V1 = Booster.BoostVector();
    Booster.setBoostVector(-Pthis);
    Booster.boost(Pchild);

    MA5::MAVector3 V2 = Pchild.Vect();
    V1 = V1 - V1.Dot(axis.Unit())*axis.Unit();
    V2 = V2 - V2.Dot(axis.Unit())*axis.Unit();
    return V1.Angle(V2);
  }

  // Get angle between 'this' boost and visible children in plane 
  // perpendicular to 3-vector 'axis', where axis is defined
  // in 'framePtr' (default gives lab frame). 
  double RestFrame::GetDeltaPhiBoostVisible(const MA5::MAVector3& axis, const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    MA5::MALorentzVector Pvis = GetVisibleFourVector(frame);
    MA5::MALorentzVector Pthis = GetFourVector(frame);
    MA5::MABoost Booster;
    Booster.setBoostVector(Pthis);
    MA5::MAVector3 boost_par = Booster.BoostVector();
    boost_par = -boost_par.Dot(axis.Unit())*axis.Unit();
    Booster.setBoostVector(boost_par.X(), boost_par.Y(), boost_par.Z());
    Booster.boost(Pthis);
    Booster.boost(Pvis);
    Booster.setBoostVector(Pthis);
    MA5::MAVector3 boost_perp = Booster.BoostVector();
    Booster.setBoostVector(-Pthis);
    Booster.boost(Pvis);

    MA5::MAVector3 V = Pvis.Vect();
    V = V - V.Dot(axis.Unit())*axis.Unit();
   
    return V.Angle(boost_perp);
  }

  // Get angle between 'this' decay axis (defined by first child)
  // and visible children in plane 
  // perpendicular to 3-vector 'axis', where axis is defined
  // in 'framePtr' (default gives lab frame). 
  double RestFrame::GetDeltaPhiDecayVisible(const MA5::MAVector3& axis, const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    if(GetNChildren() < 1) return 0.;

    MA5::MALorentzVector Pvis   = GetVisibleFourVector(frame);
    MA5::MALorentzVector Pchild = GetChildFrame(0).GetFourVector(frame);
    MA5::MALorentzVector Pthis  = GetFourVector(frame);
    MA5::MABoost Booster;
    Booster.setBoostVector(Pthis);
    MA5::MAVector3 boost_par = Booster.BoostVector();
    boost_par = -boost_par.Dot(axis.Unit())*axis.Unit();
    Booster.setBoostVector(boost_par.X(), boost_par.Y(), boost_par.Z());
    Booster.boost(Pthis);
    Booster.boost(Pvis);
    Booster.boost(Pchild);
    Booster.setBoostVector(-Pthis);
    Booster.boost(Pvis);
    Booster.boost(Pchild);

    MA5::MAVector3 Vv = Pvis.Vect();
    Vv = Vv - Vv.Dot(axis.Unit())*axis.Unit();
    MA5::MAVector3 Vc = Pchild.Vect();
    Vc = Vc - Vc.Dot(axis.Unit())*axis.Unit();
   
    return Vv.Angle(Vc);
  }
 
  // Get angle between the visible portions of children 1 and 2
  // in the plane perpendicular to 3-vector 'axis', where
  // axis is defined in 'framePtr' (default gives lab frame). 
  double RestFrame::GetDeltaPhiVisible(const MA5::MAVector3& axis, const RestFrame& frame) const {
    if(!IsSoundSpirit()){
      UnSoundSpirit(RF_FUNCTION);
      return 0.;
    }

    if(GetNChildren() != 2) return 0.;
 
    MA5::MALorentzVector Pthis = GetFourVector(frame);
    MA5::MALorentzVector P1 = GetChildFrame(0).GetVisibleFourVector(frame);
    MA5::MALorentzVector P2 = GetChildFrame(1).GetVisibleFourVector(frame);
    MA5::MABoost Booster;
    Booster.setBoostVector(Pthis);
    MA5::MAVector3 boost_par = Booster.BoostVector();
    boost_par = -boost_par.Dot(axis.Unit())*axis.Unit();
    Booster.setBoostVector(boost_par.X(), boost_par.Y(), boost_par.Z());

    Booster.boost(Pthis);
    Booster.boost(P1);
    Booster.boost(P2);
    Booster.setBoostVector(-Pthis);
    Booster.boost(P1);
    Booster.boost(P2);

    MA5::MAVector3 V1 = P1.Vect();
    MA5::MAVector3 V2 = P2.Vect();
    V1 = V1 - V1.Dot(axis.Unit())*axis.Unit();
    V2 = V2 - V2.Dot(axis.Unit())*axis.Unit();
   
    return V1.Angle(V2);
  }

  const ConstRestFrameList RestFrame::m_EmptyList;
  
}
