////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


// STL headers
#include <cstdlib>

// SampleAnalyzer headers
#include "SampleAnalyzer/Process/Core/ProgressBar.h"
#include "SampleAnalyzer/Commons/Service/LogStream.h"

using namespace MA5;


const std::string ProgressBar::header("        => progress: ");


// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
void ProgressBar::Initialize(UInt_t Nstep, 
                             Long64_t MinValue, Long64_t MaxValue)
{
  /// Initializing internal variables
  MinValue_   = MinValue; 
  MaxValue_   = MaxValue;
  Nstep_      = Nstep;
  Indicator_  = 0;
  MuteEnd_    = false;
  FirstTime_  = true;
  //  std::cout << "Indicator_=" << Indicator_ << std::endl;

  /// Problem with arguments
  if (Nstep_==0 || MaxValue_<0 || MinValue_<0) {MuteInit_=true;return;}
  else MuteInit_=false; 

  /// Calcultating the threshold to reach
  Thresholds_.resize(Nstep_+1);
  for (UInt_t i=0;i<=Nstep_;i++) 
  {
    Thresholds_[i] = static_cast<Long64_t>(
                           static_cast<Double_t>(MaxValue_-MinValue_) / 
                           static_cast<Double_t>(Nstep_)*i + MinValue_ );
  }
  //  std::cout << "eric Indicator_=" << Indicator_ << std::endl;

  /// Saving previous stream buffer related to std::cout
  oldstreambuf_cout_ = std::cout.rdbuf();
  std::cout.flush();
  oldstreambuf_cerr_ = std::cerr.rdbuf();
  std::cerr.flush();
  oldstreambuf_clog_ = std::clog.rdbuf();
  std::clog.flush();

  /// Assigning a new stream buffer with spy
  newstreambuf_cout_ = new SpyStreamBuffer(oldstreambuf_cout_);
  std::cout.rdbuf(newstreambuf_cout_);

  newstreambuf_cerr_ = new SpyStreamBuffer(oldstreambuf_cerr_);
  std::cout.rdbuf(newstreambuf_cerr_);

  newstreambuf_clog_ = new SpyStreamBuffer(oldstreambuf_clog_);
  std::cout.rdbuf(newstreambuf_clog_);

}


// -----------------------------------------------------------------------------
// Update
// -----------------------------------------------------------------------------
void ProgressBar::Update(Long64_t value)
{
  // Veto ?
  if (MuteInit_ || MuteEnd_) return;
  //  std::cout << "Indicator_=" << Indicator_ << std::endl;
  //  std::cout << "eric value = " << value << " / " << Thresholds_[0] << std::endl;
  // Check if we must increase the progress bar
  if (value<Thresholds_[Indicator_]) return;

  // Check if the progress bar reach the end
  if (Indicator_>Nstep_ || value<MinValue_)
  { MuteEnd_=true; return; }

  // Calculate how many steps to add
  UInt_t startpoint=Indicator_+1;

  for (UInt_t nextIndicator=startpoint;nextIndicator<Thresholds_.size();nextIndicator++)
  {
    if (value>Thresholds_[nextIndicator]) Indicator_++;
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
void ProgressBar::Display(UInt_t ind)
{
  // Preparing string to display
  std::string todisplay(Nstep_+2,' ');
  todisplay[0]='[';
  for (unsigned int i=0;i<ind;i++) todisplay[i+1] = '=';
  todisplay[ind+1]='>';
  todisplay[Nstep_+1]=']'; // overwrite the character '>' if ind=Nstep_

  bool newline=false;
  // Go back to the line ? 
  if (ind!=0 && !FirstTime_)
  {
    if (!newstreambuf_cout_->GetProgressBarMode()) newline=true;
    if (!newstreambuf_cerr_->GetProgressBarMode()) newline=true;
    if (!newstreambuf_clog_->GetProgressBarMode()) newline=true;
  }

  // Adding header
  unsigned int toremove = (header+todisplay).size();
  todisplay = header + "\x1b[34m"+ todisplay + "\x1b[0m";

  // Displaying
  newstreambuf_cout_->SetProgressBarMode(false);
  newstreambuf_cerr_->SetProgressBarMode(false);
  newstreambuf_clog_->SetProgressBarMode(false);
  if (newline) std::cout << std::endl;
  else if (!FirstTime_) std::cout << std::string(toremove,'\b');
  std::cout << todisplay;
  newstreambuf_cout_->SetProgressBarMode(true);
  newstreambuf_cerr_->SetProgressBarMode(true);
  newstreambuf_clog_->SetProgressBarMode(true);
  FirstTime_=false;
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
  newstreambuf_cout_->SetProgressBarMode(false);
  newstreambuf_cerr_->SetProgressBarMode(false);
  newstreambuf_clog_->SetProgressBarMode(false);
  std::cout << std::endl;

  // Restoring the initial stream buffer
  std::cout.rdbuf(oldstreambuf_cout_);
  std::cerr.rdbuf(oldstreambuf_cerr_);
  std::clog.rdbuf(oldstreambuf_clog_);
  oldstreambuf_cout_=0;
  oldstreambuf_cerr_=0;
  oldstreambuf_clog_=0;
  if (newstreambuf_cout_!=0) delete newstreambuf_cout_;
  if (newstreambuf_cerr_!=0) delete newstreambuf_cerr_;
  if (newstreambuf_clog_!=0) delete newstreambuf_clog_;

  // Reset the progress bar
  Reset();
}
