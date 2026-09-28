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


"""Legacy table of observables with their C++ implementation for each running mode.

.. note::
    The observables used to generate the analysis code are defined in
    :mod:`madanalysis.observable.observable_list`; this table is still used by the
    ``plot``/``select``/``reject`` commands for completion and validation.
"""

from __future__ import absolute_import
from __future__ import annotations
from madanalysis.enumeration.ma5_running_type import MA5RunningType
import math
import six

class metaclass(type):
        """Metaclass turning the class attribute access ``ObservableType.NAME`` into an integer code.

        Accessing ``ObservableType.NAME`` returns the index of ``NAME`` in ``ObservableType.values``; the
        conversion helpers below map such an index back to the associated properties.
        """
        def __getattr__(self, name: str) -> int:
            """Get the integer code of an enumeration entry.

            Unknown names are mapped to the index of ``UNKNOWN`` instead of raising.

            Args:
                name (``str``): name of the entry (e.g. ``ObservableType.PT``).

            Returns:
                ``int``:
                Index of the entry in ``values``.
            """
            if name in list(self.values.keys()):
                return list(self.values.keys()).index(name)
            else:
                return list(self.values.keys()).index('UNKNOWN')

        def accept_particles(self, index: int) -> bool:
            """Check whether an observable takes particles as arguments.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``bool``:
                ``True`` for particle observables (``PT``, ``M``, ...).
            """
            name = list(self.values.keys())[index]
            return self.values[name][0]

        def convert2string(self,index: int) -> str:
            """Get the name of an observable.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``str``:
                Name such as ``'PT'``.
            """
            return list(self.values.keys())[index]

        def convert2job_string(self,index: int,level: int) -> str:
            """Get the C++ expression of an observable for a running mode.

            Args:
                index (``int``): integer code of the entry.
                level (``int``): running mode (:class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType`).

            Returns:
                ``str``:
                C++ code (empty if not available in this mode).
            """
            name = list(self.values.keys())[index]
            if level==MA5RunningType.PARTON:
                return self.values[name][1]
            elif level==MA5RunningType.HADRON:
                return self.values[name][2]
            elif level==MA5RunningType.RECO:
                return self.values[name][3]
            return ""

        def convert2unit(self,index: int) -> str:
            """Get the unit of an observable.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``str``:
                Unit in LaTeX-like notation (e.g. ``'GeV/c'``).
            """
            name = list(self.values.keys())[index]
            return self.values[name][4]

        def convert2nbins(self,index: int) -> int:
            """Get the default number of bins.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``int``:
                Number of bins.
            """
            name = list(self.values.keys())[index]
            return self.values[name][5]

        def convert2xmin(self,index: int) -> float:
            """Get the default lower bound of the histograms.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``float``:
                Lower bound.
            """
            name = list(self.values.keys())[index]
            return self.values[name][6]

        def convert2xmax(self,index: int) -> float:
            """Get the default upper bound of the histograms.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``float``:
                Upper bound.
            """
            name = list(self.values.keys())[index]
            return self.values[name][7]

        def isCuttable(self,index: int) -> bool:
            """Check whether cuts can be applied on an observable.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``bool``:
                ``True`` if the observable can be used in cuts.
            """
            name = list(self.values.keys())[index]
            return self.values[name][8]

        def prefix(self,index: int) -> bool:
            """Check whether combination prefixes are allowed.

            Args:
                index (``int``): integer code of the entry.

            Returns:
                ``bool``:
                ``True`` if ``s``/``v``/``d``/``r`` prefixes are accepted.
            """
            name = list(self.values.keys())[index]
            return self.values[name][9]

        def get_list(self,level: int | None = None) -> list[str]:
            """Get the observables available in a running mode (with their prefixed variants).

            Args:
                level (``int | None``, default ``None``): running mode
                    (:class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType`); parton level
                    if ``None``.

            Returns:
                ``list[str]``:
                Observable names, including the prefixed names (``sPT``, ``vPT``, ...).
            """
            if level == None:
                level = MA5RunningType.PARTON
            output = []
            for item in self.values.keys():
                x = ObservableType.convert2job_string(list(self.values.keys()).index(item),level)
                if x=="":
                    continue
                output.append(item)
                if self.values[item][0] and self.values[item][9]:
                    output.append('s'+item)
                    output.append('v'+item)
                    output.append('sd'+item)
                    output.append('ds'+item)
                    output.append('d'+item)
                    output.append('dv'+item)
                    output.append('vd'+item)
                    output.append('r'+item)
            return output

        def get_cutlist1(self,level: int | None = None) -> list[str]:
            """Get the cuttable observables that do not take particle arguments (plus ``N``).

            Args:
                level (``int | None``, default ``None``): running mode
                    (:class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType`); parton level
                    if ``None``.

            Returns:
                ``list[str]``:
                Observable names usable in event-level cuts.
            """
            if level is None:
                level = MA5RunningType.PARTON
            output = []
            for item in self.values.keys():
                if item=="N":
                    output.append(item)
                    continue
                x = ObservableType.convert2job_string(list(self.values.keys()).index(item),level)
                if x=="":
                    continue
                if not self.values[item][8]:
                    continue
                if self.values[item][0]:
                    continue
                output.append(item)
            return output

        def get_cutlist2(self,level: int | None = None) -> list[str]:
            """Get the cuttable observables taking particle arguments (with prefixed variants).

            Args:
                level (``int | None``, default ``None``): running mode
                    (:class:`~madanalysis.enumeration.ma5_running_type.MA5RunningType`); parton level
                    if ``None``.

            Returns:
                ``list[str]``:
                Observable names usable in candidate-level cuts.
            """
            if level is None:
                level = MA5RunningType.PARTON
            output = []
            for item in self.values.keys():
                x = ObservableType.convert2job_string(list(self.values.keys()).index(item),level)
                if item=="N":
                    continue
                if x=="":
                    continue
                if not self.values[item][8]:
                    continue
                if not self.values[item][0]:
                    continue
                output.append(item)
                if not self.values[item][9]:
                    continue
                output.append('s'+item)
                output.append('v'+item)
                output.append('sd'+item)
                output.append('ds'+item)
                output.append('d'+item)
                output.append('dv'+item)
                output.append('vd'+item)
                output.append('r'+item)

            return output






