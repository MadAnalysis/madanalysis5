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


"""Command-line interpreter of the normal mode (``ma5>`` prompt).

Each command ``<cmd>`` is implemented by a ``do_<cmd>`` method (execution),
``help_<cmd>`` (help) and ``complete_<cmd>`` (tab completion), all delegating to the
command objects of the ``cmd_*`` modules.
"""


# Import Interpreter core
from __future__ import absolute_import
from __future__ import print_function
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from madanalysis.core.main import Main
from madanalysis.interpreter.interpreter_base import InterpreterBase

# Import MadAnalysis main class
from madanalysis.core.main import Main

# Import Readers for multiparticles initializing
from madanalysis.IOinterface.particle_reader import ParticleReader
from madanalysis.IOinterface.multiparticle_reader import MultiparticleReader
from madanalysis.enumeration.cut_type import CutType

# List of command
from madanalysis.interpreter.cmd_set            import CmdSet
from madanalysis.interpreter.cmd_define         import CmdDefine
from madanalysis.interpreter.cmd_define_region  import CmdDefineRegion
from madanalysis.interpreter.cmd_import         import CmdImport
from madanalysis.interpreter.cmd_remove         import CmdRemove
from madanalysis.interpreter.cmd_swap           import CmdSwap
from madanalysis.interpreter.cmd_display        import CmdDisplay
from madanalysis.interpreter.cmd_display_particles      import CmdDisplayParticles
from madanalysis.interpreter.cmd_display_multiparticles import CmdDisplayMultiparticles
from madanalysis.interpreter.cmd_display_datasets       import CmdDisplayDatasets
from madanalysis.interpreter.cmd_display_regions        import CmdDisplayRegions
from madanalysis.interpreter.cmd_plot           import CmdPlot
from madanalysis.interpreter.cmd_cut            import CmdCut
from madanalysis.interpreter.cmd_submit         import CmdSubmit
from madanalysis.interpreter.cmd_open           import CmdOpen
from madanalysis.interpreter.cmd_reset          import CmdReset
from madanalysis.interpreter.cmd_install        import CmdInstall

import logging
import readline
import os
from six.moves import input


