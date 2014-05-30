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


/// STL headers
#include <iomanip>
#include <algorithm>

/// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogReport.h"

using namespace MA5;

/// Displaying the table
void LogReport::WriteGenericReport(LogStream& os) const
{
  // Moving to a more flexible container
  std::vector<const std::pair<const LogMsgKey,LogMsgValue>* > table(MsgTable_.size());

  // Skipping print if table empty
  if (table.empty()) return; 

  UInt_t index=0;
  for (MsgConstIterator it = MsgTable_.begin(); it!=MsgTable_.end(); it++)
  { 
    table[index]=&(*it);
    index++;
  }

  // Sort the container by timing average value
  std::sort(table.begin(),table.end(),LogReport::OccurencyOrder);

  // Header
  os << "+";
  for (unsigned int i=0;i<78;i++) os << "-";
  os << "+" << endmsg;
    
  std::string title;
  if (Name_=="")  title="LogReport ";
  else title="LogReport-" + Name_;
  index=39-(title.size()/2); 
  os << "|";
  for (unsigned int i=0;i<(index-1);i++) os << " ";
  os << title;
  for (unsigned int i=(index+title.size());i<79;i++) os << " ";
  os << "|" << endmsg;

  os << "+";
  for (unsigned int i=0;i<78;i++) os << "-";
  os << "+" << endmsg;

  // Legend
  os << "| ";
  os.width(22); os << std::left << "Message";
  os.width(12); os << std::left << "NIterations";
  os.width(18); os << std::left << "@ File";
  os.width(7); os << std::left << "Line";
  os.width(18); os << std::left << "Function";
  os << "|" << endmsg;
    
  os << "|"; os.repeat('-',78); os << "|" << endmsg;


  UInt_t precision = os.precision();
  os.precision(5);

  /// Loop over the table
  for (std::vector<const std::pair<const LogMsgKey,LogMsgValue>* >::const_iterator
       it = table.begin(); it!=table.end(); it++)
  {
    os << "| ";
    os.width(22); os << std::left << (*it)->first.GetMsg();
    os.width(14); os << std::left << (*it)->second.GetCounter();
    os.width(16); os << std::left << std::scientific << (*it)->first.GetFileName();
    os.width(7); os << std::left << std::scientific << (*it)->first.GetLine();
    os.width(18); os << std::left << std::scientific << (*it)->second.GetFunction();
    os << "|" << endmsg;
  }

  /// Foot
  os.precision(precision);
  os << "+"; os.repeat('-',80); os << "+" << endmsg;
}

