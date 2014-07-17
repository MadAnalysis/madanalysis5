################################################################################
#  
#  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
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


from madanalysis.enumeration.argument_type    import ArgumentType
from madanalysis.selection.instance_name      import InstanceName
from madanalysis.enumeration.ma5_running_type import MA5RunningType
from madanalysis.observable.observable_base   import ObservableBase
import logging
import sys

def WriteHeader(file,main):
    file.write('#ifndef analysis_user_h\n#define analysis_user_h\n\n')
    file.write('#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"\n\n')
    file.write('namespace MA5\n')
    file.write('{\n')
    file.write('class user : public AnalyzerBase\n{\n')
    file.write('  INIT_ANALYSIS(user,"MadAnalysis5job")\n\n')
    file.write(' public : \n')
    file.write('  virtual bool Initialize(const MA5::Configuration& cfg,\n')
    file.write('                          const std::map<std::string,std::string>& parameters);\n')
    file.write('  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);\n')
    file.write('  virtual bool Execute(SampleFormat& sample, const EventFormat& event);\n\n')
    file.write(' private : \n')


def WriteCore(file,main,part_list):

    # Counting number of plots and cuts
    Nhistos = 0
    Ncuts   = 0
    for item in main.selection.table:
        if item.__class__.__name__=="Histogram":
            Nhistos+=1
        elif item.__class__.__name__=="Cut":
            Ncuts+=1


    # Declaring array of plots
    file.write('  // Declaring histogram array\n')
    file.write('  PlotManager plots_;\n\n')

    # Declaring array of cuts
    file.write('  // Declaring cut array\n')
    file.write('  CounterManager cuts_;\n\n')

    # Declaring short-cut for each histo
    if Nhistos!=0:
        Nhistos=0
        file.write('  // Declaring shortcut to histograms\n')
        for ind in range(0,len(main.selection.table)):

            # Histogram case
            if main.selection.table[ind].__class__.__name__=="Histogram":
                if main.selection.table[ind].observable.name=="NPID" :
                    file.write('  HistoFrequency<Int_t>* H'+str(Nhistos)+'_;\n')
                elif main.selection.table[ind].observable.name=="NAPID" :
                    file.write('  HistoFrequency<UInt_t>* H'+str(Nhistos)+'_;\n')
                elif main.selection.table[ind].logX:
                    file.write('  HistoLogX* H'+str(Nhistos)+'_;\n');
                else:
                    file.write('  Histo* H'+str(Nhistos)+'_;\n');
                Nhistos+=1

    # Write particle function
    for ind in range(len(part_list)):
        WriteParticle(file,\
                      part   = part_list[ind][0],\
                      rank   = part_list[ind][1],\
                      status = part_list[ind][2],\
                      level  = main.mode)


def WriteParticle(file,part,rank,status,level):

    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status):
        return

    # Getting new name
    newname=InstanceName.Get('P_'+part.name+rank+status)

    if level in [MA5RunningType.PARTON,MA5RunningType.HADRON]:

        # Creating new container
        file.write("   std::vector<const MCParticleFormat*> " +\
                   newname + ";\n")

        # Creating function for filling container
        if part.PTrank==0:
            WriteParticle2(file,part,rank,status)

    else:

        # Creating new container
        file.write("   std::vector<const RecParticleFormat*> " +\
                   newname + ";\n")


def WriteParticle2(file,part,rank,status):

    # Skipping if already defined
    if InstanceName.Find(part.name+rank+status):
        return

    # Getting new name
    newname=InstanceName.Get(part.name+rank+status)

    # Do mother before
    if part.mumType!="":
        WriteParticle2(file,part.mumPart,rank,'allstate')

    # Identifier function
    file.write('   bool isP_'+newname+\
               '(const MCParticleFormat* part) const {\n')

    # Null pointer
    file.write('     if ( part==0 ) return false;\n')

    # FinalSate
    if status=="finalstate":
        file.write("     if ( !PHYSICS->Id->IsFinalState(part) ) return false;\n")
    elif status=="initialstate":
        file.write("     if ( !PHYSICS->Id->IsInitialState(part) ) return false;\n")
    elif status=="interstate":
        file.write("     if ( !PHYSICS->Id->IsInterState(part) ) return false;\n")

    # Id
    file.write('     if ( ')
    variables=[]
    for item in part.particle.ids:
        variables.append('(part->pdgid()!='+str(item)+')')
    file.write('&&'.join(variables))
    file.write(' ) return false;\n')

    # Mother
    if part.mumType!="":
        mumname=InstanceName.Get(part.mumPart.name+rank+'allstate')
        if part.mumType=="<":
            file.write('     if ( !isP_' + mumname +\
                       '(part->mother1()) ) return false;\n')
        elif part.mumType=="<<":
            file.write('     const MCParticleFormat* cand = part;\n')
            file.write('     bool success=false;\n')
            file.write('     while(cand->mother1()!=0)\n')
            file.write('     {\n')
            file.write('       if ( isP_' + mumname +\
                       '(cand->mother1()) ) {success=true;break;}\n')
            file.write('       cand = cand->mother1();\n')
            file.write('     }\n')
            file.write('     if (!success) return false;\n')

    # PT rank

    # return
    file.write('     return true; }\n')


def WriteFoot(file,main):
    file.write('};\n}\n\n#endif')
