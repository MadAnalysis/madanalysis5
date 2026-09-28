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
 * @file RandomService.h
 * @brief Singleton random-number generator, available as RANDOM.
 */

#ifndef RANDOM_SERVICE_H
#define RANDOM_SERVICE_H


// STL headers 
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"


/** @brief Shortcut to the RandomService singleton. */
#define RANDOM MA5::RandomService::GetInstance()   


namespace MA5
{

/**
 * @brief Singleton random-number generator based on std::rand().
 *
 * The seed is taken from the current time unless SetSeed() is called (main.random_seed
 * in the Python interface).
 */
class RandomService
{
  // -------------------------------------------------------------
  //                        data members
  // -------------------------------------------------------------
 private :

  /** @brief Unique instance. */
  static RandomService* Service_;

  // -------------------------------------------------------------
  //                       method members
  // -------------------------------------------------------------
 private:

  /** @brief Constructor (seed from the current time). */
  RandomService() 
  {
    // select a random seed according to the time
    std::srand(time(0));
  }

  /** @brief Destructor. */
  ~RandomService()
  {}

  /** @brief (Re)initialise the generator (nothing to do). */
  void Initialize()
  { }

 public:

  /**
   * @brief Get the unique instance (created at the first call).
   *
   * @return the instance.
   */
  static RandomService* GetInstance()
  {
    if (Service_==0) Service_ = new RandomService;
    return Service_;
  }

  /**
   * @brief Set the random seed.
   *
   * @param seed seed.
   */
  static void SetSeed(int seed) { std::srand(seed); }

  /** @brief Delete the unique instance. */
  static void Kill()
  {
    if (Service_!=0) delete Service_;
    Service_=0;
  }

  /**
   * @brief Draw a number uniformly distributed in [0, 1] (both bounds included).
   *
   * @return the random number.
   */
  MAdouble64 flat() const
  {
    return static_cast<MAdouble64>(std::rand()) /
           static_cast<MAdouble64>(RAND_MAX); 
  }

};

}

#endif
