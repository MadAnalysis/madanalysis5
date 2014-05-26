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
import os
import sys


class StringTools():

        @staticmethod
        def Fill(pattern,ntimes):

            # Check ntimes
            if ntimes<=0:
                return ''

            str=''
            for i in range(ntimes):
                str+=pattern
            return str

        @staticmethod
        def Center(pattern,width):

            # Check width
            if width<=0:
                return ''

            # Size difference between pattern and line
            space = width - len(pattern)

            # Trivial case
            if space<=0:
                return pattern

            # Normal case
            else:

                space1 = int(space/2)
                space2 = space - space1
                return StringTools.Fill(' ',space1) +\
                       pattern +\
                       StringTools.Fill(' ',space2)

            
        @staticmethod
        def Left(pattern,width):

            # Check width
            if width<=0:
                return ''

            # Size difference between pattern and line
            space = width - len(pattern)

            # Trivial case
            if space<=0:
                return pattern

            # Normal case
            else:
                return pattern+StringTools.Fill(' ',space)


        @staticmethod
        def Right(pattern,width):

            # Check width
            if width<=0:
                return ''

            # Size difference between pattern and line
            space = width - len(pattern)

            # Trivial case
            if space<=0:
                return pattern

            # Normal case
            else:
                return StringTools.Fill(' ',space)+pattern
