#!/usr/bin/env python

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


"""Decorator isolating the environment variables of the MadAnalysis 5 interpreter used from MadGraph.
"""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any, Callable
import os
class Architecture(Exception):
    """Raised when the architecture file cannot be saved."""
    pass

def freeze_environment(func: Callable) -> Callable:
    """Run a method of :class:`~madanalysis.interpreter.ma5_interpreter.MA5Interpreter` in the
    MadAnalysis 5 environment.

    Before the call, ``os.environ`` is replaced by ``self.ma5_environ``; after the call,
    the (possibly modified) environment is stored back into ``self.ma5_environ``, the
    architecture is saved in ``tools/architecture.ma5`` and the caller's environment is
    restored.

    Args:
        func (``Callable``): method to wrap.

    Raises:
        ``Architecture``: if the architecture file cannot be saved.

    Returns:
        ``Callable``:
        The wrapped method.
    """

    def newf(self: Any, *args, **opts) -> Any:
        """Wrapped method (see :func:`freeze_environment`).

        Args:
            self (``MA5Interpreter``): interpreter instance.
            *args: positional arguments of the method.
            **opts: keyword arguments of the method.

        Returns:
            ``Any``:
            The result of the method.
        """
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
        # NOTE: if func raises, the caller's environment is not restored (no try/finally).
        os.environ.clear()
        os.environ.update(old_environ)

        # output
        return out

    return newf
