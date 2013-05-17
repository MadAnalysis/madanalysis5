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


import madanalysis.IOinterface.text_file_writer as TextFileWriter
from madanalysis.IOinterface.html_style_writer   import HTMLCSSWriter
from madanalysis.enumeration.color_type import ColorType
from madanalysis.enumeration.font_type import FontType
from madanalysis.enumeration.script_type import ScriptType
from madanalysis.IOinterface.text_report import TextReport

import os
import logging
import time
import pwd

class HTMLReportWriter(TextFileWriter.TextFileWriter):
    """Generate HTML report"""
    
    def __init__(self, filename, pdffile=''):
        TextFileWriter.TextFileWriter.__init__(self,filename)
        self.page=[]
        self.section=[]
        self.sectionLevel=[]
        self.bullet=0
        self.table=0
        self.current_col=0
        self.number_col=0
        self.col_size=[]
        self.first_cell=True
        self.pdffile=pdffile
        self.style = HTMLCSSWriter(filename.replace('index.html','style.css'))
        if not self.style.Open():
            logging.info('HTML style file already open')
            return
        self.style.WriteCSSLinks()
        self.style.WriteCSSbody()
        self.style.WriteCSStop()
        self.style.WriteCSSmenu()
        self.style.WriteCSSmain()
        self.style.WriteCSS()
        self.style.Close()

    @staticmethod    
    def CheckStructure(dirname):
        if not os.path.isdir(dirname):
            return False
        if not os.path.isfile(dirname+'/index.html'):
            return False
        if not os.path.isfile(dirname+'/style.css'):
            return False
        if not os.path.isfile(dirname+'/logo.png'):
            return False
        return True

    def WriteHeader(self):
        self.page.append('<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" '+ \
          '\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n')
        self.page.append('<html xmlns=\'http://www.w3.org/1999/xhtml\' xml:lang=\'en\' ' + \
          'lang=\'en\'>\n\n')
        self.page.append('<head>\n')
        self.page.append('  <title>MadAnalysis 5 HTML report</title>\n')
        self.page.append('  <link rel=\'stylesheet\' media=\'screen\' type=\'text/css\' ' + \
          'title=\'style\' href=\'style.css\' />\n')
        self.page.append('<meta http-equiv=\'Content-Type\' '+ \
           'content=\'text/html;charset=utf-8\'/>\n')
        self.page.append('</head>\n\n')
        self.page.append('<body>\n')
    
    def WriteTitle(self,title):
        self.page.append('<div id=\'top\' class=\'top\'>\n')
        self.page.append('  <table>\n')
        self.page.append('    <tr>\n      <td width=\'35%\'>\n')
        self.page.append('        <img style=\'width: 182px; height: 53px;\' ' + \
          'alt=\'ma5-logo\' src=\'logo.png\' />')
        self.page.append('<br />\n        <a href=\'http://madanalysis.irmp.ucl.ac.be\'>' + \
          'Please visit us.</a>\n')
        self.page.append('      </td>\n      <td width=\'65%\'>\n')
        self.page.append('        <h1>' + title +'</h1><br />\n')
        try:
            mydate = str(time.strftime("%d %B %Y, %H:%M:%S"))
        except:
            mydate = 'date/hour not accessible'
        try:
            mylogin = pwd.getpwuid(os.getuid())[0]
        except:
            mylogin = 'unknown' 
        self.page.append('        <i>Created by <font color=\'#0000CC\'>' + \
          mylogin + '</font> on  <font color=\'#0000CC\'>' + \
          mydate + '</font></i>\n')
        self.page.append('      </td>\n    </tr>\n  </table>\n</div>\n\n')
 
    def TableOfContents(self):
        contents = '<div id=\'menu\' class= \'menu\'>\n  <br />\n';
        if self.pdffile != '':
            contents += '  <h3>PDF version of this report</h3>\n'
            contents += '  <ul><li><a href=\''+ self.pdffile + '\'>' + 'Download here</a>' + \
             '</li></ul>\n'
        for i in range(len(self.section)):
            if self.sectionLevel[i]==1:
                if i!=0 and self.sectionLevel[i-1]==2:
                    contents +='  </ul>\n'
                contents+='  <h3>'+self.section[i]+'</h3>\n'
                if i!=(len(self.section)-1) and self.sectionLevel[i+1]==2:
                    contents +='  <ul>\n'
            elif self.sectionLevel[i]==2:
                contents+='    <li><a href=\'#' + self.section[i].replace(' ','') + '\'>' + \
                   self.section[i] + '</a></li>\n'
                if i==(len(self.section)-1):
                    contents +='  </ul>\n'
        contents += '</div>\n\n'
        contents += '<div id=\'main\' class=\'main\'>\n'
        return contents
    
    def WriteSpacor(self):
        self.page.append("<HR SIZE=\"4\" WIDTH=350 noshade>\n")

    def WriteVspace(self):
        self.page.append("<SPACER TYPE=vertical>")

    def WriteSubTitle(self,subtitle):
        self.section.append(subtitle)
        self.sectionLevel.append(1)
        self.page.append('  <br /><br />\n\n  <h2>'+ subtitle + '</h2><br />\n')

    def WriteSubSubTitle(self,subtitle):
        self.section.append(subtitle)
        self.sectionLevel.append(2)
        self.page.append('  <h3><a name=\'' + subtitle.replace(' ', '') + '\'>' + \
          subtitle+'</a></h3><br />\n')
        
    def WriteText(self,text):
         if self.bullet!=0:
            self.page.append('    <li>\n      ')
         text.WriteHTML(self.page)
         if self.bullet!=0:
            self.page.append('    </li>\n')
         else:
            self.page.append('  <br />\n')

    def NewLine(self):
        self.page.append("<BR>")

    def OpenBullet(self):
        self.bullet=self.bullet+1
        self.page[-1]=self.page[-1].replace('</h3><br />','</h3>')
        self.page.append("  <ul>\n")

    def CloseBullet(self):
        self.bullet=self.bullet-1
        self.page.append("  </ul>\n")
       
    def CreateTable(self,col,caption):
        self.table=self.table+1
        self.number_col=len(col)
        self.col_size=col
        self.page.append("  <table>\n")
        if caption.table != []:
            self.page.append("    <caption align=\'bottom\'>\n      ")
            self.WriteText(caption)
            self.page.append("    </caption>\n")
        self.page.append('    <tr>\n')

    def NewCell(self,color=ColorType.WHITE):
        size = str(round(self.col_size[self.current_col]/sum(self.col_size)*100,0))
        self.current_col=self.current_col+1
        
        if  self.current_col>self.number_col:
            logging.warning(" the number of the current column is bigger than the total number of declared columns.")
        if self.first_cell==True:
            self.page.append('      <td width=\'' + size + '%\' bgcolor=\'' + \
                ColorType.convert2hexa(color)+ '\'>\n')
        else:
            self.page.append('      </td> \n' + \
                '      <td width=\'' + size + '%\' bgcolor=\'' + \
                ColorType.convert2hexa(color)+'\'>\n')
        self.first_cell=False
       
    def NewLine(self):
        self.current_col=0
        self.first_cell=True
        self.page.append("      </td>\n    </tr>\n    <tr>\n")
        
    def EndLine(self):
        self.current_col=0
        self.first_cell=True
        self.page.append("      </td>\n    </tr>\n")
        
    def EndTable(self):
        self.table=self.table-1
        self.page.append("  </table><br /> <br />\n")

    

    def WriteFigure(self,caption,filename):
        thefile = os.path.normpath(filename)
        from ROOT import TImage
        im = TImage.Open(thefile+".png",TImage.kPng)
        if not im.IsValid():
            logging.warning(" the picture "+ thefile+".png does not exist.")
        if im.GetWidth()!=0:
            scale = 620./im.GetWidth()
        else:
            scale = 1.
        self.page.append("  <center>\n")
        self.page.append('    <img src=\'' + os.path.basename(filename) + \
                '.png\' ' + 'height=\''+ str(scale*im.GetHeight())+'\' alt =\'\' />\n')
        self.page.append("  </center><br /> <br />\n")
        
    
    def WriteFoot(self):
        if self.bullet!=0:
            logging.warning(" the number of 'OpenBullet()' and 'CloseBullet()' are different.")
        if self.table!=0:
            logging.warning("open table found. Please check for a missing 'EndTable()'.")
        self.page.append('  <br /><hr size=\'4\' width=\'350\' />\n')
        self.page.append("</div>\n")
        self.page.append("</body>\n")
        self.page.append("</html>\n")
        self.page.insert(17,self.TableOfContents())
        for item in self.page:
            self.file.write(item)
            
