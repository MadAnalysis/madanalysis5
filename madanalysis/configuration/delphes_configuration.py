################################################################################
#  
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


"""Configuration of the Delphes fast-simulation package (``set main.fastsim.*``)."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
from madanalysis.enumeration.ma5_running_type         import MA5RunningType
import os
import logging

class DelphesConfiguration:
    """Settings of the Delphes detector simulation run inside SampleAnalyzer.

    Attributes:
        detector (``str``): detector card family (``cms``, ``atlas``, ``cms-ma5tune``, ``atlas-ma5tune``).
        output (``bool``): whether the Delphes ROOT file is written.
        pileup (``str``): path to a ``.pileup`` file (empty = no pile-up).
        card (``str``): name of the Delphes card selected by :meth:`SetCard` (``delphes_<detector>[_pileup].tcl`` or ``ma5_<detector>[_pileup].tcl``).
        rootfile (``str``): user-defined name of the output ROOT file (empty = default).
        skim_genparticles / skim_tracks / skim_towers / skim_eflow (``bool``): whether the
            corresponding Delphes collections are dropped from the output.
    """

    userVariables = { "detector" : ["cms","atlas","cms-ma5tune","atlas-ma5tune"],\
                      "output": ["true","false"],\
                      "pileup": ["none"],\
                      "skim_genparticles": ["true","false"],\
                      "skim_tracks": ["true","false"],\
                      "skim_towers": ["true","false"],\
                      "skim_eflow" : ["true","false"],\
                      "rootfile" : ["none"] }

    def __init__(self) -> None:
        """Initialise a CMS-like detector without pile-up."""
        self.detector          = "cms"
        self.output            = True
        self.pileup            = ""
        self.card              = ""
        self.skim_genparticles = False
        self.skim_tracks       = False
        self.skim_towers       = False
        self.skim_eflow        = False
        self.rootfile          = ""
        self.SetCard()

    def SetCard(self) -> None:
        """Select the Delphes card from the detector and the presence of pile-up."""
        if self.detector=='cms' and self.pileup=="":
            self.card = "delphes_cms.tcl"
        elif self.detector=='cms' and self.pileup!="":
            self.card = "delphes_cms_pileup.tcl"
        elif self.detector=='atlas' and self.pileup=="":
            self.card = "delphes_atlas.tcl"
        elif self.detector=='atlas' and self.pileup!="":
            self.card = "delphes_atlas_pileup.tcl"
        elif self.detector=='cms-ma5tune' and self.pileup=="":
            self.card = "ma5_cms.tcl"
        elif self.detector=='cms-ma5tune' and self.pileup!="":
            self.card = "ma5_cms_pileup.tcl"
        elif self.detector=='atlas-ma5tune' and self.pileup=="":
            self.card = "ma5_atlas.tcl"
        elif self.detector=='atlas-ma5tune' and self.pileup!="":
            self.card = "ma5_atlas_pileup.tcl"
        
    def Display(self) -> None:
        """Log all parameters."""
        self.user_DisplayParameter("detector")
        self.user_DisplayParameter("output")
        self.user_DisplayParameter("rootfile")
        self.user_DisplayParameter("pileup")
        self.user_DisplayParameter("skim_genparticles")
        self.user_DisplayParameter("skim_tracks")
        self.user_DisplayParameter("skim_towers")
        self.user_DisplayParameter("skim_eflow")


    def user_DisplayParameter(self,parameter: str) -> None:
        """Log the value of one parameter.

        Args:
            parameter (``str``): name of the parameter.
        """
        if parameter=="detector":
            logging.getLogger('MA5').info(" detector : "+self.detector)
            return
        elif parameter=="output":
            if self.output:
                msg="true"
            else:
                msg="false"
            logging.getLogger('MA5').info(" ROOT output: "+msg)
            return
        elif parameter=="rootfile":
            if self.rootfile not in ['', "none"]:
                # FIXME: 'msg' is undefined here (UnboundLocalError); self.rootfile was probably intended.
                logging.getLogger('MA5').info(" ROOT outputfile: "+msg)
            return
        elif parameter=="pileup":
            if self.pileup=="":
                msg="none"
            else:
                msg='"'+self.pileup+'"'
            logging.getLogger('MA5').info(" pile-up source = "+msg)
            return
        elif parameter=="skim_genparticles":
            if self.skim_genparticles:
                msg="true"
            else:
                msg="false"
            logging.getLogger('MA5').info(" Skimming on GenParticles collection : "+msg)
            return
        elif parameter=="skim_tracks":
            if self.skim_tracks:
                msg="true"
            else:
                msg="false"
            logging.getLogger('MA5').info(" Skimming on Tracks collection : "+msg)
            return
        elif parameter=="skim_towers":
            if self.skim_towers:
                msg="true"
            else:
                msg="false"
            logging.getLogger('MA5').info(" Skimming on Towers collection : "+msg)
            return
        elif parameter=="skim_eflow":
            if self.skim_eflow:
                msg="true"
            else:
                msg="false"
            logging.getLogger('MA5').info(" Skimming on EFlow collection : "+msg)
            return
        else:
            logging.getLogger('MA5').error("parameter '"+parameter+"' is unknown")
            return

    def SampleAnalyzerConfigString(self) -> dict[str, str]:
            """Get the options passed to the SampleAnalyzer detector interface.

            Returns:
                ``dict[str, str]``:
                ``output`` (``'0'``/``'1'``) and optionally ``rootfile``.
            """
            mydict = {}
            if self.output:
                mydict['output'] = '1'
            else:
                mydict['output'] = '0'
            if self.rootfile not in ['', 'none']:
                mydict['rootfile'] = self.rootfile
            return mydict

    def user_SetParameter(self,parameter: str,value: str,datasets: Any,level: int) -> bool | None:
        """Set a parameter (``set main.fastsim.<parameter> = <value>``).

        Args:
            parameter (``str``): name of the parameter (see :attr:`userVariables`).
            value (``str``): value typed by the user (``pileup`` accepts ``none`` or a
                quoted/unquoted path to an existing ``.pileup`` file).
            datasets (``DatasetCollection``): datasets of the session (unused).
            level (``int``): running mode (unused).

        Returns:
            ``bool | None``:
            ``False`` for some invalid values, ``None`` otherwise.
        """
        
        # algorithm
        if parameter=="detector":

            # FIXME: the detector name is stored with the user's case, but SetCard compares lower-case
            # names: e.g. 'CMS' is accepted while the card is not updated.
            if value.lower()=="cms":
                self.detector=value
                self.SetCard()
            elif value.lower()=="atlas":
                self.detector=value
                self.SetCard()
            elif value.lower()=="cms-ma5tune":
                self.detector=value
                self.SetCard()
            elif value.lower()=="atlas-ma5tune":
                self.detector=value
                self.SetCard()
            else:
                logging.getLogger('MA5').error("algorithm called '"+value+"' is not found.")
            return

        # output
        elif parameter=="output":

            if value.lower()=="true":
                self.output = True
            elif value.lower()=="false":
                self.output = False
            else:
                logging.getLogger('MA5').error("allowed values for output are: true false")
            return

        elif parameter=="rootfile":
            if value.lower().endswith('root'):
                self.rootfile=os.path.normpath(value)
            else:
                logging.getLogger('MA5').error("Wrong output file format (root file necessary)")
                return False
            return

        # skim_genparticles
        elif parameter=="skim_genparticles":

            if value.lower()=="true":
                self.skim_genparticles = True
            elif value.lower()=="false":
                self.skim_genparticles = False
            else:
                logging.getLogger('MA5').error("allowed values for skim_genparticles are: true false")
            return

        # skim_tracks
        elif parameter=="skim_tracks":

            if value.lower()=="true":
                self.skim_tracks = True
            elif value.lower()=="false":
                self.skim_tracks = False
            else:
                logging.getLogger('MA5').error("allowed values for skim_tracks are: true false")
            return

        # skim_towers
        elif parameter=="skim_towers":

            if value.lower()=="true":
                self.skim_towers = True
            elif value.lower()=="false":
                self.skim_towers = False
            else:
                logging.getLogger('MA5').error("allowed values for skim_towers are: true false")
            return

        # skim_eflow
        elif parameter=="skim_eflow":

            if value.lower()=="true":
                self.skim_eflow = True
            elif value.lower()=="false":
                self.skim_eflow = False
            else:
                logging.getLogger('MA5').error("allowed values for skim_eflow are: true false")
            return

        # pileup
        elif parameter=="pileup":
            quoteTag=False
            if value.startswith("'") and value.endswith("'"):
                quoteTag=True
            if value.startswith('"') and value.endswith('"'):
                quoteTag=True
            if quoteTag:
                value=value[1:-1]
            valuemin = value.lower()

            # none
            if valuemin=="none":
                self.pileup = ""
                self.SetCard()
                
            # .pileup
            elif valuemin.endswith(".pileup"):
                if not os.path.isfile(value):
                    logging.getLogger('MA5').error('File called "'+value+'" is not found')
                    return
                self.pileup = value
                self.SetCard()
                return

            # other case: error
            else:
                logging.getLogger('MA5').error("The file format for the pile-up source is not known. "+\
                                               "Only files with .pileup extension can be used.")
                return False

        else:
            logging.getLogger('MA5').error("parameter called '"+parameter+"' does not exist")
            return

        
    def user_GetParameters(self) -> list[str]:
        """Get the names of the settable parameters.

        Returns:
            ``list[str]``:
            Keys of :attr:`userVariables`.
        """
        return list(DelphesConfiguration.userVariables.keys())


    def user_GetValues(self,variable: str) -> list[str]:
        """Get suggested values of a parameter (tab completion).

        Args:
            variable (``str``): name of the parameter.

        Returns:
            ``list[str]``:
            Suggested values, or an empty list.
        """
        table = []
        try:
            table.extend(DelphesConfiguration.userVariables[variable])
        except:
            pass
        return table

