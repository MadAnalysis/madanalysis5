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


import logging
import sys

class ColoredFormatter(logging.Formatter):

    def __init__(self, msg):
        logging.Formatter.__init__(self, msg)

    def format(self,record):
        if ( record.levelno >= 50 ):   #FATAL
            color = '\x1b[31m ** ERROR: '
        elif ( record.levelno >= 40 ): #ERROR
            color = '\x1b[31m ** ERROR: '
        elif ( record.levelno >= 30 ): #WARNING
            color = '\x1b[35m ** WARNING: '
        elif ( record.levelno >= 20 ): #INFO
            color = '\x1b[0m'
        elif ( record.levelno >= 10 ): #DEBUG
            color = '\x1b[36m ** DEBUG: '
        else:                          #ANYTHING ELSE
            color = '\x1b[0m'
        record.msg = color + str( record.msg ) + '\x1b[0m'
        return logging.Formatter.format(self, record)

class ColoredFormatterMA5(logging.Formatter):

    def __init__(self, msg):
        logging.Formatter.__init__(self, msg)

    def format(self,record):
        if ( record.levelno >= 90):    #INFO for BANNER
            color = '\x1b[0mMA5: '
        elif ( record.levelno >= 40 and record.levelno < 90):   #FATAL
            color = '\x1b[31mMA5-ERROR: '
        elif ( record.levelno >= 30 ): #WARNING
            color = '\x1b[35mMA5-WARNING: '
        elif ( record.levelno >= 20 ): #INFO
            color = '\x1b[0mMA5: '
        elif ( record.levelno >= 10 ): #DEBUG
            color = '\x1b[36mMA5-DEBUG: '
        else:                          #ANYTHING ELSE
            color = '\x1b[0mMA5: '
        record.msg = color + str( record.msg ) + '\x1b[0m'
        return logging.Formatter.format(self, record)


def init(LoggerStream=sys.stdout):
    rootLogger = logging.getLogger()
    hdlr = logging.StreamHandler()
    fmt = ColoredFormatter('%(message)s')
    hdlr.setFormatter(fmt)
    rootLogger.addHandler(hdlr)

    # we need to replace all root loggers by ma5 loggers for a proper interface with madgraph5
    ma5Logger = logging.getLogger('MA5')
    for hdlr in ma5Logger.handlers:
        ma5Logger.removeHandler(hdlr)
    hdlr = logging.StreamHandler(LoggerStream)
    fmt = ColoredFormatterMA5('%(message)s')
    hdlr.setFormatter(fmt)
    ma5Logger.addHandler(hdlr)
    ma5Logger.propagate=False
