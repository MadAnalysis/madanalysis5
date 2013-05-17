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
class HTMLCSSWriter(TextFileWriter.TextFileWriter):
    """Generate HTML CSS page sheet"""

    def __init__(self, filename):
        TextFileWriter.TextFileWriter.__init__(self,filename)
        self.page=[]

    def WriteCSSLinks(self):
        self.page.append('a:link {text-decoration: none; color:#424242; ' + \
           'background-color: #FFFFFF; font-size:11px;}\n')
        self.page.append('a:visited {text-decoration: none; color:#424242; ' + \
           'background-color: #FFFFFF; font-size:11px;}\n')
        self.page.append('a:hover {font-weight:bold; font-size:11px;}\n\n')


    def WriteCSSbody(self):
        self.page.append('body  { width: 800px; height: 600px; margin: 10px auto; ' + \
           'color: #000000; font-family: \'Verdana\',Serif; font-size: 12px; ' + \
           'background-color: #C0C0C0; }\n\n')

    def WriteCSStop(self):
        self.page.append('#top { height: 83px; font-family :\'Verdana\',Serif; ' + \
           'font-size: 11px; margin: auto; ' + \
           'background-color: #FFFFFF; text-align:center;}\n')
        self.page.append('.top table { width: 800px; height: 71px; ' + \
           'border: medium solid #000000; border-collapse: collapse; border-spacing: 1px}\n')
        self.page.append('.top td { font-family :\'Verdana\',Serif; font-size: 12px; ' + \
           'vertical-align: center; text-align: center; border: medium solid #000000; ' + \
           'border-collapse: collapse; }\n')
        self.page.append('.top h1 { text-align: top ; margin: auto;  font-size: 32px; ' + \
           'font-family: \'Verdana\', Serif; font-weight:bold; }\n\n')

    def WriteCSSmenu(self):
        self.page.append('#menu { float:left; background-color : #FFFFFF; color: #000000; ' + \
          'width: 150px; height: 600px; font-family: \'Verdana\',Serif; font-size: 12px; ' + \
          'margin: auto; padding: 0 0 0 10px ; }\n')
        self.page.append('.menu h3 {  margin-top:2px;  margin-bottom: 2px; ' + \
          'border-bottom:2px groove #263d65; border-top:2px groove #263d65; font-size: 14px; ' + \
          'font-family: \'Verdana\', Serif; text-align: center; width : 90%; }\n')
        self.page.append('.menu ul { list-style-type: square; }\n')
        self.page.append('.menu li { margin:0 0 0 -15px; padding: 0; }\n\n')

    def WriteCSSmain(self):
        self.page.append('#main { float: right; margin: auto; width: 640px; height: 600px; ' + \
          'padding: 0; background-color: #FFFFFF; color: #000000; ' + \
          'font-family: \'Verdana\',Serif; font-size: 12px; overflow: auto; }\n')
        self.page.append('.main h2 { text-align: center; width: 250px; margin: auto; ' + \
          'border-bottom:2px groove #263d65; border-top:2px groove #263d65; font-size: 24px; ' + \
          'font-family: \'Verdana\',Serif; font-weight:bold; }\n')
        self.page.append('.main h3 { text-align: left; margin: auto;  font-size: 14px; ' + \
          'font-family: \'Verdana\',Serif; font-weight:bold; }\n')
        self.page.append('.main h3 a:hover { text-align: left; margin: auto; ' + \
           'font-size: 14px; font-family: \'Verdana\',Serif; font-weight:bold; }\n')
        self.page.append('.main ul { list-style-type: square; }\n')
        self.page.append('.main li { margin:0 0 0 -15px; padding: 0; }\n')
        self.page.append('.main img { align=\'center\'; width: 620px; }\n')
        self.page.append('.main table { width: 620px; border: medium solid #000000; ' + \
           'border-collapse: collapse; border-spacing: 1px}\n')
        self.page.append('.main td { font-family :\'Verdana\',Serif; font-size: 12px; ' + \
           'vertical-align: center; text-align: center; border: thin solid #000000; ' + \
           'border-collapse: collapse; }\n\n')



    def WriteCSS(self):
        for item in self.page:
            self.file.write(item)

