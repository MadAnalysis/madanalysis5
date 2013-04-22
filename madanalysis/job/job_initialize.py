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
    file.write('bool user::Initialize(const MA5::Configuration& cfg,\n')
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

    # Counting number of plots and cuts
    Nhistos = 0
    Ncuts   = 0
    for item in main.selection.table:
        if item.__class__.__name__=="Histogram":
            Nhistos+=1
        elif item.__class__.__name__=="Cut":
            Ncuts+=1

    # Initializing array of cuts     
    if Ncuts!=0:
        file.write('  // Initializing cut array\n')
        file.write('  cuts_.Initialize('+str(Ncuts)+');\n')

    # Initializing each item
    ihisto  = 0
    file.write('  // Initializing each selection item\n')
    for item in main.selection.table:

        # Histogram case
        if item.__class__.__name__=="Histogram":

            # Common part
            file.write('  H'+str(ihisto)+'_ = plots_.Add_')

            # NPID
            if item.observable.name=="NPID" :
                file.write('HistoFrequency<Int_t>("selection_'+str(ihisto)+'");\n')

            # NAPID    
            elif item.observable.name=="NAPID" :
                file.write('HistoFrequency<UInt_t>("selection_'+str(ihisto)+'");\n')

            # Histo with LogX
            elif item.logX:
                file.write('HistoLogX("selection_'+str(ihisto)+'",'+\
                           str(item.nbins)+','+\
                           str(item.xmin)+','+\
                           str(item.xmax)+');\n')

            # Normal histo    
            else:
                file.write('Histo("selection_'+str(ihisto)+'",'+\
                           str(item.nbins)+','+\
                           str(item.xmin)+','+\
                           str(item.xmax)+');\n')

            ihisto+=1    

    # End
    file.write('\n')
    file.write('  // No problem during initialization\n')
    file.write('  return true;\n')
    file.write('}\n\n')
