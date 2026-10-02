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


"""Session information that does not require a rebuild of SampleAnalyzer when it changes.
"""

from __future__ import absolute_import
from __future__ import annotations
import logging


class SessionInfo:
    """Session information (not stored with the libraries).

    Attributes:
        editor (``str``): text editor used to edit cards.
        username (``str``): user name.
        tmpdir / downloaddir (``str``): temporary and download folders.
        has_root / has_matplotlib / has_gnuplot / has_pdflatex / has_latex / has_dvipdf
            (``bool``): graphical and report packages.
        has_spey / has_simplify (``bool``): statistics packages.
        has_web (``bool``): whether web access is allowed.
        has_pad / has_padma5 / has_padsfs (``bool``): installed PADs, with their
            ``*_build_path`` and ``*_original_bins``.
        gcc_header_search_path / gcc_library_search_path (``list[str]``): search paths of
            the compiler.
    """
    def __init__(self) -> None:
        """Initialise an empty session information."""
        self.editor = ""
        self.username = ""
        self.tmpdir = ""
        self.downloaddir = ""
        self.has_root = False
        self.has_matplotlib = False
        self.has_spey = False
        self.has_simplify = False
        self.has_gnuplot = False
        self.has_pdflatex = False
        self.has_latex = False
        self.has_dvipdf = False
        self.has_web = True
        self.has_pad = False
        self.has_padsfs = False
        self.has_padma5 = False
        self.gcc_header_search_path = []
        self.gcc_library_search_path = []
        self.padma5_build_path = ""
        self.padma5_original_bins = []
        self.pad_build_path = ""
        self.pad_original_bins = []
        self.logger = logging.getLogger("MA5")

    def dump(self) -> None:
        """Log all attributes at debug level."""
        for item in self.__dict__:
            self.logger.debug(item + "\t" + str(self.__dict__[item]))

    def __eq__(self, other: SessionInfo) -> bool:
        """Compare all attributes with another object.

        Args:
            other (``SessionInfo``): object to compare with.

        Returns:
            ``bool``:
            ``True`` if all attributes are equal.
        """
        return self.__dict__ == other.__dict__

    def __neq__(self, other: SessionInfo) -> bool:
        """Negation of :meth:`__eq__`.

        .. note::
            Python uses ``__ne__``, not ``__neq__``: this method is never called implicitly
            (``!=`` already falls back to the negation of ``__eq__``).

        Args:
            other (``SessionInfo``): object to compare with.

        Returns:
            ``bool``:
            ``True`` if at least one attribute differs.
        """
        return not self.__eq__(other)

    def save(self, filename: str) -> bool:
        """Pickle the session information into a file.

        Args:
            filename (``str``): destination file.

        Returns:
            ``bool``:
            ``True`` on success.
        """

        # Open the file
        try:
            # FIXME: pickle requires a binary file ('wb'); in text mode pickle.dump raises a TypeError
            # (caught below, so save() always returns False).
            file = open(filename, "w")
        except:
            self.logger.error(
                "impossible to write the configuration file '" + filename + "'"
            )
            return False

        # Dump data
        import pickle

        try:
            pickle.dump(self, file)
            test = True
        except:
            self.logger.error("error occured during saving data to " + filename)
            test = False

        # Close the file
        file.close()

        # Return the operation status
        return test

    def load(self, filename: str) -> bool:
        """Load the session information from a pickle file.

        Only the attributes existing in the current object are copied.

        Args:
            filename (``str``): pickle file.

        Returns:
            ``bool``:
            ``True`` on success.
        """

        # Open the file
        try:
            # FIXME: pickle requires a binary file ('rb').
            file = open(filename, "r")
        except:
            self.logger.error(
                "impossible to read the configuration file '" + filename + "'"
            )
            return False

        # Import data
        import pickle

        try:
            newone = pickle.load(file)
            test = True
        except:
            self.logger.warning("error occured during reading data from " + filename)
            test = False

        # Close the file
        file.close()

        if not test:
            return False

        # Fill the class variables
        import copy

        try:
            for item in self.__dict__:
                self.__dict__[item] = copy.copy(newone.__dict__[item])
        except:
            self.logger.error("error occured during copying data from " + filename)
            test = False

        # Return the operation status
        return test
