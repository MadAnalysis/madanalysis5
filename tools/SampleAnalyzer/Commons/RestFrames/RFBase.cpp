/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   RFBase.cc
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

//#include "SampleAnalyzer/Commons/RestFrames/RestFrames_config.h"
#include "SampleAnalyzer/Commons/RestFrames/RFBase.h"

namespace RestFrames {

  using std::max;
  
  ///////////////////////////////////////////////
  // RFBase class methods
  ///////////////////////////////////////////////

  RFBase::RFBase()
    : m_Log(), m_Key(-1)
  {
    m_Name  = "Empty";
    m_Title = "Empty";
    m_Body   = false;
    m_Mind   = false;
    m_Spirit = false;
    m_This = this;
    m_Owns.clear();
    m_Log.SetSource("RFBase "+GetName());
  }

  RFBase::RFBase(const std::string& sname, 
		 const std::string& stitle, int key)
    : m_Log(), m_Key(key) {
    m_Name  = sname;
    m_Title = stitle;
    m_Body   = false;
    m_Mind   = false;
    m_Spirit = false;
    m_This = this;
    m_Owns.clear();
    m_Log.SetSource("RFBase "+GetName());
  }

  RFBase::~RFBase(){
    RFBase::Clear();
  }

  void RFBase::Clear(){
    SetBody(false);
    int N = m_Owns.size();
    for(int i = 0; i < N; i++){
      delete m_Owns[i];
    }
    m_Owns.clear();
  }

  RFBase& RFBase::Empty(){
    return RFBase::m_Empty;
  }

  bool RFBase::IsEmpty() const {
    return m_Key == -1;
  }

  void RFBase::AddDependent(RFBase* dep){
    if(dep) m_Owns.push_back(dep);
  }

  bool RFBase::IsSame(const RFKey& key) const {
    return (m_Key == key);
  }
     
  bool RFBase::IsSame(const RFBase& obj) const {
    return obj == m_Key;
  }

  RFKey RFBase::GetKey() const {
    return RFKey(m_Key);
  }

  std::string RFBase::GetName() const {
    return m_Name;
  }

  std::string RFBase::GetTitle() const {
    return m_Title;
  }

  bool RFBase::SetBody(bool body) const {
    m_Body = body;
    if(!body) SetMind(body);
    return m_Body;
  }

  bool RFBase::SetMind(bool mind) const {
    m_Mind = mind;
    if(!mind) SetSpirit(mind);
    return m_Mind;
  }

  bool RFBase::SetSpirit(bool spirit) const {
    m_Spirit = spirit;
    return m_Spirit;
  }

  bool RFBase::IsSoundBody() const {
    return m_Body;
  }

  bool RFBase::IsSoundMind() const {
    return m_Mind;
  }

  bool RFBase::IsSoundSpirit() const {
    return m_Spirit;
  }

  void RFBase::Print(LogType type) const {
    std::string output = PrintString(type);
    m_Log << type << output << LogEnd;
  }

  std::string RFBase::PrintString(LogType type) const {
    std::string output = "\n";
    output += "   Name: "+GetName()+"\n";
    output += "   Title: "+GetTitle()+"\n";
    return output;
  }

  void RFBase::UnSoundBody(const std::string& function) const {
    m_Log << LogWarning;
    m_Log << "Unable to evaluate function \"" << function << "\". ";
    m_Log << "Requires a successful call to \"InitializeTree()\" ";
    m_Log << "from the LabFrame associated with this tree.";
    m_Log << LogEnd;

    RFBase::m_BodyCount++;
    if(RFBase::m_BodyCount > m_WarningTolerance && 
       m_WarningTolerance > 0) TooManyBodies(*this);
  }

  void RFBase::UnSoundMind(const std::string& function) const {
    m_Log << LogWarning;
    m_Log << "Unable to evaluate function \"" << function << "\". ";
    m_Log << "Requires a successful call to \"InitializeAnalysis()\" ";
    m_Log << "from the LabFrame associated with this tree.";
    m_Log << LogEnd;

    RFBase::m_MindCount++;
    if(RFBase::m_MindCount > m_WarningTolerance && 
       m_WarningTolerance > 0) TooManyMinds(*this);
  }

  void RFBase::UnSoundSpirit(const std::string& function) const {
    m_Log << LogWarning;
    m_Log << "Unable to evaluate function \"" << function << "\". ";
    m_Log << "Requires a successful call to \"AnalyzeEvent()\" ";
    m_Log << "from the LabFrame associated with this tree.";
    m_Log << LogEnd;

    RFBase::m_SpiritCount++;
    if(RFBase::m_SpiritCount > m_WarningTolerance && 
       m_WarningTolerance > 0) TooManySpirits(*this);
  }

  // Initializer.
  __attribute__((constructor))
  static void initializer(void){
    printf("\n" "\x1b[36m");
   // printf(PACKAGE_NAME);
  //  printf(" v");
  //  printf(PACKAGE_VERSION);
    printf("Loading: RestFrames\n");
    printf(" -- Developed by Christopher Rogan (crogan@cern.ch)\n");
    printf("                     ");
    printf("Copyright (c) 2014-2018, Christopher Rogan\n");
    printf("                     ");
    printf("http://RestFrames.com\n");
    printf("\x1b[0m" "\n");
    RestFrames::RFKey key(0);
  }

  int RFBase::m_BodyCount = 0;
  int RFBase::m_MindCount = 0;
  int RFBase::m_SpiritCount = 0;

  int RFBase::m_WarningTolerance = 100;

  RFBase RFBase::m_Empty;

  const MA5::MAVector3       RFBase::m_Empty3Vector;
  const MA5::MALorentzVector RFBase::m_Empty4Vector;

  double GetP(double Mp, double Mc1, double Mc2){
    if(Mp <= 0.) return 0.;
    Mc1 = std::max(Mc1, 0.);
    Mc2 = std::max(Mc2, 0.);
    return sqrt(std::max(0., (Mp*Mp-Mc1*Mc1-Mc2*Mc2)*(Mp*Mp-Mc1*Mc1-Mc2*Mc2)-4.*Mc1*Mc1*Mc2*Mc2) )/2./Mp;
  }

  void SetWarningTolerance(int NMAX){
    RFBase::m_WarningTolerance = NMAX;
  }

  void TooManyBodies(const RFBase& obj){
    g_Log << LogError;
    g_Log << "Too many warnings. ";
    g_Log << "Need a successful call to \"InitializeTree()\" ";
    g_Log << "from the LabFrame associated with the offending/";
    g_Log << "unsuccessful function calls. The last call came from:";
    g_Log << Log(obj);
    g_Log << "Please edit your code and try again." << LogEnd;
  }

  void TooManyMinds(const RFBase& obj){
    g_Log << LogError;
    g_Log << "Too many warnings. ";
    g_Log << "Need a successful call to \"InitializeAnalysis()\" ";
    g_Log << "from the LabFrame associated with the offending/";
    g_Log << "unsuccessful function calls. The last call came from:";
    g_Log << Log(obj);
    g_Log << "Please edit your code and try again." << LogEnd;
  }

  void TooManySpirits(const RFBase& obj){
    g_Log << LogError;
    g_Log << "Too many warnings. ";
    g_Log << "Need a successful call to \"AnalyzeEvent()\" ";
    g_Log << "from the LabFrame associated with the offending/";
    g_Log << "unsuccessful function calls. The last call came from:";
    g_Log << Log(obj);
    g_Log << "Please edit your code and try again." << LogEnd;
  }

}
