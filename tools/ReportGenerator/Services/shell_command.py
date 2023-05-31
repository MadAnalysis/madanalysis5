################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


# Standard modules
from __future__ import absolute_import
import logging
import shutil
import os
import subprocess
import sys


class ShellCommand():

    @staticmethod
    def ExecuteWithLog(theCommands,logfile,path,silent=False):

        # Open the log file
        try:
            output = open(logfile,'w')
        except:
            if not silent:
                logging.getLogger('MA5').error('impossible to write the file '+logfile)
            return False, None

        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
        except:
            if not silent:
                logging.getLogger('MA5').error('impossible to execute the commands: '+' '.join(theCommands))
            return False, None

        # Getting stdout
        out, err = result.communicate()
        if sys.version_info[0]==3:
            out = out.decode()
        if out!=None:
            for line in out:
               output.write(line)

        # Close the log file
        output.close()
            
        # Return results
        return (result.returncode==0), out
    

    @staticmethod
    def Execute(theCommands,path, **kwargs):

        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, cwd=path, **kwargs)
        except Exception as err:
            logging.getLogger('MA5').error('impossible to execute the commands: '+' '.join(theCommands))
            logging.getLogger('MA5').debug(str(err))
            return False

        # Getting stdout
        out, err = result.communicate()
            
        # Return results
        return (result.returncode==0)


    @staticmethod
    def ExecuteWithMA5Logging(theCommands,path,silent=False):
        logging.getLogger('MA5')
        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
        except Exception as err:
            if not silent:
                logging.getLogger('MA5').error('impossible to execute the commands: '+' '.join(theCommands))
            logging.getLogger('MA5').debug(str(err))
            return False, None

        while True:
            my_out = result.stdout.readline()
            if sys.version_info[0]==3:
                my_out = my_out.decode()
            if my_out == '' and result.poll() is not None:
                break
            if my_out and not 'progress' in my_out:
                logging.getLogger('MA5').info('    '+my_out.strip())
        result.poll()

        # Return results
        return (result.returncode==0)

    @staticmethod
    def ExecuteWithCapture(theCommands,path,stdin=False):

        # stdin?
        if not stdin:
            stdin_value=None
        else:
            input = open(os.devnull)
            stdin_value= input
        
        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path, stdin=stdin_value)
        except:
            logging.getLogger('MA5').error('impossible to execute the commands: '+' '.join(theCommands))
            if stdin:
                input.close()
            return False, '', ''

        # Getting stdout
        out, err = result.communicate()

        # Return results
        if stdin:
            input.close()
        if sys.version_info[0]==3:
            out = out.decode()
            err = err.decode() if err else err
        return (result.returncode==0), out, err



    @staticmethod
    def Which(theCommand,all=False,mute=False):

        # theCommands
        if all:
            theCommands = ['which','-a',theCommand]
        else:
            theCommands = ['which',theCommand]

        # Launching the commands
        try:
            result=subprocess.Popen(theCommands,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        except:
            if not mute:
                logging.getLogger('MA5').error('impossible to execute the commands: '+' '.join(theCommands))
            return []

        # Getting stdout
        out, err = result.communicate()
        if sys.version_info[0]==3:
            out = out.decode()
        if out==None:
            return []

        # Getting results
        if result.returncode!=0:
            if not mute:
                logging.getLogger('MA5').error('command '+str(theCommand)+' is not found')
            return []
            
        # Splitting the lines
        msg = out.split('\n')

        # Removing irrelevant component
        msg2 = []
        for item in msg:
            if item=='':
                continue
            if len(msg2)!=0:
                if msg2[-1]==item:
                    continue
            msg2.append(item)
            
        # Return results
        return msg2
    
