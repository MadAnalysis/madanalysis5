#!/usr/bin/env python

################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
#  
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#  
################################################################################


import os
import sys
import shutil

class AnalyzerManager:
    
    def __init__(self,name,title):
        self.name       = name
        self.title      = title
        self.currentdir = os.getcwd()
        
    def CheckFilePresence(self):
        if not os.path.isdir(self.currentdir + "/Analyzer"):
            print "Error: the directory called 'Analyzer' is not found."
            return False
        if os.path.isfile(self.currentdir + "/Analyzer/" + self.name + ".cpp"):
            print "Error: a file called 'Analyzer/"+self.name+\
                     ".cpp' is already defined."
            print "Please remove this file before."
            return False
        if os.path.isfile(self.currentdir + "/Analyzer/" + self.name + ".h"):
            print "Error: a file called 'Analyzer/"+self.name+\
                     ".h' is already defined."
            print "Please remove this file before."
            return False
        return True

    def AddAnalyzer(self):
        # creating the file from scratch
        if not os.path.isfile(self.currentdir + "/Analyzer/analysisList.cpp"):
            output = open(self.currentdir + "/Analyzer/analysisList.cpp","w")
            path = os.path.normpath(self.name+".h")
            output.write('#include "SampleAnalyzer/Analyzer/'+path+'"\n')
            output.write('#include "SampleAnalyzer/Analyzer/AnalyzerManager.h"\n')
            output.write('#include "SampleAnalyzer/Service/LogStream.h"\n')
            output.write('using namespace MA5;\n')
            output.write('#include <stdlib.h>\n\n')
            output.write('// -----------------------------------------------------------------------------\n')
            output.write('// BuildTable\n')
            output.write('// -----------------------------------------------------------------------------\n')
            output.write('void AnalyzerManager::BuildUserTable()\n')
            output.write('{\n')
            output.write('  Add("'+self.title+'",new '+self.name+");\n")
            output.write('}\n')
            output.close()
            

        # updating the file 
        else:
            shutil.copy(self.currentdir + "/Analyzer/analysisList.cpp",
                        self.currentdir + "/Analyzer/analysisList.bak")
            output = open(self.currentdir + "/Analyzer/analysisList.cpp","w")
            input  = open(self.currentdir + "/Analyzer/analysisList.bak")

            path = os.path.normpath(self.name+".h")
            output.write('#include "SampleAnalyzer/Analyzer/'+path+'"\n')

            for line in input:

                theline = line.lstrip()
                theline = theline.split()
                for word in theline:
                    if word=="}":
                        output.write('  Add("'+self.title+'",new '+self.name+');\n')
                        continue
                
                output.write(line)

            input.close()
            output.close()

    def WriteHeader(self):
        
        file = open(self.currentdir + "/Analyzer/" + self.name + ".h","w")
        file.write('#ifndef analysis_'+self.name+'_h\n')
        file.write('#define analysis_'+self.name+'_h\n\n')
        file.write('#include "SampleAnalyzer/Analyzer/AnalyzerBase.h"\n\n')
        file.write('namespace MA5\n')
        file.write('{\n')
        file.write('class '+self.name+' : public AnalyzerBase\n')
        file.write('{\n')
        file.write('  INIT_ANALYSIS('+self.name+',"'+self.title+'")\n\n')
        file.write(' public:\n')
        file.write('  virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters);\n')
        file.write('  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);\n')
        file.write('  virtual void Execute(SampleFormat& sample, const EventFormat& event);\n\n')
        file.write(' private:\n')
        file.write('};\n')
        file.write('}\n\n')
        file.write('#endif')
        file.close()
    def WriteSource(self):
        
        file = open(self.currentdir + "/Analyzer/" + self.name + ".cpp","w")
        file.write('#include "Analyzer/'+self.name+'.h"\n')
        file.write('using namespace MA5;\n')
        file.write('using namespace std;\n')
        file.write('\n')
        file.write('bool '+self.name+'::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)\n')
        file.write('{\n')
        file.write('  cout << "BEGIN Initialization" << endl;\n')
        file.write('  // initialize variables, histos\n')
        file.write('  cout << "END   Initialization" << endl;\n')
        file.write('  return true;\n')
        file.write('}\n')
        file.write('\n')
        file.write('void '+self.name+'::Execute(SampleFormat& sample, const EventFormat& event)\n')
        file.write('{\n')
        file.write('  // function applied after reading each event\n') 
        file.write('}\n')
        file.write('\n')
        file.write('void '+self.name+'::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)\n')
        file.write('{\n')
        file.write('  cout << "BEGIN Finalization" << endl;\n')
        file.write('  // saving histos\n')
        file.write('  cout << "END   Finalization" << endl;\n')
        file.write('}\n')
        file.close()



# Reading arguments
if len(sys.argv)!=2:
    print "Error: number of argument incorrect"
    print "Syntax: ./newAnalyzer.py name"
    print "with name the name of the analyzer"
    sys.exit()


print "A new class called '" + sys.argv[1] + "' will be created."
print "Please enter a title for your analyzer : "
title=raw_input("Title : ")

analyzer = AnalyzerManager(sys.argv[1],title)

# Checking presence of required files
if not analyzer.CheckFilePresence():
    sys.exit()

analyzer.WriteHeader()
analyzer.WriteSource()

# Adding analysis in analysisList.cpp
analyzer.AddAnalyzer()

print "Done !"



