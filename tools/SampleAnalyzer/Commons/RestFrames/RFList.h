/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFList.h
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

#ifndef RFList_H
#define RFList_H

#include <vector>
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"

namespace RestFrames {
  
  class RFKey;
  class RFCharge;

  class RestFrame;
  class State;
  
  template <class T>
  class RFList;
  
  ///////////////////////////////////////////////
  // RFListBase class
  ///////////////////////////////////////////////
  template <class T, class Derived>
  class RFListBase {
  public:
    RFListBase() {}

    virtual ~RFListBase() {}
    
    void Clear();

    bool Add(T& obj);

    template <class U>
    bool Add(const RFList<U>& objs){
      int N = objs.GetN();
      double ret = true;
      for(int i = 0; i < N; i++) 
	if(!Add((T&)objs[i])) ret = false;
      return ret;
    }

    int Remove(const T& obj);
    
    template <class U>
    void Remove(const RFList<U>& objs){
      int N = objs.GetN();
      for(int i = 0; i < N; i++) 
	Remove(objs[i]);
    }

    int GetN() const { return m_Objs.size(); }

    T& Get(int i) const;

    T& Get(const RFKey& key) const;

    int GetIndex(const RFKey& key) const;

    int GetIndex(const T& obj) const;

    template <class U>
    bool Contains(const U& obj) const{
      int N = GetN();
      for(int i = 0; i < N; i++)
	if(*m_Objs[i] == obj) return true;
      return false;
    }

    template <class U>
    bool Contains(const RFList<U>& objs) const {
      int N = objs.GetN();
      for(int i = 0; i < N; i++)
	if(!Contains(objs[i]))
	  return false;
      return true;
    }
    
    template <class U>
    bool IsSame(const RFList<U>& objs) const {
      return Union(objs).GetN() == Intersection(objs).GetN();
    }

    template <class U>
    Derived Union(const RFList<U>& objs) const {
      Derived objs_this = static_cast<const Derived&>(*this);
      objs_this.Add(objs);
      return objs_this;
    }

    template <class U>
    Derived Intersection(const RFList<U>& objs) const {
      Derived inter; 
      int N = objs.GetN();
      for(int i = 0; i < N; i++)
	if(Contains(objs[i])) inter.Add(objs[i]);
      return inter;
    }

    template <class U>
    Derived Complement(const RFList<U>& objs) const {
      Derived comp = static_cast<Derived&>(*this);
      int N = objs.GetN();
      for(int i = 0; i < N; i++)
	if(comp.Contains(objs.Get(i))) 
	  comp.Remove(objs.Get(i));
      return comp;
    }

    Derived& operator = (T& obj){
      Clear();
      Add(obj);
      return static_cast<Derived&>(*this);
    }

    template <class U>
    Derived& operator = (const RFList<U>& objs){
      Clear();
      Add(objs);
      return static_cast<Derived&>(*this);
    }

    T& operator [] (int i) const;

    T& operator [] (const RFKey& key) const;

    bool operator == (const T& obj) const;

    template <class U>
    bool operator == (const RFList<U>& objs) const {
      return IsSame(objs);
    }

    bool operator != (const T& obj) const;

    template <class U>
    bool operator != (const RFList<U>& objs) const {
      return !IsSame(objs);
    }

    Derived operator + (T& obj) const;

    template <class U>
    Derived operator + (const RFList<U>& objs) const {
      Derived list = static_cast<const Derived&>(*this);
      list.Add(objs);
      return list;
    }

    Derived operator - (const T& obj) const;

    template <class U>
    Derived operator-(const RFList<U>& objs) const;

    Derived& operator += (T& obj);

    template <class U>
    Derived& operator += (const RFList<U>& objs){
      Add(objs);
      return static_cast<Derived&>(*this);
    }

    Derived& operator -= (const T& obj);

    template <class U>
    Derived& operator -= (const RFList<U>& objs){
      Remove(objs);
      return static_cast<Derived&>(*this);
    }
    
  protected:
    std::vector<T*> m_Objs;
  
  };
    
  ///////////////////////////////////////////////
  // RFList class
  ///////////////////////////////////////////////
  template <class T>
  class RFList : public RestFrames::RFListBase<T,RFList<T> > {
  public:
    RFList() : RFListBase<T,RFList<T> >() { }
    
    template <class U>
    RFList(const RFList<U>& objs) : RFListBase<T,RFList<T> >() {
      RFListBase<T,RFList<T> >::Add(objs);
    }

    virtual ~RFList(){ }
  };
  
  template <>
  class RFList<RestFrames::State>
    : public RFListBase<State,RFList<RestFrames::State> > {
  public:
    RFList() : RFListBase<State,RFList<State> >() { }

    template <class U>
    RFList(const RFList<U>& objs) : RFListBase<State,RFList<State> >() {
      Add(objs);
    }

    virtual ~RFList(){ }

    State& GetFrame(const RestFrame& frame) const;
    MA5::MALorentzVector GetFourVector() const;
    RFCharge GetCharge() const;
    void Boost(const MA5::MAVector3& B) const;
  };

  template <> 
  class RFList<RestFrames::RestFrame>
    : public RFListBase<RestFrame,RFList<RestFrames::RestFrame> > {
  public:
    RFList() : RFListBase<RestFrame,RFList<RestFrame> >() {}

    template <class U>
    RFList(const RFList<U>& objs) : RFListBase<RestFrame,RFList<RestFrame> >() {
      Add(objs);
    }

    virtual ~RFList(){ }

    double GetMass() const;

    MA5::MALorentzVector GetFourVector() const;
    MA5::MALorentzVector GetFourVector(const RestFrame& frame) const;
    MA5::MALorentzVector GetVisibleFourVector() const;
    MA5::MALorentzVector GetVisibleFourVector(const RestFrame& frame) const;
    MA5::MALorentzVector GetInvisibleFourVector() const;
    MA5::MALorentzVector GetInvisibleFourVector(const RestFrame& frame) const;
    double GetEnergy(const RestFrame& frame) const;
    double GetMomentum(const RestFrame& frame) const;
    RFCharge GetCharge() const;
  };

  class RFBase;
  class Jigsaw;
  class Group;
  class VisibleState;

  typedef RestFrames::RFList<RFBase>            RFBaseList;
  typedef RestFrames::RFList<RestFrame>         RestFrameList;
  typedef RestFrames::RFList<const RestFrame>   ConstRestFrameList;
  typedef RestFrames::RFList<Group>             GroupList;
  typedef RestFrames::RFList<const Group>       ConstGroupList;
  typedef RestFrames::RFList<Jigsaw>            JigsawList;
  typedef RestFrames::RFList<const Jigsaw>      ConstJigsawList;
  typedef RestFrames::RFList<State>             StateList;
  typedef RestFrames::RFList<VisibleState>      VisibleStateList;

  typedef std::vector<RestFrames::RFList<RestFrame> > RestFrameListList;
  typedef std::vector<RestFrames::RFList<State> >     StateListList;
}

#endif
