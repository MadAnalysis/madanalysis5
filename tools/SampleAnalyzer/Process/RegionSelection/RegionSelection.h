////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


#ifndef __REGIONSELECTION_H
#define __REGIONSELECTION_H


// STL headers
#include <string>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Counter/CounterManager.h"
#include "SampleAnalyzer/Process/Writer/SAFWriter.h"


namespace MA5
{

class RegionSelection
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private:
  std::string name_;
  MAbool surviving_;
  MAuint32 NumberOfCutsAppliedSoFar_;

  CounterManager cutflow_;

  // -------------------------------------------------------------
  //                      method members
  // -------------------------------------------------------------
 public :
  /// Constructor without argument
  RegionSelection() {name_ = "";};

  /// Constructor with argument
  RegionSelection(const std::string& name) { name_=name; };

  /// Destructor
  ~RegionSelection()  { };

  /// Get methods
  std::string GetName()
    { return name_; }

  MAbool IsSurviving()
    { return surviving_; }

  MAuint32 GetNumberOfCutsAppliedSoFar()
    { return NumberOfCutsAppliedSoFar_; }

  /// Printing the list of histograms
  void WriteDefinition(SAFWriter &output);

  /// Printing the cutflow
  void WriteCutflow(SAFWriter& output)
    { cutflow_.Write_TextFormat(output); }

  /// Set methods
  void SetName(std::string name)
    { name_ = name; }

  void SetSurvivingTest(MAbool surviving)
    { surviving_ = surviving; }

  void SetNumberOfCutsAppliedSoFar(MAuint32 NumberOfCutsAppliedSoFar)
    { NumberOfCutsAppliedSoFar_ = NumberOfCutsAppliedSoFar; }

  // Increment CutFlow (when this region passes a cut)
  void IncrementCutFlow(MAfloat64 weight)
  {
    cutflow_[NumberOfCutsAppliedSoFar_].Increment(weight);
    NumberOfCutsAppliedSoFar_++;
  }

  // Add a cut to the CutFlow
  void AddCut(std::string const &CutName)
    { cutflow_.InitCut(CutName); }

  /// Getting ready for a new event
  void InitializeForNewEvent(const MAfloat64 &weight)
  {
    SetSurvivingTest(true);
    SetNumberOfCutsAppliedSoFar(0);
    cutflow_.IncrementNInitial(weight);
  }

};

}

#endif
