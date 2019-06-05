################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
    # Preprocessor commands
    file.write('#ifndef analysis_user_h\n')
    file.write('#define analysis_user_h\n')
    file.write('\n')

    # Including headers files
    file.write('#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"\n')
    if main.archi_info.has_root:
        file.write('#include "SampleAnalyzer/Interfaces/root/RootMainHeaders.h"\n')
    file.write('\n')
    if main.superfastsim.tagger.rules!={}:
        file.write('#include "new_tagger.h"\n')
    if main.superfastsim.smearer.rules!={} or main.superfastsim.reco.rules!={}:
        file.write('#include "new_smearer_reco.h"\n')

    # Namespace
    file.write('namespace MA5\n')
    file.write('{\n')

    # Class
    file.write('class user : public AnalyzerBase\n{\n')
    file.write('  INIT_ANALYSIS(user,"MadAnalysis5job")\n\n')
    file.write(' public : \n')
    file.write('  virtual bool Initialize(const MA5::Configuration& cfg,\n')
    file.write('                          const std::map<std::string,std::string>& parameters);\n')
    file.write('  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);\n')
    file.write('  virtual bool Execute(SampleFormat& sample, const EventFormat& event);\n')
    file.write('\n private : \n')


def WriteCore(file,main,part_list):
    # Write particle function
    file.write('  // Declaring particle containers\n')
    for ind in range(len(part_list)):
        WriteParticle(file,\
                      part    = part_list[ind][0],\
                      rank    = part_list[ind][1],\
                      status  = part_list[ind][2],\
                      regions = part_list[ind][3],\
                      level   = main.mode)


def WriteParticle(file,part,rank,status,regions,level):
    # Skipping if already defined
    if InstanceName.Find('P_'+part.name+rank+status+'_REG_'+'_'.join(regions)):
        return

    # Getting new name
    newname=InstanceName.Get('P_'+part.name+rank+status+'_REG_'+'_'.join(regions))

    if level in [MA5RunningType.PARTON,MA5RunningType.HADRON]:

        # Creating new container
        file.write("  std::vector<const MCParticleFormat*> " +\
                   newname + ";\n")

        # Creating function for filling container
        if part.PTrank==0:
            WriteParticle2(file,part,rank,status)

    else:

        # Creating new container
        file.write("  std::vector<const RecParticleFormat*> " +\
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
    file.write('  bool isP_'+newname+\
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
            file.write('     if (part->mothers().size()==0) return false;\n')
            file.write('     bool mumOK=true;\n')
            file.write('     for (MAuint32 mum=0;mum<part->mothers().size();mum++)\n')
            file.write('     { \n')
            file.write('       mumOK &= !isP_'+ mumname + '(part->mothers()[mum]);\n')
            file.write('     }\n')
            file.write('     if (mumOK) return false;\n')
        elif part.mumType=="<<":
            file.write('     const MCParticleFormat* cand = part;\n')
            file.write('     bool success=false;\n')
            file.write('     while(cand->mothers().size()!=0)\n')
            file.write('     {\n')
            file.write('       if ( isP_' + mumname +\
                       '(cand->mothers()[0]) ) {success=true;break;}\n')
            file.write('       cand = cand->mothers()[0];\n')
            file.write('     }\n')
            file.write('     if (!success) return false;\n')

    # PT rank

    # return
    file.write('     return true; }\n')


def WriteFoot(file,main):
    file.write('};\n}\n\n#endif')
