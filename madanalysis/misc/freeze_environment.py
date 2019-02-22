#!/usr/bin/env python

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


import os
class Architecture(Exception):
    pass

def freeze_environment(func):

    def newf(self, *args, **opts):
        # resetting the environement
        old_environ = dict(os.environ)
        os.environ.clear()
        os.environ.update(self.ma5_environ)

        # the function
        out = func(self, *args, **opts)

        # restoring the environment and sving the architecture
        self.ma5_environ.update(os.environ)
        if not self.main.archi_info.save(self.main.archi_info.ma5dir+'/tools/architecture.ma5'):
            raise Architecture('Cannot save the architecture')
        os.environ.clear()
        os.environ.update(old_environ)

        # output
        return out

    return newf
