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
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


struct datatypes
{
  unsigned int bool_size;
  unsigned int char_size;
  unsigned int uchar_size;
  unsigned int short_size;
  unsigned int ushort_size;
  unsigned int int_size;
  unsigned int uint_size;
  unsigned int long_size;
  unsigned int ulong_size;
  unsigned int llong_size;
  unsigned int ullong_size;
  unsigned int float_size;
  unsigned int double_size;
  unsigned int ldouble_size;
};


void Header(std::ostream& str)
{
  str << "////////////////////////////////////////////////////////////////////////////////" << std::endl;
  str << "//" << std::endl;
  str << "//  Copyright (C) 2012-2016 Eric Conte, Benjamin Fuks" << std::endl;
  str << "//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>" << std::endl;
  str << "//" << std::endl;
  str << "//  This file is part of MadAnalysis 5." << std::endl;
  str << "//  Official website: <https://launchpad.net/madanalysis5>" << std::endl;
  str << "//" << std::endl;
  str << "//  MadAnalysis 5 is free software: you can redistribute it and/or modify" << std::endl;
  str << "//  it under the terms of the GNU General Public License as published by" << std::endl;
  str << "//  the Free Software Foundation, either version 3 of the License, or" << std::endl;
  str << "//  (at your option) any later version." << std::endl;
  str << "//" << std::endl;
  str << "//  MadAnalysis 5 is distributed in the hope that it will be useful," << std::endl;
  str << "//  but WITHOUT ANY WARRANTY; without even the implied warranty of" << std::endl;
  str << "//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the" << std::endl;
  str << "//  GNU General Public License for more details." << std::endl;
  str << "//" << std::endl;
  str << "//  You should have received a copy of the GNU General Public License" << std::endl;
  str << "//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>" << std::endl;
  str << "//" << std::endl;
  str << "////////////////////////////////////////////////////////////////////////////////" << std::endl;
  str << std::endl << std::endl;
}

void Print(const datatypes& data, std::ostream& str, bool comment)
{
  std::string header ="";
  if (comment) header="// ";

  str << header << "bool    = " << data.bool_size    << " bytes" << std::endl;
  str << header << "char    = " << data.char_size    << " bytes" << std::endl;
  str << header << "uchar   = " << data.uchar_size   << " bytes" << std::endl;
  str << header << "short   = " << data.short_size   << " bytes" << std::endl;
  str << header << "ushort  = " << data.ushort_size  << " bytes" << std::endl;
  str << header << "int     = " << data.int_size     << " bytes" << std::endl;
  str << header << "uint    = " << data.uint_size    << " bytes" << std::endl;
  str << header << "long    = " << data.long_size    << " bytes" << std::endl;
  str << header << "ulong   = " << data.ulong_size   << " bytes" << std::endl;
  str << header << "llong   = " << data.llong_size   << " bytes" << std::endl;
  str << header << "ullong  = " << data.ullong_size  << " bytes" << std::endl;
  str << header << "float   = " << data.float_size   << " bytes" << std::endl;
  str << header << "double  = " << data.double_size  << " bytes" << std::endl;
  str << header << "ldouble = " << data.ldouble_size << " bytes" << std::endl;
}

