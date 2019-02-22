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


from madanalysis.install.install_manager      import InstallManager
class DetectorManager():

    def __init__(self, main):
        self.main   = main
        self.forced = self.main.forced

    def manage(self, detector):
        # initialization
        # Getting the 'already installed' flags
        import logging
        if detector == 'delphes':
            installed     = self.main.archi_info.has_delphes
            uninstalled   = self.main.archi_info.has_delphesMA5tune
            otherdetector = 'delphesMA5tune'
        elif detector == 'delphesMA5tune':
            installed     = self.main.archi_info.has_delphesMA5tune
            uninstalled   = self.main.archi_info.has_delphes
            otherdetector = 'delphes'
        else:
            return True
        # Installing / activating if necessary
        self.main.forced=True
        if self.main.fastsim.package == detector and not installed:
            installer=InstallManager(self.main)
            if uninstalled:
                if not installer.Deactivate(otherdetector):
                    self.main.forced=self.forced
                    return False
            if installer.Activate(detector)==-1:
                self.main.forced=self.forced
                return False
        self.main.forced=self.forced
        return True

