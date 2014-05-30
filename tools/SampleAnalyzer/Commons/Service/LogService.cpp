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


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"

using namespace MA5;

/// Initializing the static member 
LogService* LogService::Service_ = 0;


/// Modifiying level of verbosity
void LogService::SetVerbosityLevel(VerbosityLevel level)
{
  // Converting to integer
  UInt_t myLevel = static_cast<UInt_t>(level);

  // Setting the internal data
  if (myLevel<=1) Level_=DEBUG_LEVEL;
  else if (myLevel>=5) Level_=ERROR_LEVEL;
  else if (myLevel==2) Level_=USER_LEVEL;
  else if (myLevel==3) Level_=INFO_LEVEL;
  else Level_=WARNING_LEVEL;

  // Mute/UnMute corresponding logger
  if (Level_==DEBUG_LEVEL)
  {
    Debug_.SetUnMute();
    SetGlobalMuteUser(false);
    Info_.SetUnMute();
    Warning_.SetUnMute();
    Error_.SetUnMute();
  }
  else if (Level_==USER_LEVEL)
  {
    Debug_.SetMute();
    SetGlobalMuteUser(false);
    Info_.SetUnMute();
    Warning_.SetUnMute();
    Error_.SetUnMute();
  }
  else if (Level_==INFO_LEVEL)
  {
    Debug_.SetMute();
    SetGlobalMuteUser(true);
    Info_.SetUnMute();
    Warning_.SetUnMute();
    Error_.SetUnMute();
  }
  else if (Level_==WARNING_LEVEL)
  {
    Debug_.SetMute();
    SetGlobalMuteUser(true);
    Info_.SetMute();
    Warning_.SetUnMute();
    Error_.SetUnMute();
  }
  else
  {
    Debug_.SetMute();
    SetGlobalMuteUser(true);
    Info_.SetMute();
    Warning_.SetMute();
    Error_.SetUnMute();
  }
}


