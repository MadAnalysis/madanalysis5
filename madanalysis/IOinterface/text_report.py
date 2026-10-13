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


"""Formatted text used in the HTML and LaTeX reports."""

from __future__ import absolute_import
from __future__ import annotations
from typing import IO, Any
from madanalysis.enumeration.color_type import ColorType
from madanalysis.enumeration.font_type import FontType
from madanalysis.enumeration.script_type import ScriptType
import logging
import six

class TextReport():
    """Sequence of formatted text chunks and line breaks.

    The current font, colour and script (sub/superscript) apply to the text added with
    :meth:`Add`.

    Attributes:
        table (``list``): :class:`FormattedText` and :class:`NewLine` objects.
        font / color / script (``int``): current style (enumeration codes).
    """

    class FormattedText():
        """Text chunk with a font, a colour and a script.

        Attributes:
            dicolatex (``dict[str, str]``): escaping of the LaTeX special characters.
        """
        
        dicolatex = {'#':'\#','%':'\%','_':'\_','/':'/\-', \
                     '{':'\{', '}':'\}','^':'\^{}'}
            
        def __init__(self,text: str,font: int,color: int,script: int) -> None:
            """Create a text chunk.

            Args:
                text (``str``): text.
                font (``int``): :class:`~madanalysis.enumeration.font_type.FontType` code.
                color (``int``): :class:`~madanalysis.enumeration.color_type.ColorType` code.
                script (``int``): :class:`~madanalysis.enumeration.script_type.ScriptType` code.
            """
            self.text   = text
            self.font   = font
            self.color  = color
            self.script = script
        
        def ReplaceAll(self,text: str,dic: dict[str, str]) -> str:
            """Apply several substring replacements.

            Args:
                text (``str``): input text.
                dic (``dict[str, str]``): replacements ``{old: new}``.

            Returns:
                ``str``:
                The modified text.
            """
            word = text
            for i,j in six.iteritems(dic):
                word = word.replace(i,j)
            return word

        def WriteHTML(self,file: list[str]) -> None:
            """Append the HTML representation of the chunk.

            Args:
                file (``list[str]``): HTML page being built (list of strings).
            """
            if self.text=='':
                return
            file.append(ScriptType.htmlscript(self.script))
            file.append(FontType.convert2html(self.font))
            # NOTE: 2 is the code of BLACK in ColorType (magic number).
            if self.color!=2:
                file.append('<font color=\'' + ColorType.convert2hexa(self.color)+'\'>')
                file.append(self.text+"</font>")
            else:
                file.append(self.text)
            file.append(FontType.convert2htmlclose(self.font))
            file.append(ScriptType.htmlscriptclose(self.script))

        def WriteLATEX(self,file: IO[str]) -> None:
            """Write the LaTeX representation of the chunk (special characters escaped).

            Args:
                file (``IO[str]``): LaTeX file.
            """
            if self.text.find('ma5>')!=-1:
                self.text = self.text + '\\\\\n'
            file.write(ScriptType.latexscript(self.script))
            file.write(FontType.convert2latex(self.font))
            if ColorType.convert2string(self.color) == 'black':
                if not 'Plot' in self.text and not 'ma5>' in self.text and not 'Cut' in self.text:
                    file.write(' ' + self.ReplaceAll(self.text,TextReport.FormattedText.dicolatex))
                else:
                    file.write(self.ReplaceAll(self.text,TextReport.FormattedText.dicolatex))
            else:
                file.write("\\textcolor{"+\
                       ColorType.convert2string(self.color)+"}{")
                file.write(self.ReplaceAll(self.text,\
                       TextReport.FormattedText.dicolatex)+"}")
            file.write(FontType.convert2latexclose(self.font))
            file.write(ScriptType.latexscriptclose(self.script))

    class NewLine():
        """Line break."""

        @staticmethod
        def WriteHTML(file: list[str]) -> None:
            """Append an HTML line break.

            Args:
                file (``list[str]``): HTML page being built.
            """
            file.append('<br />\n')

        @staticmethod
        def WriteLATEX(file: IO[str]) -> None:
            """Write a new line in the LaTeX file.

            Args:
                file (``IO[str]``): LaTeX file.
            """
            file.write('\n')

    def __init__(self) -> None:
        """Create an empty text with the normal style."""
        self.Reset()

    def SetNormal(self) -> None:
        """Reset the style (normal font, black, no script)."""
        self.font = FontType.none
        self.color = ColorType.BLACK
        self.script = ScriptType.none

    def SetFont(self,font: int) -> None:
        """Set the font of the next chunks.

        Args:
            font (``int``): :class:`~madanalysis.enumeration.font_type.FontType` code.
        """
        self.font = font

    def SetColor(self,color: int) -> None:
        """Set the colour of the next chunks.

        Args:
            color (``int``): :class:`~madanalysis.enumeration.color_type.ColorType` code.
        """
        self.color = color

    def SetScript(self,script: int) -> None:
        """Set the script of the next chunks.

        Args:
            script (``int``): :class:`~madanalysis.enumeration.script_type.ScriptType` code.
        """
        self.script = script

    def Add(self,text: str) -> None:
        r"""Add text with the current style (``\n`` become line breaks).

        Args:
            text (``str``): text to add.
        """
        lines = text.split("\n")
        for item in lines:
            self.table.append(TextReport.FormattedText(item,self.font,self.color,self.script))
            # NOTE: compares values, not positions: repeated identical lines are not followed by a line break.
            if item!=lines[-1]:
                self.table.append(TextReport.NewLine())

    def Reset(self) -> None:
        """Remove all chunks and reset the style."""
        self.table=[]
        self.SetNormal()

    def WriteHTML(self,file: list[str]) -> None:
        """Append the HTML representation of all chunks.

        Args:
            file (``list[str]``): HTML page being built.
        """
        for item in self.table:
            item.WriteHTML(file)

    def WriteLATEX(self,file: IO[str]) -> None:
        """Write the LaTeX representation of all chunks.

        Args:
            file (``IO[str]``): LaTeX file.
        """
        for item in self.table:
            item.WriteLATEX(file)

    def IsThereNewLine(self) -> bool:
        """Check whether the text contains a line break.

        Returns:
            ``bool``:
            ``True`` if at least one :class:`NewLine` is present.
        """
        for item in self.table:
            if type(item)==type(TextReport.NewLine()):
                return True
        return False
