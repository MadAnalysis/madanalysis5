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


// STL headers
#include <iomanip>
#include <algorithm>
#include <vector>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogReport.h"


using namespace MA5;

/// Displaying the table
void LogReport::WriteGenericReport(LogStream& os) const
{
  // Moving to a more flexible container
  std::vector<const std::pair<const LogMsgKey,LogMsgValue>* > table(MsgTable_.size());

  // Skipping print if table empty
  if (table.empty()) return; 

  MAuint32 index=0;
  for (MsgConstIterator it = MsgTable_.begin(); it!=MsgTable_.end(); it++)
  { 
    table[index]=&(*it);
    index++;
  }

  // Sort the container by timing average value
  std::sort(table.begin(),table.end(),LogReport::OccurencyOrder);

  // Header
  os << "+";
  for (MAuint32 i=0;i<118;i++) os << "-";
  os << "+" << endmsg;
    
  std::string title;
  if (Name_=="")  title="LogReport ";
  else title="LogReport-" + Name_;
  index=39-(title.size()/2); 
  os << "|";
  for (MAuint32 i=0;i<(index-1);i++) os << " ";
  os << title;
  for (MAuint32 i=(index+title.size());i<119;i++) os << " ";
  os << "|" << endmsg;

  os << "+";
  for (MAuint32 i=0;i<118;i++) os << "-";
  os << "+" << endmsg;

  // Legend
  os << "| ";
  os.width(46); os << std::left << "Message";
  os.width(12); os << std::left << "NIterations";
  os.width(2);  os << std::left << "@";
  os.width(50); os << std::left << "File";
  os.width(7);  os << std::left << "Line";
  os << "|" << endmsg;
    
  os << "|"; os.repeat('-',118); os << "|" << endmsg;


  MAuint32 precision = os.precision();
  os.precision(5);

  /// Loop over the table
  for (std::vector<const std::pair<const LogMsgKey,LogMsgValue>* >::const_iterator
       it = table.begin(); it!=table.end(); it++)
  {
    os << "| ";
    os.width(46); os << std::left << (*it)->first.GetMsg().substr(0,44);
    os.width(14); os << std::left << (*it)->second.GetCounter();
    os.width(50); os << std::left << std::scientific << (*it)->first.GetFileName().substr(0,48);
    os.width(7);  os << std::left << std::scientific << (*it)->first.GetLine();
    os << "|" << endmsg;
  }

  /// Foot
  os.precision(precision);
  os << "+"; os.repeat('-',118); os << "+" << endmsg;
}

