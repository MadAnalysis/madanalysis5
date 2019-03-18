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


# Setting global variables of MadAnalysis main
from madanalysis.core.script_stack import ScriptStack
from madanalysis.core.main         import Main
from string_tools                  import StringTools

# Python import
import os
import sys
import logging


class MA5mode():
    
   def __init__(self):
      self.partonlevel    = False
      self.hadronlevel    = False
      self.recolevel      = False
      self.expertmode     = False
      self.forcedmode     = False
      self.scriptmode     = False
      self.debug          = False
      self.build          = False
      self.developer_mode = False



################################################################################
# Function DefaultInstallCard
################################################################################
def DefaultInstallCard():
    logging.getLogger('MA5').info("Generate a default installation_options.dat file...")
    output = file('installation_options.dat','w')
    output.write('# WARNING! MA5 SHOULD DETECT AUTOMATICALLY YOUR CONFIGURATION\n')
    output.write('# IF THIS AUTOMATED MODE FAILS, YOU CAN FORCE SOME \n')
    output.write('# OPTIONS THROUGH THIS FILE\n')
    output.write('\n')
    output.write('# ----GENERAL----\n')
    output.write('# tmp_dir = /tmp/ma5/\n')
    output.write('# download_dir = /tmp/downloadma5/\n')
    output.write('# webaccess_veto = 0 # 0=No, 1=Yes\n')
    output.write('\n')
    output.write('# -----ROOT-----\n')
    output.write('# root_veto     = 0 # 0=No, 1=Yes\n')
    output.write('# root_bin_path = /home/root/bin\n')
    output.write('\n')
    output.write('# -----MATPLOTLIB-----\n')
    output.write('# matplotlib_veto = 0 # 0=No, 1=Yes\n')
    output.write('\n')
    output.write('# -----DELPHES----- \n')
    output.write('# delphes_veto     = 0 # 0=No, 1=Yes\n')
    output.write('# delphes_includes = /home/delphes/delphes/include/\n')
    output.write('# delphes_libs     = /home/delphes/delphes/lib/\n')
    output.write('\n')
    output.write('# -----DELPHESMA5TUNE-----\n')
    output.write('# delphesMA5tune_veto     = 0 # 0=No, 1=Yes\n')
    output.write('# delphesMA5tune_includes = /home/delphesMA5tune/include\n')
    output.write('# delphesMA5tune_libs     = /home/delphesMA5tune/lib\n')
    output.write('\n')
    output.write('# -----ZLIB-----\n')
    output.write('# zlib_veto     = 0 # 0=No, 1=Yes\n')
    output.write('# zlib_includes = /home/zlib/include/\n')
    output.write('# zlib_libs     = /home/zlib/lib/\n')
    output.write('\n')
    output.write('# -----FASTJET-----\n')
    output.write('# fastjet_veto     = 0 # 0=No, 1=Yes\n')
    output.write('# fastjet_bin_path = /home/fastjet/build/bin/\n')
    output.write('\n')
    output.write('# -----PAD-----\n')
    output.write('# pad_veto = 0 # 0=No, 1=Yes\n')
    output.write('# pad_build_path = /home/PAD/build/\n')
    output.write('\n')
    output.write('# -----PADForMA5Tune-----\n')
    output.write('# padma5_veto = 0 # 0=No, 1=Yes\n')
    output.write('# padma5_build_path = /home/PADForMA5tune/build/\n')
    output.write('\n')
    output.write('# -----PDFLATEX-----\n')
    output.write('# pdflatex_veto = 0 # 0=No, 1=Yes\n')
    output.write('\n')
    output.write('# -----LATEX-----\n')
    output.write('# latex_veto = 0 # 0=No, 1=Yes\n')
    output.write('\n')
    output.write('# -----SCIPY-----\n')
    output.write('# scipy_veto = 0 # 0=No, 1=Yes\n')
    output.write('\n')
    output.close()


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
                                     "PHReEvhfmsbdqi", \
                                     ["partonlevel","hadronlevel","recolevel",\
                                      "expert","version","release","help",\
                                      "forced","script","debug","build","qmode","installcard"])
    except getopt.GetoptError, err:
        logging.getLogger('MA5').error(str(err))
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
        elif o in ["-s","--script"]:
            mode.scriptmode=True
        elif o in ["-v","--version","--release"]:
            logging.getLogger('MA5').info("MA5 release : " + version + " [ " + date  + " ]")
            sys.exit()
        elif o in ["-d", "--debug"]:
            mode.debug = True
        elif o in ["-b","--build"]:
            mode.build = True
        elif o in ["-q","--qmode"]:
            mode.developer_mode = True
            print ""
            print " **** DEVELOPER MODE DETECTED **** "
            print ""
        elif o in ["-h","--help"]:
            Usage()
            sys.exit()
        elif o in ["-i","--installcard"]:
            DefaultInstallCard()
            sys.exit()
        else:
            logging.getLogger('MA5').error("Argument '"+o+"' is not found.")
            Usage()
            sys.exit()


    # Checking consistency between arguments
    if mode.partonlevel and mode.hadronlevel:
       logging.getLogger('MA5').error("Parton mode and hadron mode cannot be set in a same time.\n"
                     "Please choose only one of these modes.")
       sys.exit()
    elif mode.partonlevel and mode.recolevel:
       logging.getLogger('MA5').error("Parton mode and reco mode cannot be set in a same time.\n"
                     "Please choose only one of these modes.")
       sys.exit()

    elif mode.hadronlevel and mode.recolevel:
       logging.getLogger('MA5').error("Hadron mode and reco mode cannot be set in a same time.\n"
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

    main.forced         = mode.forcedmode
    Main.forced         = mode.forcedmode
    main.script         = mode.scriptmode
    main.developer_mode = mode.developer_mode

    # Displaying header
    logging.getLogger('MA5').info("")
    logging.getLogger('MA5').info("*************************************************************")
    logging.getLogger('MA5').info("*                                                           *")
    logging.getLogger('MA5').info("*        W E L C O M E  to  M A D A N A L Y S I S  5        *")
    logging.getLogger('MA5').info("*                         ______  ______                    *")
    logging.getLogger('MA5').info("*                 /'\_/`\/\  __ \/\  ___\                   *")
    logging.getLogger('MA5').info("*                /\      \ \ \_\ \ \ \__/                   *")
    logging.getLogger('MA5').info("*                \ \ \__\ \ \  __ \ \___``\                 *")
    logging.getLogger('MA5').info("*                 \ \ \_/\ \ \ \/\ \/\ \_\ \                *")
    logging.getLogger('MA5').info("*                  \ \_\\\ \_\ \_\ \_\ \____/                *")
    logging.getLogger('MA5').info("*                   \/_/ \/_/\/_/\/_/\/___/                 *")
    logging.getLogger('MA5').info("*                                                           *")
    logging.getLogger('MA5').info("*   MA5 release : " + "%-24s" % main.archi_info.ma5_version + "%+15s" % main.archi_info.ma5_date  + "   *")
    logging.getLogger('MA5').info("*                                                           *")
    logging.getLogger('MA5').info("*         Comput. Phys. Commun. 184 (2013) 222-256          *")
    logging.getLogger('MA5').info("*             Eur. Phys. J. C74 (2014) 3103                 *")
    logging.getLogger('MA5').info("*                                                           *")
    logging.getLogger('MA5').info("*   The MadAnalysis Development Team - Please visit us at   *")
    logging.getLogger('MA5').info("*            https://launchpad.net/madanalysis5             *")
    logging.getLogger('MA5').info("*                                                           *")
    logging.getLogger('MA5').info("*              Type 'help' for in-line help.                *")
    logging.getLogger('MA5').info("*                                                           *")
    logging.getLogger('MA5').info("*************************************************************")

    # Displaying special banner if auto-check mode is activated 
    if mode.debug:
        log = logging.getLogger()
        log.setLevel(logging.DEBUG)
        logging.getLogger('MA5').debug("")
        logging.getLogger('MA5').debug("DEBUG MODE ACTIVATED")
        logging.getLogger('MA5').debug("")

    # Checking the present configuration
    if not main.CheckConfig(debug=mode.debug):
        sys.exit()

    # Building (if necesserary) the SampleAnalyzer library
    if not main.BuildLibrary(forced=mode.build):
        sys.exit()

    # Checking the present configuration
    if not main.CheckConfig2(debug=mode.debug):
        sys.exit()

    logging.getLogger('MA5').info("*************************************************************")


    # Expert mode
    if mode.expertmode:
        from madanalysis.core.expert_mode import ExpertMode
        main.expertmode = True
        expert = ExpertMode(main)
        dirname=""
        if len(arglist)>0:
          dirname=arglist[0]
        if not expert.CreateDirectory(dirname):
            sys.exit()
        dirname=""
        if len(arglist)>1:
          dirname=arglist[1]
        if not expert.Copy(dirname):
            sys.exit()
        expert.GiveAdvice()

        return False # Exit with no repeation

    # Normal mode
    else:

        # Launching the interpreter
        from madanalysis.interpreter.interpreter import Interpreter
        interpreter = Interpreter(main)
        interpreter.InitializeHistory()

        # Executing the ma5 scripts
        if not ScriptStack.IsEmpty() and not ScriptStack.IsFinished():
            interpreter.load()

            # Must be restarted?
            if main.repeatSession==True:
                return True

            # Exit if script mode activated
            if main.script:
                interpreter.run_cmd("quit")
                return False # Exit with no repeation

        # Interpreter loop
        interpreter.cmdloop()
        interpreter.FinalizeHistory()
        if main.repeatSession==True:
            return True
        else:
            return False


################################################################################
# Function usage
################################################################################
def Usage():
    logging.getLogger('MA5').info("\nUsage of MadAnalysis 5")
    logging.getLogger('MA5').info("------------------------")
    logging.getLogger('MA5').info("Syntax : ./bin/ma5 [options] [scripts]\n")
    
    logging.getLogger('MA5').info("[options]")
    logging.getLogger('MA5').info("This optional argument allows to select the running mode of " +\
                 "MadAnalysis 5 appropriate to the type of event files to analyze. " +\
                 "If absent, the parton-level mode is selected. Warning: the " +\
                 "different modes are self-excluding each other and only one " +\
                 "choice has to be made.")
    logging.getLogger('MA5').info("List of available options :")
    logging.getLogger('MA5').info(" -P or --partonlevel : parton-level mode")
    logging.getLogger('MA5').info(" -H or --hadronlevel : hadron-level mode")
    logging.getLogger('MA5').info(" -R or --recolevel   : detector-level mode")
    logging.getLogger('MA5').info(" -e or -E or --expert : entering expert mode")
    logging.getLogger('MA5').info(" -v or --version")
    logging.getLogger('MA5').info("    or --release     : display the version number of MadAnalysis")
    logging.getLogger('MA5').info(" -b or --build       : rebuild the SampleAnalyzer static library")
    logging.getLogger('MA5').info(" -f or --forced      : do not ask for confirmation when MA5 removes a directory or overwrites an object") 
    logging.getLogger('MA5').info(" -s or --script      : quit automatically MA5 when the script is loaded")
    logging.getLogger('MA5').info(" -h or --help        : dump this help")
    logging.getLogger('MA5').info(" -i or --installcard : produce the default installation card in installation_card.dat")
    logging.getLogger('MA5').info(" -d or --debug       : debug mode")
    logging.getLogger('MA5').info(" -q or --qmode       : developper mode only for MA5 developpers\n")
    
    logging.getLogger('MA5').info("[scripts]")
    logging.getLogger('MA5').info("This optional argument is a list of filenames containing a "+\
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

    # Deal with the scripts in normal mode
    if not mode.expertmode:
        for arg in arglist:
            ScriptStack.AddScript(arg)

    # Loop over MA5 sessions
    # Goal: allowing to restart
    while True:

        # Launch the interpreter
        repeat = MainSession(mode,arglist,ma5dir,version,date)

        # Normal end
        if not repeat:
            break

        # Restart
        logging.getLogger('MA5').info("")
        logging.getLogger('MA5').info(StringTools.Fill('-',40))
        logging.getLogger('MA5').info(StringTools.Center('RESTART THE MADANALYSIS 5 SESSION',40))
        logging.getLogger('MA5').info(StringTools.Fill('-',40))
        logging.getLogger('MA5').info("")




