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


# Setting global variables of MadAnalysis main
from madanalysis.core.main import Main
from string_tools import StringTools
import os
import sys
import logging

class MA5mode():
    
   def __init__(self):
      self.partonlevel = False
      self.hadronlevel = False
      self.recolevel   = False
      self.expertmode  = False
      self.forcedmode  = False
      self.mg5mode     = False
      self.scriptmode  = False
      self.debug       = False
      self.build       = False



################################################################################
# Function DecodeArguments
################################################################################
def DecodeArguments(version, date):
    
    import sys
    import os
    
    # Checking arguments
    import getopt
    try:
        optlist, arglist = getopt.getopt(sys.argv[1:], \
                                     "PHReEvhfmsbd", \
                                     ["partonlevel","hadronlevel","recolevel",\
                                      "expert","version","release","help",\
                                      "forced","script","mg5","debug","build"])
    except getopt.GetoptError, err:
        logging.error(str(err))
        Usage()
        sys.exit()

    # Creating container for arguments
    mode = MA5mode()

    # Reading aguments
    for o,a in optlist:
        if o in ["-P","--partonlevel"]:
            mode.partonlevel=True
        elif o in ["-H","--hadronlevel"]:
            mode.hadronlevel=True
        elif o in ["-R","--recolevel"]:
            mode.recolevel=True
        elif o in ["-E","-e","--expert"]:
            mode.expertmode=True
        elif o in ["-f","--forced"]:
            mode.forcedmode=True
        elif o in ["-m","--mg5"]:
            mode.mg5mode=True
        elif o in ["-s","--script"]:
            mode.scriptmode=True
        elif o in ["-v","--version","--release"]:
            logging.info("MA5 release : " + version + " [ " + date  + " ]")
            sys.exit()
        elif o in ["-d", "--debug"]:
            mode.debug = True
        elif o in ["-b","--build"]:
            mode.build = True
        elif o in ["-h","--help"]:
            Usage()
            sys.exit()

    # Checking consistency between arguments
    if mode.partonlevel and mode.hadronlevel:
       logging.error("Parton mode and hadron mode cannot be set in a same time.\n"
                     "Please choose only one of these modes.")
       sys.exit()
    elif mode.partonlevel and mode.recolevel:
       logging.error("Parton mode and reco mode cannot be set in a same time.\n"
                     "Please choose only one of these modes.")
       sys.exit()
    
    elif mode.hadronlevel and mode.recolevel:
       logging.error("Hadron mode and reco mode cannot be set in a same time.\n"
                     "Please choose only one of these modes.")
       sys.exit()

    if mode.scriptmode:
       mode.forcedmode=True

    return mode, arglist
        

################################################################################
# Function MainSession
################################################################################
def MainSession(mode,arglist,ma5dir,version,date):

    # Instantiating  MadAnalysis main class
    main = Main()
    main.archi_info.ma5dir      = ma5dir
    main.archi_info.ma5_version = version
    main.archi_info.ma5_date    = date

    # Setting argument in the main program 
    from madanalysis.enumeration.ma5_running_type import MA5RunningType
    if mode.partonlevel:
        main.mode=MA5RunningType.PARTON
    elif mode.hadronlevel:
        main.mode=MA5RunningType.HADRON
    elif mode.recolevel:
        main.mode=MA5RunningType.RECO
        from madanalysis.enumeration.normalize_type import NormalizeType
        main.normalize=NormalizeType.NONE

    # Re-initializing the list of observables (hadron-level or reco-level)
    if mode.hadronlevel or mode.recolevel:
        main.InitObservables(main.mode)

    main.forced = mode.forcedmode
    Main.forced = mode.forcedmode
    main.script = mode.scriptmode
    main.mg5    = mode.mg5mode

    # Setting batch mode for ROOT
    #sys.argv.append('-b-')

    # Displaying header
    logging.info("")
    logging.info(\
    "*************************************************************\n" + \
    "*                                                           *\n" + \
    "*        W E L C O M E  to  M A D A N A L Y S I S  5        *\n" + \
    "*                         ______  ______                    *\n" + \
    "*                 /'\_/`\/\  __ \/\  ___\                   *\n" + \
    "*                /\      \ \ \_\ \ \ \__/                   *\n" + \
    "*                \ \ \__\ \ \  __ \ \___``\                 *\n" + \
    "*                 \ \ \_/\ \ \ \/\ \/\ \_\ \                *\n" + \
    "*                  \ \_\\\ \_\ \_\ \_\ \____/                *\n" + \
    "*                   \/_/ \/_/\/_/\/_/\/___/                 *\n" + \
    "*                                                           *\n" + \
    "*   MA5 release : " + \
             "%-24s" % main.archi_info.ma5_version + "%+15s" % main.archi_info.ma5_date  + "   *\n" + \
    "*                                                           *\n" + \
    "*         Comput. Phys. Commun. 184 (2013) 222-256          *\n" + \
    "*           J. Phys. Conf. Ser. 123 (2014) 012032           *\n" + \
    "*                  arXiv:1405.3982 [hep-ph]                 *\n" + \
    "*                                                           *\n" + \
    "*   The MadAnalysis Development Team - Please visit us at   *\n" + \
    "*            https://launchpad.net/madanalysis5             *\n" + \
    "*                                                           *\n" + \
    "*              Type 'help' for in-line help.                *\n" + \
    "*                                                           *\n" + \
    "*************************************************************")

    # Displaying special banner if auto-check mode is activated 
    if mode.debug:
        log = logging.getLogger()
        log.setLevel(logging.DEBUG)
        logging.debug("")
        logging.debug("DEBUG MODE ACTIVATED")
        logging.debug("")

    # Checking the present linux configuration
    if not main.CheckLinuxConfig(debug=mode.debug):
        sys.exit()

    # Building (if necesserary) the SampleAnalyzer library
    if not main.BuildLibrary(forced=mode.build):
        sys.exit()	

    logging.info("*************************************************************")


    # Expert mode
    if mode.expertmode:
        from madanalysis.core.expert_mode import ExpertMode
        main.expertmode = True
        expert = ExpertMode(main)
        if not expert.CreateDirectory():
            sys.exit()
        if not expert.Copy():
            sys.exit()
        expert.GiveAdvice()
        return False

    # Normal mode
    else:
    
        # Launching the interpreter
        from madanalysis.interpreter.interpreter import Interpreter
        interpreter = Interpreter(main)

        # Looking for script
        for arg in arglist:
            filename=os.path.expanduser(arg)
            filename=os.path.abspath(filename)
            interpreter.load(filename)
    
        # Exit if script mode activated
        if len(arglist)!=0 and main.script:
            interpreter.run_cmd("quit")
            return False
    
        # Interpreter loop
        else:
            interpreter.cmdloop()
            if main.repeatSession==True:
                return True
            else:
                return False
        
       
