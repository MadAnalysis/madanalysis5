////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


// STL headers

// SampleAnalyzer headers
#include "SampleAnalyzer/Core/ProgressBar.h"

using namespace MA5;


// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
void ProgressBar::Initialize(UInt_t Nstep, 
                             Long64_t MinValue, Long64_t MaxValue)
{
  /// Initializing internal variables
  MinValue_=MinValue; MaxValue_=MaxValue; Nstep_=Nstep;
  Indicator_=0; MuteEnd_=false;
  if (Nstep==0 || MaxValue_<0 || MinValue_<0) {MuteInit_=true;return;}
  else MuteInit_=false; 

  /// Calcultating the threshold to reach
  Thresholds_.resize(Nstep_+1);
  for (UInt_t i=0;i<=Nstep_;i++) 
    Thresholds_[i] =   static_cast<Double_t>(MaxValue_-MinValue_) 
                     / static_cast<Double_t>(Nstep_)*i + MinValue_;

  /// Saving previous stream buffer related to std::cout
  oldstreambuf_ = std::cout.rdbuf();
  std::cout.flush();

  /// Assigning a new stream buffer with spy
  newstreambuf_ = new SpyStreamBuffer(oldstreambuf_);
  std::cout.rdbuf(newstreambuf_);
}


// -----------------------------------------------------------------------------
// Update
// -----------------------------------------------------------------------------
void ProgressBar::Update(Long64_t value)
{
  // Veto ?
  if (MuteInit_ || MuteEnd_) return;

  // Check if we must increase the progress bar
  if (value<Thresholds_[Indicator_]) return;

  // Check if the progress bar reach the end
  if (Indicator_>=Nstep_ || value>MaxValue_ || value<MinValue_)
  { MuteEnd_=true; return;}

  // Calculate how many steps to add
  UInt_t startpoint=Indicator_+1;

  for (UInt_t nextIndicator=startpoint;nextIndicator<Thresholds_.size();nextIndicator++)
  {
    if (value<Thresholds_[nextIndicator]) Indicator_++;
    else break;
  }

  // Display the progressbar
  Display(Indicator_);

  // Updateing progress bar indicator
  Indicator_++;
}


// -----------------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------------
void ProgressBar::Display(UInt_t ind) const
{
  // Preparing string to display
  std::string todisplay(Nstep_+2,' ');
  todisplay[0]='[';
  for (unsigned int i=0;i<ind;i++) todisplay[i+1] = '=';
  todisplay[ind+1]='>';
  todisplay[Nstep_+1]=']'; // overwrite the character '>' if ind=Nstep_

  // Go back to the line ? 
  if (ind!=0)
  {
    std::vector<char> last_chars = newstreambuf_->get_last_chars();
    if (last_chars[0] != '\n') std::cout << std::endl;
    else if (last_chars[1] == ']') std::cout << "\b\r";
  }

  std::cout << todisplay << std::endl;
}


// -----------------------------------------------------------------------------
// Finalize
// -----------------------------------------------------------------------------
void ProgressBar::Finalize()
{
  // Veto ?
  if (MuteInit_) return;

  // Display the indicator with a status of 100% ?
  if (!MuteEnd_) Display(Nstep_);

  // Restoring the initial stream buffer
  std::cout.rdbuf(oldstreambuf_);
  oldstreambuf_=0;
  if (newstreambuf_!=0) delete newstreambuf_;

  // Reset the progress bar
  Reset();
}
