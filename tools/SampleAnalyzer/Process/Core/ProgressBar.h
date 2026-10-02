////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file ProgressBar.h
 * @brief Progress bar of the event reading.
 */

#ifndef ProgressBar_h
#define ProgressBar_h


// STL headers
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h" 
#include "SampleAnalyzer/Commons/Service/LogService.h"


namespace MA5
{


  /**
   * @brief Progress bar displayed while reading a file.
   *
   * std::cout, std::cerr and std::clog are redirected through spy buffers so that any
   * message printed during the reading starts on a new line.
   */
  class ProgressBar
  {

  // -------------------------------------------------------------
  //                        class SpySreamBuffer
  // -------------------------------------------------------------
  protected:

    /** @brief Stream buffer inserting a new line before the first character written after the progress bar. */
    class SpyStreamBuffer : public std::streambuf 
    {
    public:

      /**
       * @brief Constructor.
       *
       * @param buf underlying stream buffer.
       */
      SpyStreamBuffer(std::streambuf* buf) : buf_(buf)
      {
        add_endl_=false;
        // no buffering, overflow on every character
        setp(0, 0);
      }

      /**
       * @brief Enable/disable the insertion of a new line.
       *
       * @param status true when the progress bar has just been drawn.
       */
      void SetProgressBarMode(MAbool status=true)
      { add_endl_=status; }

      /**
       * @brief Is the insertion of a new line enabled?
       *
       * @return the status.
       */
      MAbool GetProgressBarMode() const
      { return add_endl_; }

      /**
       * @brief Forward a character (after a new line if needed).
       *
       * @param c character.
       * @return the character.
       */
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

      /** @brief Underlying buffer and new-line flag. */
      std::streambuf* buf_;
      MAbool add_endl_;
    };

  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
  protected:

    /** @brief Start value (e.g. first byte of the file). */
    MAint64 MinValue_;

    /** @brief End value (e.g. last byte of the file). */
    MAint64 MaxValue_;

    /** @brief Number of steps of the bar. */
    MAuint32 Nstep_;

    /** @brief Current step. */
    MAuint32 Indicator_;

    /** @brief Muted after an invalid initialisation. */
    MAbool MuteInit_;

    /** @brief Muted once the end is reached. */
    MAbool MuteEnd_;

    /** @brief Is the bar drawn for the first time? */
    MAbool FirstTime_;

    /** @brief Values corresponding to each step. */
    std::vector<MAint64> Thresholds_;

    /** @brief Spy buffers of std::cout, std::cerr and std::clog. */
    SpyStreamBuffer* newstreambuf_cout_;
    SpyStreamBuffer* newstreambuf_cerr_;
    SpyStreamBuffer* newstreambuf_clog_;

    /** @brief Original buffers of std::cout, std::cerr and std::clog. */
    std::streambuf* oldstreambuf_cout_;
    std::streambuf* oldstreambuf_cerr_;
    std::streambuf* oldstreambuf_clog_;

    /** @brief Text displayed before the bar. */
    static const std::string header;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 public:

    /** @brief Constructor. */
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

    /** @brief Destructor (deletes the spy buffers). */
    ~ProgressBar() 
    { 
      // NOTE: Finalize() deletes the buffers without resetting the pointers: they are deleted twice here.
      if (newstreambuf_cout_!=0) delete newstreambuf_cout_;  
      if (newstreambuf_cerr_!=0) delete newstreambuf_cerr_; 
      if (newstreambuf_clog_!=0) delete newstreambuf_clog_; 
   }

    /** @brief Reset the internal state. */
    void Reset()
    {
      MinValue_=0; MaxValue_=0; Nstep_=0; Indicator_=0; 
      MuteInit_=false; MuteEnd_=false; FirstTime_=true;
      Thresholds_.clear();
    }

    /**
     * @brief Initialise the bar and redirect the standard streams.
     *
     * @param Nstep number of steps.
     * @param MinValue start value.
     * @param MaxValue end value.
     */
    void Initialize(MAuint32 Nstep, 
                    MAint64 MinValue, MAint64 MaxValue);

    /**
     * @brief Update the bar.
     *
     * @param value current value.
     */
    void Update(MAint64 value);

    /** @brief Draw the complete bar and restore the standard streams. */
    void Finalize();

    /**
     * @brief Draw the bar.
     *
     * @param ind current step.
     */
    void Display(MAuint32 ind);
  };
}

#endif
