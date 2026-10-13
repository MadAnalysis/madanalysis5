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


"""Periodic timer calling a function at regular intervals in a background thread."""

from __future__ import absolute_import
from __future__ import annotations
from typing import Any, Callable
import threading
import time

class Timer:
    """Repeating timer based on :class:`threading.Timer`.

    The target is called every ``tempo`` seconds from :meth:`start` until :meth:`stop`.
    """
    def __init__(self,tempo: float,target: Callable[..., Any],args: list[Any] = [],kwargs: dict[str, Any] = {}) -> None:
        """Initialise the timer (not started).

        Args:
            tempo (``float``): period in seconds.
            target (``Callable[..., Any]``): function to call periodically.
            args (``list[Any]``, default ``[]``): positional arguments of ``target``.
            kwargs (``dict[str, Any]``, default ``{}``): keyword arguments of ``target``.
        """
        self.target = target
        self.args = args
        self.kwargs = kwargs
        self.tempo = tempo

    def run(self) -> None:
        """Re-arm the timer and call the target (executed at each period)."""
        self.timer = threading.Timer(self.tempo,self.run)
        self.timer.start()
        self.target(*self.args,**self.kwargs)

    def start(self) -> None:
        """Start the timer (the first call happens after one period)."""
        self.timer = threading.Timer(self.tempo,self.run)
        self.timer.start()

    def stop(self) -> None:
        """Cancel the pending call."""
        self.timer.cancel()
        