@six.add_metaclass(metaclass)
class ObservableType(object):
    """Observables and their properties.

    Each entry of ``values`` is::

        [accept_particles, cpp_parton, cpp_hadron, cpp_reco, unit, nbins, xmin, xmax,
         cuttable, allow_combination_prefix]

    where ``accept_particles`` tells whether the observable takes particles as
    arguments, ``cpp_*`` is the C++ expression for each running mode (empty if the
    observable is not available), ``nbins/xmin/xmax`` are the default binning,
    ``cuttable`` tells whether cuts can be applied and ``allow_combination_prefix``
    whether the ``s``/``v``/``d``/``r`` prefixes of
    :class:`~madanalysis.enumeration.combination_type.CombinationType` are accepted.
    """

    # name : accept_particles 
    values = { 'UNKNOWN' : [False,'','','','',0,0,0,False,False],\
               'SQRTS' :   [False,'PHYSICS->SqrtS(event.mc())','PHYSICS->SqrtS(event.mc())','','GeV',100,0.,1000., True, False],\
               'TET' :     [False,'PHYSICS->Transverse->EventTET(event.mc())','PHYSICS->Transverse->EventTET(event.mc())',\
                            'PHYSICS->Transverse->EventTET(event.rec())','GeV',100,0.,1000., True,False],\
               'MET' :     [False,'PHYSICS->Transverse->EventMET(event.mc())','PHYSICS->Transverse->EventMET(event.mc())',\
                            'PHYSICS->Transverse->EventMET(event.rec())','GeV',100,0.,1000., True,False],\
               'THT' :     [False,'PHYSICS->Transverse->EventTHT(event.mc())','PHYSICS->Transverse->EventTHT(event.mc())',\
                            'PHYSICS->Transverse->EventTHT(event.rec())','GeV',100,0.,1000., True,False],\
               'MHT' :     [False,'PHYSICS->Transverse->EventMHT(event.mc())','PHYSICS->Transverse->EventMHT(event.mc())',\
                            'PHYSICS->Transverse->EventMHT(event.rec())','GeV',100,0.,1000.,True,False],\
               'WEIGHTS' : [False,'PHYSICS->weights(event.mc())','PHYSICS->weights(event.mc())','','',100,0.,1., True,False],\
               'NPID':     [False,'NPID','NPID','NPID','',100,0.,100.,False,False],\
               'NAPID':    [False,'NAPID','NAPID','NAPID','',100,0.,100.,False,False],\
               'E'   :     [True,'e()','e()','e()','GeV',100,0.,1000.,True,True],\
               'M'   :     [True,'m()','m()','m()','GeV/c^{2}',100,0.,1000.,True,True],\
               'P'   :     [True,'p()','p()','p()','GeV/c',100,0.,1000.,True,True],\
               'ET'  :     [True,'et()','et()','et()','GeV',100,0.,1000.,True,True],\
               'MT'  :     [True,'mt()','mt()','mt()','GeV/c^{2}',100,0.,1000.,True,True],\
               'PT'  :     [True,'pt()','pt()','pt()','GeV/c',100,0.,1000.,True,True],\
               'PX'  :     [True,'px()','px()','px()','GeV/c',100,-1000.,1000.,True,True],\
               'PY'  :     [True,'py()','py()','py()','GeV/c',100,-1000.,1000.,True,True],\
               'PZ'  :     [True,'pz()','pz()','pz()','GeV/c',100,-1000.,1000.,True,True],\
               'R'   :     [True,'r()','r()','r()','',100,0.,1000.,True,True],\
               'THETA' :   [True,'theta()','theta()','theta()','',100,0.,2*math.pi+0.01,True,True],\
               'ETA' :     [True,'eta()','eta()','eta()','',100,-8.0,+8.0,True,True],\
               'PHI' :     [True,'phi()','phi()','phi()','',100,0.,2*math.pi+0.01,True,True],\
               'Y'   :     [True,'y()','y()','y()','',100,-8.0,+8.0,True,True],\
               'BETA' :    [True,'beta()','beta()','beta()','',100,0.,1.,True,True],\
               'GAMMA':    [True,'gamma()','gamma()','gamma()','',100,1.,1000.,True,True],\
               'N'    :    [True,'N()','N()','N()','',20,0.,20.,True,True],\
               'ISOL' :    [True,'','','isolated()','',2,0,1,True,False],\
               'HE_EE':    [True,'','','HEoverEE()','',100,0,100,True,False],\
               'NTRACKS':  [True,'','','ntracks()','',100,0,100,True,False]  }

