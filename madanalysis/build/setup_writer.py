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


import logging
from string_tools import StringTools


class SetupWriter():


    @staticmethod
    def OrderPath(paths1,middle,paths2,ma5dir):
        all=[]
        allsh=[]
        allcsh=[]
        for item in paths1:
            path=item.replace(ma5dir,'$MA5_BASE')
            all.append(path)
            allsh.append(path)
            allcsh.append(path)
        allsh.append(middle)
        allcsh.append('"'+middle+'"')
        for item in paths2:
            path=item.replace(ma5dir,'$MA5_BASE')
            all.append(path)
            allsh.append(path)
            allcsh.append(path)
        return all, allsh, allcsh

        
    @staticmethod
    def WriteSetupFile(bash,path,archi_info):

        # Variable to check at the end
        toCheck=[]

        # Opening file in write-only mode
        import os
        if bash:
            filename = os.path.normpath(path+"/setup.sh")
        else:
            filename = os.path.normpath(path+"/setup.csh")
        try:
            file = open(filename,"w")
        except:
            logging.error('Impossible to create the file "' + filename +'"')
            return False

        # Calling the good shell
        if bash:
            file.write('#!/bin/sh\n')
        else:
            file.write('#!/bin/csh -f\n')
        file.write('\n')

        # Defining colours
        file.write('# Defining colours for shell\n')
        if bash:
            file.write('GREEN="\\\\033[1;32m"\n')
            file.write('RED="\\\\033[1;31m"\n')
            file.write('PINK="\\\\033[1;35m"\n')
            file.write('BLUE="\\\\033[1;34m"\n')
            file.write('YELLOW="\\\\033[1;33m"\n')
            file.write('CYAN="\\\\033[1;36m"\n')
            file.write('NORMAL="\\\\033[0;39m"\n')
            # using ' ' could be more convenient to code
            # but in this case, the colour code are interpreted
            # by the linux command 'more'
        else:
            file.write('set GREEN  = "\\033[1;32m"\n')
            file.write('set RED    = "\\033[1;31m"\n')
            file.write('set PINK   = "\\033[1;35m"\n')
            file.write('set BLUE   = "\\033[1;34m"\n')
            file.write('set YELLOW = "\\033[1;33m"\n')
            file.write('set CYAN   = "\\033[1;36m"\n')
            file.write('set NORMAL = "\\033[0;39m"\n')
        file.write('\n')

        # Treating ma5dir
        ma5dir=archi_info.ma5dir
        if ma5dir.endswith('/'):
            ma5dir=ma5dir[:-1]

        # Configuring PATH environment variable
        file.write('# Configuring MA5 environment variable\n')
        if bash:
            file.write('export MA5_BASE=' + (ma5dir)+'\n')
        else:
            file.write('setenv MA5_BASE ' + (ma5dir)+'\n')
        toCheck.append('MA5_BASE')
        file.write('\n')

        # Treating PATH
        toPATH, toPATHsh, toPATHcsh = SetupWriter.OrderPath(archi_info.toPATH1,'$PATH',archi_info.toPATH2,ma5dir)
        toLDPATH, toLDPATHsh, toLDPATHcsh = SetupWriter.OrderPath(archi_info.toLDPATH1,'$LD_LIBRARY_PATH',archi_info.toLDPATH2,ma5dir)
        toDYLDPATH, toDYLDPATHsh, toDYLDPATHcsh = SetupWriter.OrderPath(archi_info.toLDPATH1,'$DYLD_LIBRARY_PATH',archi_info.toLDPATH2,ma5dir)
                
        # Configuring PATH environment variable
        if len(toPATH)!=0:
            file.write('# Configuring PATH environment variable\n')
            if bash:
                file.write('if [ $PATH ]; then\n')
                file.write('export PATH='+(':'.join(toPATHsh))+'\n')
                file.write('else\n')
                file.write('export PATH='+(':'.join(toPATH))+'\n')
                file.write('fi\n')
            else:
                file.write('if ( $?PATH ) then\n')
                file.write('setenv PATH '+(':'.join(toPATHcsh))+'\n')
                file.write('else\n')
                file.write('setenv PATH '+(':'.join(toPATH))+'\n')
                file.write('endif\n')
            toCheck.append('PATH')
            file.write('\n')

        if len(toLDPATH)!=0:
            
            # Configuring LD_LIBRARY_PATH environment variable
            file.write('# Configuring LD_LIBRARY_PATH environment variable\n')
            if bash:
                file.write('if [ $LD_LIBRARY_PATH ]; then\n')
                file.write('export LD_LIBRARY_PATH='+(':'.join(toLDPATHsh))+'\n')
                file.write('else\n')
                file.write('export LD_LIBRARY_PATH='+(':'.join(toLDPATH))+'\n')
                file.write('fi\n')
            else:
                file.write('if ( $?LD_LIBRARY_PATH ) then\n')
                file.write('setenv LD_LIBRARY_PATH '+(':'.join(toLDPATHcsh))+'\n')
                file.write('else\n')
                file.write('setenv LD_LIBRARY_PATH '+(':'.join(toLDPATH))+'\n')
                file.write('endif\n')
            toCheck.append('LD_LIBRARY_PATH')
            file.write('\n')

            # Configuring LIBRARY_PATH environment variable
            #file.write('# Configuring LIBRARY_PATH environment variable\n')
            #if bash:
            #    file.write('export LIBRARY_PATH=' + (os.environ['LD_LIBRARY_PATH'])+'\n')
            #else:
            #    file.write('setenv LIBRARY_PATH ' + (os.environ['LD_LIBRARY_PATH'])+'\n')
            #file.write('\n')

            # Configuring DYLD_LIBRARY_PATH environment variable
            if archi_info.isMac:
                file.write('# Configuring DYLD_LIBRARY_PATH environment variable\n')
                if bash:
                    file.write('if [ $DYLD_LIBRARY_PATH ]; then\n')
                    file.write('export DYLD_LIBRARY_PATH='+ (':'.join(toDYLDPATHsh))+'\n')
                    file.write('else\n')
                    file.write('export DYLD_LIBRARY_PATH='+ (':'.join(toLDPATH))+'\n')
                    file.write('fi\n')
                else:
                    file.write('if ( $?DYLD_LIBRARY_PATH ) then\n')
                    file.write('setenv DYLD_LIBRARY_PATH '+(':'.join(toDYLDPATHcsh))+'\n')
                    file.write('else\n')
                    file.write('setenv DYLD_LIBRARY_PATH '+(':'.join(toLDPATH))+'\n')
                    file.write('endif\n')
                toCheck.append('DYLD_LIBRARY_PATH')
                file.write('\n')

            # Configuring CPLUS_INCLUDE_PATH environment variable
            #file.write('# Configuring CPLUS_INCLUDE_PATH environment variable\n')
            #if bash:
            #    file.write('export CPLUS_INCLUDE_PATH=' + (os.environ['CPLUS_INCLUDE_PATH'])+'\n')
            #else:
            #    file.write('setenv CPLUS_INCLUDE_PATH ' + (os.environ['CPLUS_INCLUDE_PATH'])+'\n')
            #file.write('\n')

        # Checking that all environment variables are defined
        file.write('# Checking that all environment variables are defined\n')
        if bash:
            file.write('if [[ ')
            for ind in range(0,len(toCheck)):
                if ind!=0:
                    file.write(' && ')
                file.write('$'+toCheck[ind])
            file.write(' ]]; then\n')
            file.write('echo -e $YELLOW"'+StringTools.Fill('-',56)+'"\n')
	    file.write('echo -e "'+StringTools.Center('Your environment is properly configured for MA5',56)+'"\n')
	    file.write('echo -e "'+StringTools.Fill('-',56)+'"$NORMAL\n')
            file.write('fi\n')
        else:
            file.write('if ( ')
            for ind in range(0,len(toCheck)):
                if ind!=0:
                    file.write(' && ')
                file.write('$?'+toCheck[ind])
            file.write(' ) then\n')
            file.write('echo $YELLOW"'+StringTools.Fill('-',56)+'"\n')
	    file.write('echo "'+StringTools.Center('Your environment is properly configured for MA5',56)+'"\n')
	    file.write('echo "'+StringTools.Fill('-',56)+'"$NORMAL\n')
            file.write('endif\n')

        # Closing the file
        try:
            file.close()
        except:
            logging.error('Impossible to close the file "'+filename+'"')
            return False

        return True


    @staticmethod
    def WriteSetupFileForJob(bash,path,archi_info):

        # Variable to check at the end
        toCheck=[]

        # Opening file in write-only mode
        import os
        if bash:
            filename = os.path.normpath(path+"/setup.sh")
        else:
            filename = os.path.normpath(path+"/setup.csh")
        try:
            file = open(filename,"w")
        except:
            logging.error('Impossible to create the file "' + filename +'"')
            return False

        # Calling the good shell
        if bash:
            file.write('#!/bin/sh\n')
        else:
            file.write('#!/bin/csh -f\n')
        file.write('\n')

        # Defining colours
        file.write('# Defining colours for shell\n')
        if bash:
            file.write('GREEN="\\\\033[1;32m"\n')
            file.write('RED="\\\\033[1;31m"\n')
            file.write('PINK="\\\\033[1;35m"\n')
            file.write('BLUE="\\\\033[1;34m"\n')
            file.write('YELLOW="\\\\033[1;33m"\n')
            file.write('CYAN="\\\\033[1;36m"\n')
            file.write('NORMAL="\\\\033[0;39m"\n')
            # using ' ' could be more convenient to code
            # but in this case, the colour code are interpreted
            # by the linux command 'more'
        else:
            file.write('set GREEN  = "\\033[1;32m"\n')
            file.write('set RED    = "\\033[1;31m"\n')
            file.write('set PINK   = "\\033[1;35m"\n')
            file.write('set BLUE   = "\\033[1;34m"\n')
            file.write('set YELLOW = "\\033[1;33m"\n')
            file.write('set CYAN   = "\\033[1;36m"\n')
            file.write('set NORMAL = "\\033[0;39m"\n')
        file.write('\n')

        # Treating ma5dir
        ma5dir=archi_info.ma5dir
        if ma5dir.endswith('/'):
            ma5dir=ma5dir[:-1]

        # Configuring PATH environment variable
        file.write('# Configuring MA5 environment variable\n')
        if bash:
            file.write('export MA5_BASE=' + (ma5dir)+'\n')
        else:
            file.write('setenv MA5_BASE ' + (ma5dir)+'\n')
        toCheck.append('MA5_BASE')
        file.write('\n')

        # Launching MadAnalysis with empty script
        file.write('# Launching MA5 to check if the libraries need to be rebuild\n')
        file.write('$MA5_BASE/bin/ma5 --script $MA5_BASE/madanalysis/input/init.ma5\n')
        file.write('\n')

        # Loading the SampleAnalyzer setup files
        file.write('# Loading the setup files\n')
        if bash:
            file.write('source $MA5_BASE/tools/SampleAnalyzer/setup.sh\n')
        else:
            file.write('source $MA5_BASE/tools/SampleAnalyzer/setup.csh\n')

        # Closing the file
        try:
            file.close()
        except:
            logging.error('Impossible to close the file "'+filename+'"')
            return False

        return True