int main()
{
  std::cout << "------------------------------------" << std::endl;
  std::cout << "MA5 C++ PORTABILITY CHECK-UP - BEGIN" << std::endl;
  std::cout << "------------------------------------" << std::endl;
  std::cout << std::endl;

  datatypes data;
  data.bool_size    = sizeof(bool);
  data.char_size    = sizeof(signed   char);
  data.uchar_size   = sizeof(unsigned char);
  data.short_size   = sizeof(signed   short int);
  data.ushort_size  = sizeof(unsigned short int);
  data.int_size     = sizeof(signed   int);
  data.uint_size    = sizeof(unsigned int);
  data.long_size    = sizeof(signed   long int);
  data.ulong_size   = sizeof(unsigned long int);
  data.llong_size   = sizeof(signed   long long int);
  data.ullong_size  = sizeof(unsigned long long int);
  data.float_size   = sizeof(float);
  data.double_size  = sizeof(double);
  data.ldouble_size = sizeof(long double);

  Print(data,std::cout,false);
  std::cout << std::endl;

  bool test = true;

  // C++ integer
  std::cout << "cross-check of C++ hierarchy for int   = ";
  if (1               <= data.char_size && 
      data.char_size  <= data.short_size && 
      data.short_size <= data.int_size && 
      data.int_size   <= data.long_size && 
      data.long_size  <= data.llong_size)
  {
    std::cout << "OK" << std::endl;
  }
  else
  {
    std::cout << "NO" << std::endl;
    test=false;
  }


  // C++ float
  std::cout << "cross-check of C++ hierarchy for float = ";
  if (data.float_size  <= data.double_size && 
      data.double_size <= data.ldouble_size)
  {
    std::cout << "OK" << std::endl;
  }
  else
  {
    std::cout << "NO" << std::endl;
    test=false;
  }
  std::cout << std::endl;


  // find int8
  std::cout << "int8     = ";
  if (data.char_size==1) std::cout << "[char]" << std::endl;
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find uint8
  std::cout << "uint8    = ";
  if (data.uchar_size==1) std::cout << "[unsigned char]" << std::endl;
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find int16
  std::cout << "int16    = ";
  if (data.short_size==2) std::cout << "[short]" << std::endl;
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find uint16
  std::cout << "uint16   = ";
  if (data.ushort_size==2) std::cout << "[unsigned short]" << std::endl;
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find int32
  std::cout << "int32    = ";
  bool INT_4BYTES=true;
  if (data.int_size==4) 
  {
    std::cout << "[int]" << std::endl;
  }
  else if (data.long_size==4) 
  {
    std::cout << "[long]" << std::endl;
    INT_4BYTES=false;
  }
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find uint32
  std::cout << "uint32   = ";
  if (INT_4BYTES && data.uint_size==4) std::cout << "[unsigned int]" << std::endl;
  else if (!INT_4BYTES && data.long_size==4) std::cout << "[unsigned long]" << std::endl;
  else
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find int64
  std::cout << "int64    = ";
  bool LONG_8BYTES=true;
  if (data.long_size==8) 
  {
    std::cout << "[long]" << std::endl;
  }
  else if (data.llong_size==8) 
  {
    std::cout << "[long long]" << std::endl;
    LONG_8BYTES=false;
  }
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find uint64
  std::cout << "uint64   = ";
  if (LONG_8BYTES && data.long_size==8) std::cout << "[unsigned long]" << std::endl;
  else if (!LONG_8BYTES && data.llong_size==8) std::cout << "[unsigned long long]" << std::endl;
  else
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find float16
  std::cout << "float32  = ";
  if (data.float_size==4) std::cout << "[float]" << std::endl;
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }

  // find double32
  std::cout << "double64 = ";
  if (data.double_size==8) std::cout << "[double]" << std::endl;
  else 
  {
    std::cout << "[error]" << std::endl;
    test=false;
  }
 
  std::cout << std::endl;
  std::string ma5dir = std::getenv("MA5_BASE");
  ma5dir+="/tools/SampleAnalyzer/Commons/Base/";
  std::cout << "Writing the file called 'PortabilityTags.h' in '" << ma5dir << "' ..." << std::endl;
  std::ofstream output((ma5dir+"PortabilityTags.h").c_str());
  if (output.bad() || !output.is_open())
  {
    std::cout << "Problem to write the file!" << std::endl;
    test=false;
  }
  else
  {
    Header(output);
    output << "#ifndef PORTABILITY_CHECK" << std::endl;
    output << "#define PORTABILITY_CHECK" << std::endl;
    output << std::endl << std::endl;
    output << "// Tags produced automatically by the Portability Check-Up" << std::endl;
    if (!test) output << "// Error detected with the current architecture" << std::endl;
    Print(data,output,true);
    output << std::endl;
    output << "#define INT_4BYTES  " << INT_4BYTES  << std::endl;
    output << "#define LONG_8BYTES " << LONG_8BYTES << std::endl;
    output << std::endl << std::endl;
    output << "#endif" << std::endl;
  }
  output.close();

  std::cout << "Checking the file called 'PortabilityTags.h' in '" << ma5dir << "' ..." << std::endl;
  std::ifstream input((ma5dir+"PortabilityTags.h").c_str());
  if (input.bad() || !input.is_open())
  {
    std::cout << "Problem to write the file!" << std::endl;
    test=false;
  }
  input.close();


  std::cout << std::endl << "FINAL TEST = ";
  if (test) std::cout << "OK"; else std::cout << "NO";
  std::cout << std::endl;
  std::cout << "INT_4BYTES  = " << INT_4BYTES << std::endl;
  std::cout << "LONG_8BYTES = " << LONG_8BYTES << std::endl;

  std::cout << std::endl;
  std::cout << "------------------------------------" << std::endl;
  std::cout << "MA5 C++ PORTABILITY CHECK-UP   - END" << std::endl;
  std::cout << "------------------------------------" << std::endl;

  return 0;
}
