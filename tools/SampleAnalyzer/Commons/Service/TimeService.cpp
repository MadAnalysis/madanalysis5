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
#include <iomanip>
#include <algorithm>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/TimeService.h"

using namespace MA5;

// Initializing the static member 
TimeService* TimeService::service_ = 0;


/// Displaying the table of timing measures
void TimeService::WriteGenericReport(LogStream& os) const
{
#ifndef TIMER_MODE

#else

  // Moving to a more flexible container
  std::vector<const std::pair<const std::string,TimeMeasureType>* > table(MeasureTable_.size());

  // Skipping print if table empty
  if (table.empty()) return; 

  unsigned int index=0;
  for (TimeConstIterator it = MeasureTable_.begin(); 
       it!=MeasureTable_.end(); it++)
  { 
    table[index]=&(*it);
    index++;
  }

  // Sort the container by timing average value
  std::sort(table.begin(),table.end(),TimeService::timingOrder);

  os << "+";
  for (unsigned int i=0;i<78;i++) os << "-";
  os << "+" << endmsg;
    
  os << "|";
  for (unsigned int i=0;i<34;i++) os << " ";
  os << "TimeReport";
  for (unsigned int i=0;i<34;i++) os << " ";
  os << "|" << endmsg;

  os << "+";
  for (unsigned int i=0;i<78;i++) os << "-";
  os << "+" << endmsg;

  os << "| ";
  os.width(16); os << std::left << "ProcessName";
  os.width(12); os << std::left << "NIterations";
  os.width(12); os << std::left << "Min(s)";
  os.width(12); os << std::left << "Average(s)";
  os.width(12); os << std::left << "Max(s)";
  os.width(13); os << std::left << "Deviation(s)";
  os << "|" << endmsg;
    
  os << "|";
  for (unsigned int i=0;i<78;i++) os << " ";
  os << "|" << endmsg;


  UInt_t precision = os.precision();
  os.precision(5);

  for (std::vector<const std::pair<const std::string,TimeMeasureType>* >::const_iterator
       it = table.begin(); it!=table.end(); it++)
  {
    os << "| ";
    os.width(16); os << std::left << (*it)->first;
    os.width(12); os << std::left << (*it)->second.GetNIterations();
    os.width(12); os << std::left << std::scientific << (*it)->second.GetMin();
    os.width(12); os << std::left << std::scientific << (*it)->second.GetAverage();
    os.width(12); os << std::left << std::scientific << (*it)->second.GetMax();
    os.width(13); os << std::left << std::scientific << (*it)->second.GetDeviation();
    os << "|" << endmsg;
  }

  os.precision(precision);

  os << "+";
  for (unsigned int i=0;i<78;i++) os << "-";
  os << "+" << endmsg;

#endif
}

