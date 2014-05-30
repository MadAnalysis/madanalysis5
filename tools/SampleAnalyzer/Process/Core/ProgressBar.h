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


#ifndef ProgressBar_h
#define ProgressBar_h

// STL headers
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>

// ROOT headers
#include <Rtypes.h> 

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{


  class ProgressBar
  {

  // -------------------------------------------------------------
  //                        class SpySreamBuffer
  // -------------------------------------------------------------
  protected:

    class SpyStreamBuffer : public std::streambuf 
    {
    public:

      /// Constructor
      SpyStreamBuffer(std::streambuf* buf) : buf_(buf)
      {
        add_endl_=false;
        // no buffering, overflow on every char
        setp(0, 0);
      }

      /// Set ProgressBar mode
      void SetProgressBarMode(bool status=true)
      { add_endl_=status; }

      /// Accessor to ProgressBar mode status
      bool GetProgressBarMode() const
      { return add_endl_; }

      /// Overflow method
      virtual int_type overflow(int_type c)
      {
        if (add_endl_) 
        {
          buf_->sputc('\n');
          add_endl_=false;
        }
        buf_->sputc(c);
        return c;
      }
    private:

      std::streambuf* buf_;
      bool add_endl_;
    };

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
  protected:

    /// Start point of the progress bar
    Long64_t MinValue_;

    /// End point of the progress bar
    Long64_t MaxValue_;

    /// Number of steps
    UInt_t Nstep_;

    /// Progress indicator
    UInt_t Indicator_;

    /// Mute if bad initialization
    Bool_t MuteInit_;

    /// Mute if the progress bar reachs the end bound
    Bool_t MuteEnd_;

    /// First time
    Bool_t FirstTime_;

    /// Thresholds
    std::vector<Long64_t> Thresholds_;

    /// Pointer to the new stream buffer
    SpyStreamBuffer* newstreambuf_cout_;
    SpyStreamBuffer* newstreambuf_cerr_;
    SpyStreamBuffer* newstreambuf_clog_;

    /// Pointer to the old stream buffer
    std::streambuf* oldstreambuf_cout_;
    std::streambuf* oldstreambuf_cerr_;
    std::streambuf* oldstreambuf_clog_;

    static const std::string header;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

    /// Constructor without argument
    ProgressBar()
    {
      newstreambuf_cout_=0;
      newstreambuf_cerr_=0;
      newstreambuf_clog_=0;
      oldstreambuf_cout_=0;
      oldstreambuf_cerr_=0;
      oldstreambuf_clog_=0;
      Reset(); 
    }

    /// Destructor 
    ~ProgressBar() 
    { 
      if (newstreambuf_cout_!=0) delete newstreambuf_cout_;  
      if (newstreambuf_cerr_!=0) delete newstreambuf_cerr_; 
      if (newstreambuf_clog_!=0) delete newstreambuf_clog_; 
   }

    /// Reset
    void Reset()
    {
      MinValue_=0; MaxValue_=0; Nstep_=0; Indicator_=0; 
      MuteInit_=false; MuteEnd_=false; FirstTime_=true;
      Thresholds_.clear();
    }

    /// Initializing the progress bar
    void Initialize(UInt_t Nstep, 
                    Long64_t MinValue, Long64_t MaxValue);

    /// Updating the display of the progress bar
    void Update(Long64_t value);

    /// Finalizing the progress bar
    void Finalize();

    /// Displaying the progress bar
    void Display(UInt_t ind);
  };
}

#endif
