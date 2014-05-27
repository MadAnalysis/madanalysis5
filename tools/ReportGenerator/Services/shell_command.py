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


# Standard modules
import logging
import shutil
import os
import commands
import subprocess


class ShellCommand():

    @staticmethod
    def ExecuteWithLog(theCommands,logfile,path,silent=False):

        # Open the log file
        try:
            output = open(logfile,'w')
        except:
            if not silent:
                logging.error('impossible to write the file '+logfile)
            return False, None

        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
        except:
            if not silent:
                logging.error('impossible to execute the commands: '+' '.join(theCommands))
            return False, None

        # Getting stdout
        out, err = result.communicate()
        if out!=None:
            for line in out:
               output.write(line)

        # Close the log file
        output.close()
            
        # Return results
        return (result.returncode==0), out
    

    @staticmethod
    def Execute(theCommands,path):

        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, cwd=path)
        except:
            logging.error('impossible to execute the commands: '+' '.join(theCommands))
            return False

        # Getting stdout
        out, err = result.communicate()
            
        # Return results
        return (result.returncode==0)

    @staticmethod
    def ExecuteWithCapture(theCommands,path):

        # Launching the commands
        try:
            result=subprocess.Popen(theCommands, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
        except:
            logging.error('impossible to execute the commands: '+' '.join(theCommands))
            return False, '', ''

        # Getting stdout
        out, err = result.communicate()
            
        # Return results
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
                logging.error('impossible to execute the commands: '+' '.join(theCommands))
            return []

        # Getting stdout
        out, err = result.communicate()
        if out==None:
            return []

        # Getting results
        if result.returncode!=0:
            if not mute:
                logging.error('command '+str(theCommand)+' is not found')
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
    