#===============================================================================
# Interpreter
#===============================================================================
class Interpreter(InterpreterBase):
    """MadAnalysis 5 command-line interpreter (:class:`cmd.Cmd` subclass).

    Attributes:
        main (``Main``): session state.
        cmd_<name>: command objects (e.g. :attr:`cmd_set`, :attr:`cmd_plot`,
            :attr:`cmd_select`/:attr:`cmd_reject`, :attr:`cmd_submit`/:attr:`cmd_resubmit`).
    """

    def __init__(self, main: Main,*arg, **opt) -> None:
        """Create the interpreter, its command objects and load the default (multi)particles.

        Args:
            main (``Main``): session state.
            *arg: arguments of :class:`cmd.Cmd`.
            **opt: keyword arguments of :class:`cmd.Cmd`.
        """

        # Calling constructor from InterpreterBase
        InterpreterBase.__init__(self, *arg, **opt)

        # Getting back main
        self.main = main

        # Getting back all commands
        self.cmd_set                    = CmdSet(main)
        self.cmd_define                 = CmdDefine(main)
        self.cmd_define_region          = CmdDefineRegion(main)
        self.cmd_display                = CmdDisplay(main)
        self.cmd_display_datasets       = CmdDisplayDatasets(main)
        self.cmd_display_multiparticles = CmdDisplayMultiparticles(main)
        self.cmd_display_particles      = CmdDisplayParticles(main)
        self.cmd_display_regions        = CmdDisplayRegions(main)
        self.cmd_import                 = CmdImport(main)
        self.cmd_remove                 = CmdRemove(main)
        self.cmd_swap                   = CmdSwap(main)
        self.cmd_plot                   = CmdPlot(main)
        self.cmd_reject                 = CmdCut(main,CutType.REJECT)
        self.cmd_select                 = CmdCut(main,CutType.SELECT)
        self.cmd_reset                  = CmdReset(main)
        self.cmd_open                   = CmdOpen(main)
        self.cmd_submit                 = CmdSubmit(main)
        self.cmd_resubmit               = CmdSubmit(main,resubmit=True)
        self.cmd_install                = CmdInstall(main)

        # Initializing multiparticle
        self.InitializeParticle()
        self.InitializeMultiparticle()


    def InitializeHistory(self) -> None:
        """Load the ``readline`` history from ``<ma5dir>/.ma5history``."""
        # Importing history
        self.history_file = os.path.normpath(self.main.archi_info.ma5dir + '/.ma5history')
        logging.getLogger('MA5').debug("Importing history from: "+self.history_file+" ...")
        if os.path.exists(self.history_file):
            logging.getLogger('MA5').debug("-> File found. Reading history ...")
            try:
                readline.read_history_file(self.history_file)
            except:
                logging.getLogger('MA5').debug("-> Problem during the reading. The history is skipped!")
        logging.getLogger('MA5').debug("-> Success!")


    def FinalizeHistory(self) -> None:
        """Save the ``readline`` history (500 lines at most) into ``<ma5dir>/.ma5history``.
        """
        # Importing history
        logging.getLogger('MA5').debug("Exporting the history to: "+self.history_file+" ...")
        readline.set_history_length(500)
        try:
            readline.write_history_file(self.history_file)
        except:
            logging.getLogger('MA5').debug("-> Problem during the writing. The history is not saved!")
        logging.getLogger('MA5').debug("-> Success!")


    def do_set(self,line: str) -> None:
        """Execute the ``set`` command (delegated to :class:`~madanalysis.interpreter.cmd_set.CmdSet`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_set.do(self.split_arg(line),line)

    def help_set(self) -> None:
        """Display the help of the ``set`` command."""
        self.cmd_set.help()

    def complete_set(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``set`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_set.complete(text,line,begidx,endidx)

    def do_define(self,line: str) -> None:
        """Execute the ``define`` command (delegated to :class:`~madanalysis.interpreter.cmd_define.CmdDefine`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_define.do(self.split_arg(line))

    def do_define_region(self,line: str) -> None:
        """Execute the ``define_region`` command (delegated to :class:`~madanalysis.interpreter.cmd_define_region.CmdDefineRegion`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_define_region.do(self.split_arg(line))

    def help_define(self) -> None:
        """Display the help of the ``define`` command."""
        self.cmd_define.help()

    def help_define_region(self) -> None:
        """Display the help of the ``define_region`` command."""
        self.cmd_define_region.help()

    def complete_define(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``define`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_define.complete(text,line,begidx,endidx)

    def complete_define_region(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``define_region`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_define_region.complete(text,line,begidx,endidx)

    def do_display(self,line: str) -> None:
        """Execute the ``display`` command (delegated to :class:`~madanalysis.interpreter.cmd_display.CmdDisplay`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_display.do(self.split_arg(line))

    def help_display(self) -> None:
        """Display the help of the ``display`` command."""
        self.cmd_display.help()

    def complete_display(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``display`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_display.complete(text,line,begidx,endidx)

    def do_display_particles(self,line: str) -> None:
        """Execute the ``display_particles`` command (delegated to :class:`~madanalysis.interpreter.cmd_display_particles.CmdDisplayParticles`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_display_particles.do(self.split_arg(line))

    def help_display_particles(self) -> None:
        """Display the help of the ``display_particles`` command."""
        self.cmd_display_particles.help()

    def complete_display_particles(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``display_particles`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_display_particles.complete(text,line,begidx,endidx)

    def do_display_multiparticles(self,line: str) -> None:
        """Execute the ``display_multiparticles`` command (delegated to :class:`~madanalysis.interpreter.cmd_display_multiparticles.CmdDisplayMultiparticles`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_display_multiparticles.do(self.split_arg(line))

    def help_display_multiparticles(self) -> None:
        """Display the help of the ``display_multiparticles`` command."""
        self.cmd_display_multiparticles.help()

    def complete_display_multiparticles(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``display_multiparticles`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_display_multiparticles.complete(text,line,begidx,endidx)

    def do_display_datasets(self,line: str) -> None:
        """Execute the ``display_datasets`` command (delegated to :class:`~madanalysis.interpreter.cmd_display_datasets.CmdDisplayDatasets`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_display_datasets.do(self.split_arg(line))

    def do_display_regions(self,line: str) -> None:
        """Execute the ``display_regions`` command (delegated to :class:`~madanalysis.interpreter.cmd_display_regions.CmdDisplayRegions`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_display_regions.do(self.split_arg(line))

    def help_display_datasets(self) -> None:
        """Display the help of the ``display_datasets`` command."""
        self.cmd_display_datasets.help()

    def help_display_regions(self) -> None:
        """Display the help of the ``display_regions`` command."""
        self.cmd_display_regions.help()

    def complete_display_datasets(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``display_datasets`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_display_datasets.complete(text,line,begidx,endidx)

    def complete_display_regions(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``display_regions`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_display_regions.complete(text,line,begidx,endidx)

    def do_import(self,line: str) -> None:
        """Execute the ``import`` command (delegated to :class:`~madanalysis.interpreter.cmd_import.CmdImport`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_import.do(self.split_arg(line),self,self.history)

    def help_import(self) -> None:
        """Display the help of the ``import`` command."""
        self.cmd_import.help()

    def complete_import(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``import`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_import.complete(text,line,begidx,endidx)

     # Restart
    def do_restart(self, line: str) -> bool | None:
        """Request a restart of the session (after confirmation, unless in forced mode).

        Args:
            line (``str``): ignored.

        Returns:
            ``bool | None``:
            ``True`` (stops the command loop and sets ``main.repeatSession``) if the restart
            is accepted, ``None`` otherwise.
        """

        # Note: Restart is available in script mode
        # Asking the safety question
        YES=True
        if not Main.forced:
           logging.getLogger('MA5').warning("Are you sure to restart the MadAnalysis 5 session? (Y/N)")
           allowed_answers=['n','no','y','yes']
           answer=""
           while answer not in  allowed_answers:
              answer=input("Answer: ")
              answer=answer.lower()
              if answer=="no" or answer=="n":
                   YES=False
                   break
              elif answer=='yes' or answer=='y':
                   YES=True
                   break

        # Restart?
        if YES:
            self.main.repeatSession=True
            return True
        else:
            return None

    def help_restart(self) -> None:
        """Display the help of the ``restart`` command."""
        logging.getLogger('MA5').info("   Syntax: restart ")
        logging.getLogger('MA5').info("   Quit the current MadAnalysis sessiona and open a new one.")
        logging.getLogger('MA5').info("   All the information will be discarded.")

    def do_remove(self,line: str) -> None:
        """Execute the ``remove`` command (delegated to :class:`~madanalysis.interpreter.cmd_remove.CmdRemove`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_remove.do(self.split_arg(line))

    def help_remove(self) -> None:
        """Display the help of the ``remove`` command."""
        self.cmd_remove.help()

    def complete_remove(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``remove`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_remove.complete(text,line,begidx,endidx)

    def do_swap(self,line: str) -> None:
        """Execute the ``swap`` command (delegated to :class:`~madanalysis.interpreter.cmd_swap.CmdSwap`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_swap.do(self.split_arg(line))

    def help_swap(self) -> None:
        """Display the help of the ``swap`` command."""
        self.cmd_swap.help()

    def complete_swap(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``swap`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_swap.complete(text,line,begidx,endidx)

    def do_install(self,line: str) -> bool | None:
        """Execute the ``install`` command (delegated to :class:`~madanalysis.interpreter.cmd_install.CmdInstall`).

        Args:
            line (``str``): arguments typed after the command name.

        Returns:
            ``bool | None``:
            ``True`` if a restart has been accepted after the installation, ``None`` otherwise.
        """
        result = self.cmd_install.do(self.split_arg(line))
        if result=='restart':
            logging.getLogger('MA5').info(" ")
            logging.getLogger('MA5').info("MadAnalysis 5 must be restarted for taking into account the present installation.")
            return self.do_restart('restart')

    def help_install(self) -> None:
        """Display the help of the ``install`` command."""
        self.cmd_install.help()

    def complete_install(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``install`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_install.complete(text,self.split_arg(line),begidx,endidx)

    def do_open(self,line: str) -> None:
        """Execute the ``open`` command (delegated to :class:`~madanalysis.interpreter.cmd_open.CmdOpen`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_open.do(self.split_arg(line))

    def help_open(self) -> None:
        """Display the help of the ``open`` command."""
        self.cmd_open.help()

    def complete_open(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``open`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_open.complete(text,line,begidx,endidx)

    def do_reset(self,line: str) -> None:
        """Execute the ``reset`` command (delegated to :class:`~madanalysis.interpreter.cmd_reset.CmdReset`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_reset.do(self.split_arg(line),self)

    def help_reset(self) -> None:
        """Display the help of the ``reset`` command."""
        self.cmd_reset.help()

    def complete_reset(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``reset`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_reset.complete(text,line,begidx,endidx)

    def do_submit(self,line: str) -> None:
        """Execute the ``submit`` command (delegated to :class:`~madanalysis.interpreter.cmd_submit.CmdSubmit`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_submit.do(self.split_arg(line),self.history)

    def help_submit(self) -> None:
        """Display the help of the ``submit`` command."""
        self.cmd_submit.help()

    def complete_submit(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``submit`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_submit.complete(text,line,begidx,endidx)

    def do_resubmit(self,line: str) -> None:
        """Execute the ``resubmit`` command (delegated to :class:`~madanalysis.interpreter.cmd_submit.CmdSubmit`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_resubmit.do(self.split_arg(line),self.history)

    def help_resubmit(self) -> None:
        """Display the help of the ``resubmit`` command."""
        self.cmd_resubmit.help()

    def complete_resubmit(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``resubmit`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        return self.cmd_resubmit.complete(text,line,begidx,endidx)

    def do_plot(self,line: str) -> None:
        """Execute the ``plot`` command (delegated to :class:`~madanalysis.interpreter.cmd_plot.CmdPlot`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_plot.do(self.split_arg(line))

    def help_plot(self) -> None:
        """Display the help of the ``plot`` command."""
        self.cmd_plot.help()

    def complete_plot(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``plot`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        tmp = line.replace("["," [ ")
        tmp = tmp.replace("]"," ] ")
        tmp = tmp.replace(")"," ) ")
        tmp = tmp.replace("("," ( ")
        tmp = tmp.replace(","," , ")
        tmp = tmp.replace("{"," { ")
        tmp = tmp.replace("}"," } ")
        return self.cmd_plot.complete(text,self.split_arg(tmp),begidx,endidx)

    def do_reject(self,line: str) -> None:
        """Execute the ``reject`` command (delegated to :class:`~madanalysis.interpreter.cmd_cut.CmdCut`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_reject.do(self.split_arg(line))

    def help_reject(self) -> None:
        """Display the help of the ``reject`` command."""
        self.cmd_reject.help()

    def complete_reject(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``reject`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        tmp = line.replace("["," [ ")
        tmp = tmp.replace("]"," ] ")
        tmp = tmp.replace("("," ( ")
        tmp = tmp.replace(")"," ) ")
        tmp = tmp.replace("{"," { ")
        tmp = tmp.replace("}"," } ")
        return self.cmd_reject.complete(text,self.split_arg(tmp),begidx,endidx)

    def do_select(self,line: str) -> None:
        """Execute the ``select`` command (delegated to :class:`~madanalysis.interpreter.cmd_cut.CmdCut`).

        Args:
            line (``str``): arguments typed after the command name.
        """
        self.cmd_select.do(self.split_arg(line))

    def help_select(self) -> None:
        """Display the help of the ``select`` command."""
        self.cmd_select.help()

    def complete_select(self,text: str,line: str,begidx: int,endidx: int) -> list[str] | None:
        """Tab completion of the ``select`` command.

        Args:
            text (``str``): word being completed.
            line (``str``): full input line.
            begidx (``int``): start index of ``text`` in ``line``.
            endidx (``int``): end index of ``text`` in ``line``.

        Returns:
            ``list[str] | None``:
            Possible completions.
        """
        tmp = line.replace("["," [ ")
        tmp = tmp.replace("]"," ] ")
        tmp = tmp.replace("("," ( ")
        tmp = tmp.replace(")"," ) ")
        tmp = tmp.replace("{"," { ")
        tmp = tmp.replace("}"," } ")
        return self.cmd_select.complete(text,self.split_arg(tmp),begidx,endidx)

    def InitializeParticle(self) -> None:
        """Load the default particle labels of the running mode."""
        input = ParticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode,self.main.forced)
        input.Load()

    def InitializeMultiparticle(self) -> None:
        """Load the default multiparticle labels of the running mode."""
        input = MultiparticleReader(self.main.archi_info.ma5dir,self.cmd_define,self.main.mode,self.main.forced)
        input.Load()

    # PreLoop
    def preloop(self) -> None:
        """Set the prompt (``ma5>``) before entering the command loop."""
        self.prompt = 'ma5>'
#        if readline and not 'libedit' in readline.__doc__:
#            readline.set_completion_display_matches_hook(self.print_suggestions)

    def deal_multiple_categories(self, dico: dict[str, list[str]]) -> list[str]:
        """Format completions grouped by category for the custom ``readline`` display.

        Args:
            dico (``dict[str, list[str]]``): completions per category.

        Returns:
            ``list[str]``:
            Completions with ``@@<category>@@`` markers (plain list with libedit).
        """

        if 'libedit' in readline.__doc__:
            # No parser in this case, just send all the valid options
            out = []
            for name, opt in dico.items():
                out += opt
            return out

        # That's the real work
        out = []
        valid=0
        # if the key starts with number order the key with that number.
        for name, opt in dico.items():
            if not opt:
                continue
            name = name.replace(' ', '_')
            valid += 1
            out.append(opt[0].rstrip()+'@@'+name+'@@')
            # Remove duplicate
            d = {}
            for x in opt:
                d[x] = 1    
            opt = list(d.keys())
            opt.sort()
            out += opt

            
        if valid == 1:
            out = out[1:]
        return out
    
    def print_suggestions(self, substitution: str, matches: list[str], longest_match_length: int)  -> None:
        """Display the completions (grouped by category) below the prompt.

        Args:
            substitution (``str``): text being completed (unused).
            matches (``list[str]``): completions.
            longest_match_length (``int``): length of the longest completion.
        """
        longest_match_length += len(self.completion_prefix)
        try:
            if len(matches) == 1:
                self.stdout.write(matches[0]+' ')
                return
            self.stdout.write('\n')
            l2 = [a[-2:] for a in matches]
            if '@@' in l2:
                nb_column = self.getTerminalSize()//(longest_match_length+1)
                pos=0
                for val in self.completion_matches:
                    if val.endswith('@@'):
                        category = val.rsplit('@@',2)[1]
                        category = category.replace('_',' ')
                        self.stdout.write('\n %s:\n%s\n' % (category, '=' * (len(category)+2)))
                        start = 0
                        pos = 0
                        continue
                    elif pos and pos % nb_column ==0:
                        self.stdout.write('\n')
                    self.stdout.write(self.completion_prefix + val + \
                                      ' ' * (longest_match_length +1 -len(val)))
                    pos +=1
                self.stdout.write('\n')
            else:
                # nb column
                nb_column = self.getTerminalSize()//(longest_match_length+1)
                for i,val in enumerate(matches):
                    if i and i%nb_column ==0:
                        self.stdout.write('\n')
                    self.stdout.write(self.completion_prefix + val + \
                                     ' ' * (longest_match_length +1 -len(val)))
                self.stdout.write('\n')
    
            self.stdout.write(self.prompt+readline.get_line_buffer())
            self.stdout.flush()
        except Exception as error:
            if __debug__:
                 print(error)

    def getTerminalSize(self) -> int:
        """Get the width of the terminal.

        Returns:
            ``int``:
            Number of columns (80 if it cannot be determined).
        """
        def ioctl_GWINSZ(fd: int) -> tuple[int, int] | None:
            """Query the terminal size through ``ioctl``.

            Args:
                fd (``int``): file descriptor.

            Returns:
                ``tuple[int, int] | None``:
                ``(rows, columns)``, or ``None`` on failure.
            """
            try:
                import fcntl, termios, struct, os
                cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ,
                                                     '1234'))
            except:
                return None
            return cr
        cr = ioctl_GWINSZ(0) or ioctl_GWINSZ(1) or ioctl_GWINSZ(2)
        if not cr:
            try:
                fd = os.open(os.ctermid(), os.O_RDONLY)
                cr = ioctl_GWINSZ(fd)
                os.close(fd)
            except:
                pass
        if not cr:
            try:
                cr = (os.environ['LINES'], os.environ['COLUMNS'])
            except:
                cr = (25, 80)
        return int(cr[1])

    #def complete(self, text, state):
    def complete2(self,text: str,state: int) -> str | None:
        """Legacy ``readline`` completion function (not installed as the completer).

        If no command has been typed yet, the command names are completed; otherwise
        ``complete_<command>`` is used. Commands separated by ``;`` and escaped spaces are
        supported.

        Args:
            text (``str``): word being completed.
            state (``int``): index of the requested completion.

        Returns:
            ``str | None``:
            The ``state``-th completion, or ``None`` when exhausted.
        """
                
        if state == 0:
            import readline
            origline = readline.get_line_buffer()
            line = origline.lstrip()
            stripped = len(origline) - len(line)
            begidx = readline.get_begidx() - stripped
            endidx = readline.get_endidx() - stripped
            
            if ';' in line:
                begin, line = line.rsplit(';',1)
                begidx = begidx - len(begin) - 1
                endidx = endidx - len(begin) - 1
                if line[:begidx] == ' ' * begidx:
                    begidx=0

            if begidx>0:
                cmd, args, foo = self.parseline(line)
                if cmd == '':
                    compfunc = self.completedefault
                else:
                    try:
                        compfunc = getattr(self, 'complete_' + cmd)
                    except AttributeError:
                        compfunc = self.completedefault
            else:
                compfunc = self.completenames
                
            # correct wrong splittion with '\ '
            # NOTE: the literal backslash-space below is an invalid escape sequence (SyntaxWarning with
            # recent Python versions); it evaluates to a backslash followed by a space.
            if line and begidx > 2 and line[begidx-2:begidx] == '\ ':
                Ntext = line.split(os.path.sep)[-1]
                self.completion_prefix = Ntext.rsplit('\ ', 1)[0] + '\ '
                to_rm = len(self.completion_prefix) - 1
                Nbegidx = len(line.rsplit(os.path.sep, 1)[0]) + 1
                data = compfunc(Ntext.replace('\ ', ' '), line, Nbegidx, endidx)
                self.completion_matches = [p[to_rm:] for p in data 
                                              if len(p)>to_rm]                
            # correct wrong splitting with '-'
            elif line and line[begidx-1] == '-':
             try:    
                Ntext = line.split()[-1]
                self.completion_prefix = Ntext.rsplit('-',1)[0] +'-'
                to_rm = len(self.completion_prefix)
                Nbegidx = len(line.rsplit(None, 1)[0])
                data = compfunc(Ntext, line, Nbegidx, endidx)
                self.completion_matches = [p[to_rm:] for p in data 
                                              if len(p)>to_rm]
             except Exception as error:
                 print(error)
            else:
                self.completion_prefix = ''
                self.completion_matches = compfunc(text, line, begidx, endidx)
        #print self.completion_matches

        self.completion_matches = [ (l[-1] in [' ','@','=',os.path.sep] 
                      and l or (l+' ')) for l in self.completion_matches if l]
        
        try:
            return self.completion_matches[state]
        except IndexError as error:
            #if __debug__:
            #    print '\n Completion ERROR:'
            #    print error
            #    print '\n'
            return None    
