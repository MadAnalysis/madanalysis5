################################################################################
#  
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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


from __future__ import absolute_import
from madanalysis.enumeration.ma5_running_type import MA5RunningType
import logging

def WriteHadronicList(file,main):
    file.write('  // definition of the multiparticle "hadronic"\n')
    for item in main.multiparticles.Get("hadronic"):
        file.write('  PHYSICS->mcConfig().AddHadronicId('+str(item)+');\n')


def WriteInvisibleList(file,main):
    file.write('  // definition of the multiparticle "invisible"\n')
    for item in main.multiparticles.Get("invisible"):
        file.write('  PHYSICS->mcConfig().AddInvisibleId('+str(item)+');\n')


def WriteJobInitialize(file,main):

    # Function header
    file.write('MAbool user::Initialize(const MA5::Configuration& cfg,\n')
    file.write('                      const std::map<std::string,std::string>& parameters)\n')
    file.write('{\n')

    # mcConfig initialization
    if main.mode!=MA5RunningType.RECO:
        file.write('  // Initializing PhysicsService for MC\n') 
        file.write('  PHYSICS->mcConfig().Reset();\n\n')
        WriteHadronicList(file,main)
        file.write('\n')
        WriteInvisibleList(file,main)
        file.write('\n')

    # recConfig initialization
    if main.mode==MA5RunningType.RECO:
        file.write('  // Initializing PhysicsService for RECO\n') 
        file.write('  PHYSICS->recConfig().Reset();\n\n')
        if main.isolation.algorithm=='cone':
            file.write('  PHYSICS->recConfig().UseDeltaRIsolation('+\
                       str(main.isolation.isolation.radius) + ');\n')
        else:
            file.write('  PHYSICS->recConfig().UseSumPTIsolation('+\
                       str(main.isolation.isolation.sumPT) + ',' +\
                       str(main.isolation.isolation.ET_PT) + ');\n')
        file.write('\n')

    # Region initiatization
    file.write('  // ===== Signal region ===== //\n')
    if main.regions.GetNames() == []:
        file.write('  Manager()->AddRegionSelection(\"myregion\");\n');
    else:
        for reg in main.regions.GetNames():
            file.write('  Manager()->AddRegionSelection(\"' + reg + '\");\n');
    file.write('\n')

    # Cut initiatization
    counter = 0
    file.write('  // ===== Selections ===== //\n')
    for item in main.selection.table:
        if item.__class__.__name__=="Cut":
            if len(item.part)==0:
                counter+=1;
                if len(item.regions)!=len(main.regions):
                    if len(item.regions)!=1:
                        file.write('  std::string RNc'+str(counter)+'[]={'+\
                            (', '.join('"'+reg+'"' for reg in item.regions))+'};\n')
                        file.write('  Manager()->AddCut(\"' + str(counter) + '_' + item.conditions.GetStringDisplay() +\
                            '\", RNc'+ str(counter)+');\n');
                    else:
                        file.write('  Manager()->AddCut(\"' + str(counter) + '_' + item.conditions.GetStringDisplay() + '\", '+\
                           '\"'+item.regions[0]+'\");\n');
                else:
                    file.write('  Manager()->AddCut(\"' + str(counter) + '_' + item.conditions.GetStringDisplay() + '\");\n');
    file.write('\n')

    # Histo initiatization
    counter = 0
    file.write('  // ===== Histograms ===== //\n')
    for item in main.selection.table:
        if item.__class__.__name__=="Histogram":
            counter+=1;
            if len(item.regions)!=len(main.regions):
                if len(item.regions)>1:
                    file.write('  std::string RNh'+str(counter)+'[]={'+\
                        (', '.join('"'+reg+'"' for reg in item.regions))+'};\n')
                    # NPID and NAPID
                    if item.observable.name in ["NPID", "NAPID"] :
                        file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +\
                            '\", RNh'+ str(counter)+');\n');
                    # Histo with LogX
                    elif item.logX:
                        file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                            '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', RNh'+ str(counter)+');\n');
                    # Normal Histo
                    else:
                        file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name +\
                            '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', RNh'+ str(counter)+');\n');
                elif len(item.regions)==1:
                    # NPID and NAPID
                    if item.observable.name in ["NPID", "NAPID"] :
                        file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +\
                            '\", \"'+item.regions[0]+'\");\n');
                    # Histo with LogX
                    elif item.logX:
                        file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                          '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', \"'+item.regions[0]+'\");\n');
                    # Normal Histo
                    else:
                        file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name + '\", '+\
                           str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', \"'+item.regions[0]+'\");\n');
                else:
                    # NPID and NAPID
                    if item.observable.name in ["NPID", "NAPID"] :
                        file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +');\n');
                    # Histo with LogX
                    elif item.logX:
                        file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                          '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
                    # Normal Histo
                    else:
                        file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name + '\", '+\
                           str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
            else:
               # NPID and NAPID
               if item.observable.name in ["NPID", "NAPID"] :
                   file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +'\");\n');
               # Histo with LogX
               elif item.logX:
                   file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                     '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
               # Normal Histo
               else:
                    file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name + '\", ' +\
                      str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
    file.write('\n')

    # End
    file.write('  // No problem during initialization\n')
    file.write('  return true;\n')
    file.write('}\n\n')
