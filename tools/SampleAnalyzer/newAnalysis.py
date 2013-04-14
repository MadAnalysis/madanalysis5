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

class AnalysisManager:
    
    def __init__(self,name,title):
        self.name       = name
        self.title      = title
        self.currentdir = os.getcwd()
        
    def CheckFilePresence(self):
        if not os.path.isdir(self.currentdir + "/Analysis"):
            print "Error: the directory called 'Analysis' is not found."
            return False
        if os.path.isfile(self.currentdir + "/Analysis/" + self.name + ".cpp"):
            print "Error: a file called 'Analysis/"+self.name+\
                     ".cpp' is already defined."
            print "Please remove this file before."
            return False
        if os.path.isfile(self.currentdir + "/Analysis/" + self.name + ".h"):
            print "Error: a file called 'Analysis/"+self.name+\
                     ".h' is already defined."
            print "Please remove this file before."
            return False
        return True

    def AddAnalysis(self):
        # creating the file from scratch
        if not os.path.isfile(self.currentdir + "/Analysis/analysisList.cpp"):
            output = open(self.currentdir + "/Analysis/analysisList.cpp","w")
            path = os.path.normpath("Analysis/"+self.name+".h")
            output.write('#include "'+path+'"\n')
            output.write('#include "Analysis/AnalysisManager.h"\n')
            output.write('#include "Analysis/user.h"\n')
            output.write('#include "Service/LogStream.h"\n')
            output.write('// -----------------------------------------------------------------------------\n')
            output.write('// BuildTable\n')
            output.write('// -----------------------------------------------------------------------------\n')
            output.write('void AnalysisManager::BuildTable()\n')
            output.write('{\n')
            output.write('  Add("'+self.title+'",new '+self.name+");\n")
            output.write('}\n')
            output.close()
            

        # updating the file 
        else:
            shutil.copy(self.currentdir + "/Analysis/analysisList.cpp",
                        self.currentdir + "/Analysis/analysisList.bak")
            output = open(self.currentdir + "/Analysis/analysisList.cpp","w")
            input  = open(self.currentdir + "/Analysis/analysisList.bak")

            path = os.path.normpath("Analysis/"+self.name+".h")
            output.write('#include "'+path+'"\n')

            for line in input:

                theline = line.lstrip()
                theline = theline.split()
                for word in theline:
                    if word=="}":
                        output.write("  Add(new "+self.name+");\n")
                        continue
                
                output.write(line)

            input.close()
            output.close()

    def WriteHeader(self):
        
        file = open(self.currentdir + "/Analysis/" + self.name + ".h","w")
        file.write('#ifndef analysis_'+self.name+'_h\n')
        file.write('#define analysis_'+self.name+'_h\n\n')
        file.write('#include "Analysis/AnalysisBase.h"\n\n')
        file.write('class '+self.name+' : public AnalysisBase\n')
        file.write('{\n')
        file.write('  INIT_ANALYSIS('+self.name+',"'+self.title+'")\n\n')
        file.write(' public:\n')
        file.write('  virtual void Initialize();\n')
        file.write('  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);\n')
        file.write('  virtual void Execute(SampleFormat& sample, const EventFormat& event);\n\n')
        file.write(' private:\n')
        file.write('};\n\n')
        file.write('#endif')
        file.close()

    def WriteSource(self):
        
        file = open(self.currentdir + "/Analysis/" + self.name + ".cpp","w")
        file.write('#include "Analysis/'+self.name+'.h"\n\n')
        file.write('void '+self.name+'::Initialize()\n')
        file.write('{\n')
        file.write('}\n')
        file.write('void '+self.name+'::Execute(const SampleFormat& sample, const EventFormat& event)\n')
        file.write('{\n')
        file.write('}\n')
        file.write('void '+self.name+'::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)\n')
        file.write('{\n')
        file.write('}\n')
        file.close()



# Reading arguments
if len(sys.argv)!=2:
    print "Error: number of argument incorrect"
    print "Syntax: ./newAnalysis.py name"
    print "with name the name of the analysis"
    sys.exit()


print "A new class called '" + sys.argv[1] + "' will be created."
print "Please enter a title for your analysis : "
title=raw_input("Title : ")

analysis = AnalysisManager(sys.argv[1],title)

# Checking presence of required files
if not analysis.CheckFilePresence():
    sys.exit()

analysis.WriteHeader()
analysis.WriteSource()

# Adding analysis in analysisList.cpp
analysis.AddAnalysis()

print "Done !"



