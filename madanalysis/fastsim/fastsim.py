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


"""Simplified Fast Simulation (SFS) of MadAnalysis 5 (arXiv:2006.09387).

The SFS parametrises the detector response with user-defined formulas:

* ``define tagger <true> as <reco> <efficiency> [<bounds>] {<working point>}``
* ``define smearer <object> with <observable> <resolution> [<bounds>]``
* ``define reco_efficiency <object> <efficiency> [<bounds>]``
* ``define jes <scale> [<bounds>]`` and ``define energy_scaling <object> <scale> [<bounds>]``
* ``define scaling <observable> for <object> <scale> [<bounds>]``

The rules are converted into C++ code by :mod:`madanalysis.job.job_tagger_header`,
:mod:`madanalysis.job.job_tagger_main`, :mod:`madanalysis.job.job_smearer_reco_header`
and :mod:`madanalysis.job.job_smearer_reco_main`.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any
import logging
from madanalysis.fastsim.ast            import AST
from madanalysis.fastsim.tagger         import Tagger, TaggerStatus
from madanalysis.fastsim.smearer        import Smearer
from madanalysis.fastsim.recoefficiency import RecoEfficiency
from madanalysis.fastsim.scaling        import Scaling
from six.moves import range


class SuperFastSim:
    """Container of all SFS rules and settings (``main.superfastsim``).

    Attributes:
        tagger / smearer / reco / scaling: the corresponding rule containers.
        jetrecomode (``str``): ``"jets"`` (smear the clustered jets) or ``"constituents"``
            (smear the jet constituents before clustering).
        mag_field / radius / half_length (``float``): magnetic field (T) and tracker
            dimensions (m) used by the particle propagator.
        propagator (``bool``): whether charged particles are propagated in the magnetic field.
        track_isocone_radius / electron_isocone_radius / muon_isocone_radius /
        photon_isocone_radius (``list[float]``): isolation cone radii to compute.
        observables (``ObservableManager | str``): observables usable in formulas.
    """

    # Initialization
    def __init__(self) -> None:
        """Create an empty SFS configuration."""
        self.logger                  = logging.getLogger('MA5')
        self.tagger                  = Tagger()
        self.smearer                 = Smearer()
        self.reco                    = RecoEfficiency()
        self.scaling                 = Scaling()
        self.jetrecomode             = 'jets'
        self.mag_field               = 1e-9
        self.radius                  = 1e+99
        self.half_length             = 1e+99
        self.propagator              = False
        self.track_isocone_radius    = []
        self.electron_isocone_radius = []
        self.muon_isocone_radius     = []
        self.photon_isocone_radius   = []
        self.observables             = ''

    def InitObservables(self, obs_list: Any) -> None:
        """Set the observables usable in the SFS formulas.

        Args:
            obs_list (``ObservableManager``): observable registry of the session.
        """
        self.observables = obs_list

    def Reset(self) -> None:
        """Remove all rules and reset the settings (the observables are kept)."""
        self.tagger                  = Tagger()
        self.smearer                 = Smearer()
        self.reco                    = RecoEfficiency()
        self.scaling                 = Scaling()
        self.jetrecomode             = 'jets'
        self.mag_field               = 1e-9
        self.radius                  = 1e+99
        self.half_length             = 1e+99
        self.propagator              = False
        self.track_isocone_radius    = []
        self.electron_isocone_radius = []
        self.muon_isocone_radius     = []
        self.photon_isocone_radius   = []

    # Definition of a new tagging/smearing rule
    def define(self, args: list[str], prts: Any) -> None:
        """Decode a ``define tagger/smearer/reco_efficiency/jes/energy_scaling/scaling``
        command and store the corresponding rule.

        The labels ``c``, ``track`` and ``JES`` are temporarily added to the
        multiparticle collection while the command is decoded. Errors are logged and the
        command is ignored.

        Args:
            args (``list[str]``): arguments of the ``define`` command.
            prts (``MultiParticleCollection``): (multi)particles of the session.
        """
        # FIXME: if the user already defined 'c', Add() asks interactively to overwrite it and the
        # label is removed at the end of the command, deleting the user definition.
        prts.Add('c', [4])
        prts.Add('track', [])  # PDGID is not important
        prts.Add('JES', [])  # PDGID is not important
        prts_remove = ['c', 'track', 'JES']

        ## remove all initializations when this session is over
        def remove_prts_def(prts_remove: list[str],prts: Any) -> None:
            """Remove the temporary labels from the multiparticle collection.

            Args:
                prts_remove (``list[str]``): labels to remove.
                prts (``MultiParticleCollection``): multiparticle collection.
            """
            for particle in prts_remove:
                prts.Remove(particle,     None)

        ## list of PDG codes associated with a a multiparticle
        def is_pdgcode(prt: str) -> bool:
            """Check whether a string is a (signed) PDG code.

            Args:
                prt (``str``): string to test.

            Returns:
                ``bool``:
                ``True`` for strings like ``11``, ``-11`` or ``+11``.
            """
            return (prt[0] in ('-','+') and prt[1:].isdigit()) or prt.isdigit()

        ## Checking the length of the argument list
        if (args[0] == 'tagger' and len(args) < 5) or (args[0] == 'smearer' and len(args) < 3) \
                or (args[0] == 'reco_efficiency' and len(args) < 3) or (args[0] == 'jes' and len(args) < 2) \
                or (args[0] == 'energy_scaling' and len(args) < 3) or (args[0] == 'scaling' and len(args) < 4):
            self.logger.error('Not enough arguments for tagging/smearing/reconstruction/scaling')
            remove_prts_def(prts_remove, prts)
            return

        ## Checking the first argument
        if args[0]=='jes':
            true_id = 'JES'
        elif args[0]=='scaling':
            true_id = args[3]
        else:
            true_id = args[1]
        #### First, do we have either a multiparticle or a PDG code
        if not (true_id in prts.GetNames() or is_pdgcode(true_id)):
            self.logger.error('the 1st argument must be a PDG code or (multi)particle label')
            remove_prts_def(prts_remove,prts)
            return
        #### Second let's check if we have a multiparticle associated with a unique PDGID
        if true_id in prts.GetNames() and len(list(set([abs(x) for x in prts[true_id]])))==1:
            true_id = str(abs(prts[true_id][0]))
        #### Third, we have a number
        elif is_pdgcode(true_id):
            true_id=str(abs(int(true_id)))
        #### light jet protection
        if true_id in ['1','2','3', 'j']:
            true_id = '21'

        ## Checking the second and third arguments of a tagger
        if args[0]=='tagger':
            if args[2]!='as':
                self.logger.error('the 2nd argument must be the keyword \'as\'')
                remove_prts_def(prts_remove,prts)
                return
            reco_id = args[3]
            #### First, do we have either a multiparticle or a PDG code
            if not (reco_id in prts.GetNames() or is_pdgcode(reco_id)):
                self.logger.error('the 4th argument must be a PDG code or (multi)particle label')
                remove_prts_def(prts_remove,prts)
                return
            #### Second let's check if we have a multiparticle associated with a unique PDGID
            if reco_id in prts.GetNames() and len(list(set([abs(x) for x in prts[reco_id]])))==1:
                reco_id = str(abs(prts[reco_id][0]))
            #### Third, we have a number
            elif is_pdgcode(reco_id):
                reco_id=str(abs(int(reco_id)))
            #### light jet protection
            if reco_id in ['1','2','3', 'j']:
                reco_id = '21'
            to_decode=args[4:]

        ## Checking the second and third arguments of a smearer
        elif args[0] == 'smearer':
            if args[2]!='with':
                self.logger.error('the 2nd argument must be the keyword \'with\'')
                remove_prts_def(prts_remove,prts)
                return
            obs = args[3].upper()
            if not (obs in self.smearer.vars):
                self.logger.error('the 4th argument must be an observable in '+ ', '.join(self.smearer.vars))
                remove_prts_def(prts_remove,prts)
                return
            to_decode=args[4:]

        ## Checking the second and third arguments of a smearer
        elif args[0] == 'reco_efficiency':
            to_decode=args[2:]

        ## Jet energy scaling (and scaling in general)
        elif args[0]=='jes':
            to_decode=args[1:]
            obs = 'E'
        elif args[0]=='energy_scaling':
            to_decode=args[2:]
            obs = 'E'
        elif args[0]=='scaling':
            if args[2]!='for':
                self.logger.error('Scaling - the 2nd argument must be the keyword \'for\'')
                remove_prts_def(prts_remove,prts)
                return
            obs = args[1].upper()
            if not (obs in self.scaling.vars):
                self.logger.error('Scaling - the 1st argument must be an observable in '+ ', '.join(self.scaling.vars))
                remove_prts_def(prts_remove,prts)
                return
            to_decode=args[4:]

        ## Getting the bounds and the function
        function, bounds, tags = self.decode_args(to_decode)
        if function=='':
            self.logger.error('Cannot decode the function or the bounds - ' + args[0] + ' ignored.')
            remove_prts_def(prts_remove,prts)
            return

        ## Adding a rule to a tagger/smearer
        if args[0] == 'tagger':
            self.tagger.add_rule(true_id, reco_id, function, bounds, TaggerStatus.get_status(tags))
        elif args[0] == 'smearer':
            self.smearer.add_rule(true_id, obs, function, bounds)
        elif args[0] == 'reco_efficiency':
            self.reco.add_rule(true_id, function, bounds)
        elif args[0] in ['jes', 'energy_scaling', 'scaling']:
            self.scaling.add_rule(true_id, obs, function, bounds)
        remove_prts_def(prts_remove, prts)
        return


    # Transform the arguments passed in the interpreter in the right format
    def decode_args(self,myargs: list[str]) -> tuple[Any, Any, str | None]:
        """Split the arguments of an SFS rule into function, bounds and working point.

        The bounds are given between square brackets and the working point between curly
        brackets; the remaining leading arguments form the function.

        Args:
            myargs (``list[str]``): arguments following the object specification.

        Returns:
            ``tuple[Any, Any, str | None]``:
            The function and bounds as :class:`~madanalysis.fastsim.ast.AST` objects (``''``
            and ``[]`` on error) and the working-point string (``None`` on error, ``""`` if
            absent).
        """
        # Special formating for the power operator
        args = ' '.join(myargs).replace('^', ' ^ ')
        for symb in ['< =', '> =', '= =']:
            args = args.replace(symb, ''.join(symb.split()))
        args = args.split()

        ## To get the difference pieces of the command
        Nbracket1    = 0
        Nbracket2    = 0
        Nbracket3    = 0
        beginOptions = len(args)
        endOptions   = -1
        foundOptions = False
        beginTags    = len(args)
        endTags      = -1
        foundTags    = False

        ## Extraction of the arguments
        for i in range(0,len(args)):
            if args[i]=='(':
                Nbracket1+=1
            elif args[i]==')':
                Nbracket1-=1
            elif args[i] == '[':
                Nbracket2+=1
                if Nbracket1==0:
                    beginOptions = i
                    foundOptions = True
            elif args[i] == ']':
                Nbracket2-=1
                if Nbracket1==0:
                    endOptions = i
            # Look for tagging options
            elif args[i] == "{":
                Nbracket3+=1
                if Nbracket1 == 0 and Nbracket2 == 0:
                    beginTags = i
                    foundTags = True
            elif args[i] == "}":
                Nbracket3-=1
                if Nbracket1 == 0 and Nbracket2 == 0:
                    endTags = i


        ## Sanity
        if Nbracket1!=0:
            self.logger.error("number of opening '(' and closing ')' does not match.")
            return '', [], None
        if Nbracket2!=0:
            self.logger.error("number of opening '[' and closing ']' does not match.")
            return '', [], None
        if Nbracket3!=0:
            self.logger.error("number of opening '{' and closing '}' does not match.")
            return '', [], None

        ## Find tags
        tags = args[beginTags + 1:endTags]
        if len(tags) > 1:
            self.logger.error("Can not process more than one tag.")
            return '', [], None
        tags = tags[0] if len(tags) == 1 else ""

        ## Putting the bounds into an AST
        bounds = ' '.join(args[beginOptions+1:endOptions])
        if bounds in ['', ' ']:
            bounds = 'true'
        ast_bounds = AST(0, self.observables.full_list)
        ast_bounds.feed(bounds)

        ## Putting the efficiency into an AST
        start_from = beginOptions if foundOptions else beginTags
        efficiency = ' ' .join(args[:start_from])
        ast_eff = AST(1, self.observables.full_list)
        ast_eff.feed(efficiency)

        self.logger.debug(f"Tags: {tags}\nBounds: {bounds}\nefficiency: {efficiency}")

        return ast_eff, ast_bounds, tags



    # Display of a taggers/smearer
    def display(self,args: list[str]) -> None:
        """Log the rules of one SFS module.

        Args:
            args (``list[str]``): ``[module]`` with module ``tagger``, ``smearer``,
                ``reco_efficiency``, ``jes``, ``energy_scaling`` or ``scaling``.
        """
        if args[0]=='tagger':
            self.tagger.display()
        elif args[0]=='smearer':
            self.smearer.display(self.jetrecomode)
        elif args[0]=='reco_efficiency':
            self.reco.display()
        elif args[0] in ['jes','energy_scaling','scaling']:
            self.scaling.display(self.jetrecomode)
        return



    # On/off checks
    def isRecoOn(self) -> bool:
        """Check whether reconstruction efficiencies are defined.

        Returns:
            ``bool``:
            ``True`` if at least one rule exists.
        """
        return self.reco.rules != {}
    def isTaggerOn(self) -> bool:
        """Check whether taggers are defined.

        Returns:
            ``bool``:
            ``True`` if at least one rule exists.
        """
        return self.tagger.rules != {}
    def isSmearerOn(self) -> bool:
        """Check whether smearers are defined.

        Returns:
            ``bool``:
            ``True`` if at least one rule exists.
        """
        return self.smearer.rules != {}
    def isPropagatorOn(self) -> bool:
        """Check whether the particle propagator is enabled.

        Returns:
            ``bool``:
            :attr:`propagator`.
        """
        return self.propagator
    def isScalingOn(self) -> bool:
        """Check whether scaling rules are defined.

        Returns:
            ``bool``:
            ``True`` if at least one rule exists.
        """
        return self.scaling.rules != {}
    def isRecoSmearerOn(self) -> bool:
        """Check whether the body of the generated smearer is needed.

        Returns:
            ``bool``:
            ``True`` if scaling, smearing or reconstruction rules exist.
        """
        # all modules that modifies new_smearer body
        return (self.isScalingOn() or self.isSmearerOn() or self.isRecoOn())
    def isNewSmearerOn(self) -> bool:
        """Check whether a generated smearer (header) is needed.

        Returns:
            ``bool``:
            ``True`` if :meth:`isRecoSmearerOn` or the propagator is on.
        """
        # all modules that modifies new_smearer header
        return (self.isRecoSmearerOn() or self.isPropagatorOn())
    def isSFSOn(self) -> bool:
        """Check whether any SFS functionality is used.

        Returns:
            ``bool``:
            ``True`` if a smearer or a tagger is needed.
        """
        return (self.isNewSmearerOn() or self.isTaggerOn())