################################################################################
# Function usage
################################################################################
def Usage():
    logging.info("\nUsage of MadAnalysis 5")
    logging.info("------------------------")
    logging.info("Syntax : ./bin/ma5 [options] [scripts]\n")
    logging.info("[options]")
    logging.info("This optional argument allows to select the running mode of " +\
                 "MadAnalysis 5 appropriate to the type of event files to analyze. " +\
                 "If absent, the parton-level mode is selected. Warning: the " +\
                 "different modes are self-excluding each other and only one " +\
                 "choice has to be made.")
    logging.info("List of available options :")
    logging.info(" -P or --partonlevel : parton-level mode")
    logging.info(" -H or --hadronlevel : hadron-level mode")
    logging.info(" -R or --recolevel   : detector-level mode")
    logging.info(" -e or -E or --expert : entering expert mode")
    logging.info(" -v or --version")
    logging.info("    or --release     : display the version number of MadAnalysis")
    logging.info(" -b or --build       : rebuild the SampleAnalyzer static library")
    logging.info(" -f or --forced      : do not ask for confirmation when MA5 removes a directory or overwrites an object") 
    logging.info(" -s or --script      : quit automatically MA5 when the script is loaded")
    logging.info(" -m or --mg5         : run MadAnalysis with options related to MadGraph")
    logging.info(" -h or --help        : dump this help")
    logging.info(" -d or --debug       : debug mode\n")
    logging.info("[scripts]")
    logging.info("This optional argument is a list of filenames containing a "+\
                 "set of MadAnalysis 5 commands. The file name are handled as "+\
                 "concatenated, and the commands are applied sequentially.\n")



################################################################################
# Function PrimarySession
################################################################################

def LaunchMA5(version, date, ma5dir):
    
    # Configuring the logger
    import colored_log
    colored_log.init()
    log = logging.getLogger()
    log.setLevel(logging.INFO)

    # Setting global variables of MadAnalysis main
    from madanalysis.core.main import Main

    # Configurating tab completion
    try: 
        import readline
    except ImportError:
        try:
            import pyreadline as readline
        except:
            print "For tab completion and history, install module readline."
    else:
        import rlcompleter
    
        if 'r261:67515' in sys.version and  'GCC 4.2.1 (Apple Inc. build 5646)' in sys.version:
            readline.parse_and_bind("bind ^I rl_complete")
            readline.__doc__ = 'libedit'  
    
        elif hasattr(readline, '__doc__'):
            if 'libedit' not in readline.__doc__:
                readline.parse_and_bind("tab: complete")
            else:
                readline.parse_and_bind("bind ^I rl_complete")
        else:
            readline.__doc__ = 'GNU'
            readline.parse_and_bind("tab: complete")

    # Read arguments
    mode,arglist = DecodeArguments(version, date)


    # Loop over MA5 sessions
    # Goal: allowing to restart
    while True:

        # Launch the interpreter
        repeat = MainSession(mode,arglist,ma5dir,version,date)

        # Normal end
        if not repeat:
            break

        # Restart
        logging.info("")
        logging.info(StringTools.Fill('-',40))
        logging.info(StringTools.Center('RESTART THE MADANALYSIS 5 SESSION',40))
        logging.info(StringTools.Fill('-',40))
        logging.info("")
    
