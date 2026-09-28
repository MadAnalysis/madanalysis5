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
 * @file DetectorBase.h
 * @brief Interface of the detector simulations (Delphes, Delphes-MA5tune).
 */

#ifndef DETECTOR_BASE_H
#define DETECTOR_BASE_H


// STL headers
#include <map>
#include <string>
#include <algorithm>
#include <locale>

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/DataFormat/EventFormat.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"


namespace MA5
{

  /** @brief Abstract base class of the detector simulations. */
  class DetectorBase
  {
    //--------------------------------------------------------------------------
    //                              data members
    //--------------------------------------------------------------------------
  protected :

    /** @brief Path of the detector configuration card. */
    std::string configFile_;

    //--------------------------------------------------------------------------
    //                              method members
    //--------------------------------------------------------------------------
  public :

    /** @brief Constructor. */
    DetectorBase () 
    { }

    /** @brief Destructor. */
    virtual ~DetectorBase()
    { }

    /**
     * @brief Simulate the detector response for an event.
     *
     * @param mySample current sample.
     * @param myEvent event (the reconstructed objects are filled).
     * @return false in case of error.
     */
    virtual MAbool Execute(SampleFormat& mySample, EventFormat& myEvent)=0;

    /**
     * @brief Initialise the detector simulation.
     *
     * @param configFile path of the detector card.
     * @param options additional options.
     * @return false in case of error.
     */
    virtual MAbool Initialize(const std::string& configFile, const std::map<std::string,std::string>& options)=0;

    /** @brief Finalise the detector simulation. */
    virtual void Finalize()=0;

    /** @brief Print the parameters of the detector simulation. */
    virtual void PrintParam()=0;

    /**
     * @brief Accessor to the name of the detector simulation.
     *
     * @return the name.
     */
    virtual std::string GetName()=0;

    /**
     * @brief Accessor to the parameters of the detector simulation.
     *
     * @return a printable summary of the parameters.
     */
    virtual std::string GetParameters()=0;

    /**
     * @brief Accessor to the path of the detector card.
     *
     * @return the path.
     */
    const std::string& GetConfigFile() const
    { return configFile_; }

    /**
     * @brief Get a printable description of the detector card.
     *
     * @return the description.
     */
    virtual const std::string PrintConfigFile() const
    { return "        with config card: "+GetConfigFile(); }

    /**
     * @brief Convert a string to lower case.
     *
     * @param word string.
     * @return the lower-case string.
     */
    static std::string Lower(const std::string& word)
    {
      std::string result;
      std::transform(word.begin(), word.end(), 
                     std::back_inserter(result), 
                     (MAint32 (*)(MAint32))std::tolower);
      return result;
    }

  protected:


  };

}

#endif
