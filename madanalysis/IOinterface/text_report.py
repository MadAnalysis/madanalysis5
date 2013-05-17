################################################################################
#  
#  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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


from madanalysis.enumeration.color_type import ColorType
from madanalysis.enumeration.font_type import FontType
from madanalysis.enumeration.script_type import ScriptType
import logging

class TextReport():

    class FormattedText():
        
        dicolatex = {'#':'\#','%':'\%','_':'\_','/':'/\-', \
                     '{':'\{', '}':'\}','^':'\^{}'}
            
        def __init__(self,text,font,color,script):
            self.text   = text
            self.font   = font
            self.color  = color
            self.script = script
        
        def ReplaceAll(self,text,dic):
            word = text
            for i,j in dic.iteritems():
                word = word.replace(i,j)
            return word
            
        def WriteHTML(self,file):
            if self.text=='':
                return
            file.append(ScriptType.htmlscript(self.script))
            file.append(FontType.convert2html(self.font))
            if self.color!=2:
                file.append('<font color=\'' + ColorType.convert2hexa(self.color)+'\'>')
                file.append(self.text+"</font>")
            else:
                file.append(self.text)
            file.append(FontType.convert2htmlclose(self.font))
            file.append(ScriptType.htmlscriptclose(self.script))

        def WriteLATEX(self,file):
            
            if self.text.find('ma5>')!=-1:
                self.text = self.text + '\\\\\n'
            file.write(ScriptType.latexscript(self.script))
            file.write(FontType.convert2latex(self.font))
            if ColorType.convert2string(self.color) == 'black':
                file.write(' ' + self.ReplaceAll(self.text,\
                       TextReport.FormattedText.dicolatex))
            else:
                file.write("\\textcolor{"+\
                       ColorType.convert2string(self.color)+"}{")
                file.write(self.ReplaceAll(self.text,\
                       TextReport.FormattedText.dicolatex)+"}")
            file.write(FontType.convert2latexclose(self.font))
            file.write(ScriptType.latexscriptclose(self.script))
    
    class NewLine():

        @staticmethod
        def WriteHTML(file):
            file.append('<br />\n')

        @staticmethod
        def WriteLATEX(file):
            file.write('\n')
        
    def __init__(self):
        self.Reset()
        
    def SetNormal(self):
        self.font = FontType.none
        self.color = ColorType.BLACK
        self.script = ScriptType.none

    def SetFont(self,font):
        self.font = font

    def SetColor(self,color):
        self.color = color
        
    def SetScript(self,script):
        self.script = script

    def Add(self,text):
        lines = text.split("\n")
        for item in lines:
            self.table.append(TextReport.FormattedText(item,self.font,self.color,self.script))
            if item!=lines[-1]:
                self.table.append(TextReport.NewLine())

    def Reset(self):
        self.table=[]
        self.SetNormal()

    def WriteHTML(self,file):
        for item in self.table:
            item.WriteHTML(file)

    def WriteLATEX(self,file):
        for item in self.table:
            item.WriteLATEX(file)

    def IsThereNewLine(self):
        for item in self.table:
            if type(item)==type(TextReport.NewLine()):
                return True
        return False
    

                            

        
        
    
        
