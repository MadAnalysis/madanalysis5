################################################################################
#  
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


from __future__ import absolute_import
from madanalysis.enumeration.linestyle_type import LineStyleType
from madanalysis.enumeration.backstyle_type import BackStyleType
from madanalysis.enumeration.color_type import ColorType
from madanalysis.dataset.sample_info import SampleInfo
from madanalysis.layout.layout import Layout
import logging

class Dataset:

    userVariables = { "type"        : ["signal","background"], \
                      "linecolor"   : ["auto","none","red","black","white","yellow",\
                                       "blue","grey","green","purple","cyan","orange"], \
                      "backcolor"   : ["auto","none","red","black","white","yellow",\
                                       "blue","grey","green","purple","cyan","orange"], \
                      "backstyle"    : ["solid","dotted","hline","dline","vline"], \
                      "linestyle"   : ["solid","dashed","dotted","dash-dotted"],\
                      "linewidth"   : [], \
                      "weight"      : [], \
                      "xsection"    : [], \
                      "scale_up_variation"   : [], \
                      "scale_down_variation" : [], \
                      "scale_variation"      : [], \
                      "pdf_up_variation"     : [], \
                      "pdf_down_variation"   : [], \
                      "pdf_variation"        : [], \
                      "title"       : [], \
                      "weighted_events": ["true","false"]}

    def __init__(self,name):
        self.name              = name.lower()
        self.weight            = 1.
        self.xsection          = 0.
        self.scaleup           = None
        self.scaledn           = None
        self.pdfup             = None
        self.pdfdn             = None
        self.background        = False
        self.linecolor         = ColorType.AUTO
        self.linestyle         = LineStyleType.SOLID
        self.lineshade         = 0
        self.linewidth         = 1
        self.backcolor         = ColorType.AUTO
        self.backstyle         = BackStyleType.SOLID
        self.backshade         = 0
        self.filenames         = []
        self.title             = name
        self.weighted_events   = True
        self.measured_global   = SampleInfo() 
        self.measured_detail   = []

    def __len__(self):
        return len(self.filenames)

    def __getitem__(self,i):
        return self.filenames[i]

    def user_GetValues(self,variable):
        try:
            return Dataset.userVariables[variable]
        except:
            return []

    def user_GetParameters(self):
        return list(Dataset.userVariables.keys())

    def user_SetParameter(self,variable,value,value2="",value3=""):
        #type
        if variable == "type":
            if value=="signal":
                self.background=False
            elif value=="background":
                self.background=True
            else:
                logging.getLogger('MA5').error("The possible values for the attribute 'type' "+\
                                               "are 'signal' and 'background'.")

        #weighted events
        elif variable == "weighted_events":
            if value=="true":
                self.weighted_events=True
            elif value=="false":
                self.weighted_events=False
            else:
                logging.getLogger('MA5').error("The possible values for the attribute "+\
                                               "'weighted_events' are 'true' and 'false'.")

        #linecolor        
        elif variable == "linecolor":
            # COLOR
            if value=="red":
                self.linecolor=ColorType.RED
            elif value=="none":
                self.linecolor=ColorType.NONE
            elif value=="black":
                self.linecolor=ColorType.BLACK
            elif value=="white":
                self.linecolor=ColorType.WHITE
            elif value=="yellow":
                self.linecolor=ColorType.YELLOW
            elif value=="blue":
                self.linecolor=ColorType.BLUE
            elif value=="grey":
                self.linecolor=ColorType.GREY
            elif value=="green":
                self.linecolor=ColorType.GREEN
            elif value=="purple":
                self.linecolor=ColorType.PURPLE
            elif value=="cyan":
                self.linecolor=ColorType.CYAN
            elif value=="orange":
                self.linecolor=ColorType.ORANGE
            elif value=="auto":
                self.linecolor=ColorType.AUTO
            else:
                logging.getLogger('MA5').error("the possible values for the attribute 'linecolor' are "+\
                                               "'auto', 'none', 'black', 'white', 'red', 'yellow'," + \
                                               "'blue', 'grey', 'purple', 'cyan', 'orange'.")
                return

            #SHADE
            if value3=="":
                self.lineshade=0
                return

            try:
                code=int(value3)
            except:
                logging.getLogger('MA5').error("the parameter '"+value3+"' is not an integer")
                return
            
            if value2=='+':
                self.lineshade=code
            elif value2=='-':
                self.lineshade=code*-1
            else:
                logging.getLogger('MA5').error("the parameter '"+value2+"' is not '+' or '-'")
                return

        #linestyle
        elif variable == "linestyle":
            if value=="solid":
                self.linestyle=LineStyleType.SOLID
            elif value=="dashed":
                self.linestyle=LineStyleType.DASHED
            elif value=="dotted":
                self.linestyle=LineStyleType.DOTTED
            elif value=="dash-dotted":
                self.linestyle=LineStyleType.DASHDOTTED
            else:
                logging.getLogger('MA5').error("the possible values for the attribute 'linestyle' are "+\
                                               "'solid', 'dashed', 'dotted', 'dash-dotted'.")
                
        #linewidth
        elif variable == "linewidth":
            try:
                code=int(value)
            except:
                logging.getLogger('MA5').error("the parameter '"+value+"' is not an integer value")
                return

            if code>0 and code<10:
                self.linewidth=code
            else:
                logging.getLogger('MA5').error("the parameter '"+value+"' must be > 0 and < 11")

        #backstyle
        elif variable == "backstyle":
            if value=="solid":
                self.backstyle=BackStyleType.SOLID
            elif value=="dotted":
                self.backstyle=BackStyleType.DOTTED
            elif value=="hline":
                self.backstyle=BackStyleType.HLINE
            elif value=="dline":
                self.backstyle=BackStyleType.DLINE
            elif value=="vline":
                self.backstyle=BackStyleType.VLINE
            else:
                logging.getLogger('MA5').error("the possible values for the attribute 'backstyle' are "+\
                                               "'solid', 'dotted', 'hline', 'dline', 'vline'.")

        #backcolor        
        elif variable == "backcolor":
            if value=="red":
                self.backcolor=ColorType.RED
            elif value=="black":
                self.backcolor=ColorType.BLACK
            elif value=="none":
                self.backcolor=ColorType.NONE
            elif value=="white":
                self.backcolor=ColorType.WHITE
            elif value=="yellow":
                self.backcolor=ColorType.YELLOW
            elif value=="blue":
                self.backcolor=ColorType.BLUE
            elif value=="grey":
                self.backcolor=ColorType.GREY
            elif value=="green":
                self.backcolor=ColorType.GREEN
            elif value=="purple":
                self.backcolor=ColorType.PURPLE
            elif value=="cyan":
                self.backcolor=ColorType.CYAN
            elif value=="orange":
                self.backcolor=ColorType.ORANGE
            elif value=="auto":
                self.backcolor=ColorType.AUTO
            else:
                logging.getLogger('MA5').error("the possible value for the attribute 'backcolor' are "+\
                                               "'auto', 'none', 'black', 'white', 'red', 'yellow'," + \
                                               "'blue', 'grey', 'purple', 'cyan', 'orange'.")
                return
               
            #SHADE
            if value3=="":
                self.backshade=0
                return

            try:
                code=int(value3)
            except:
                logging.getLogger('MA5').error("the parameter '"+value3+"' is not an integer")
                return
            
            if value2=='+':
                self.backshade=code
            elif value2=='-':
                self.backshade=code*-1
            else:
                logging.getLogger('MA5').error("the parameter '"+value2+"' is not '+' or '-'")
                return

        #weight
        elif variable in ["weight", "xsection"]:
            try:
                tmp = float(value)
            except:
                logging.getLogger('MA5').error("the value of the attribute '"+variable+\
                                               "' must be set to a positive floating number.")
                return
            if tmp>=0:
                if variable == "weight":
                    self.weight=tmp
                elif variable == "xsection":
                    self.xsection=tmp
                    self.measured_global.xsection = tmp
            else:
                logging.getLogger('MA5').error("the value of the attribute '"+variable+\
                                               "' must be set to a positive floating number.")
                return

        # uncertainties
        elif variable in ["scale_up_variation","scale_down_variation",\
                          "pdf_up_variation","pdf_down_variation",\
                          "scale_variation", "pdf_variation"]:
            try:
                tmp = float(value)
            except:
                logging.getLogger('MA5').error("the value of the attribute '"+variable+\
                                               "' must be set to a positive floating-point number")
                return
            if tmp>=0:
                if variable == "scale_up_variation":
                    self.scaleup = tmp
                    if self.scaledn == None:
                        self.scaledn = .0
                elif variable == "scale_down_variation":
                    self.scaledn = tmp
                    if self.scaleup == None:
                        self.scaleup = 0.
                elif variable == "pdf_up_variation":
                    self.pdfup = tmp
                    if self.pdfdn == None:
                        self.pdfdn = 0.
                elif variable == "pdf_down_variation":
                    self.pdfdn = tmp
                    if self.pdfup == None:
                        self.pdfup = 0.
                elif variable == "scale_variation":
                    self.scaledn = tmp
                    self.scaleup = tmp
                elif variable == "pdf_variation":
                    self.pdfdn = tmp
                    self.pdfup = tmp
            else:
                logging.getLogger('MA5').error("the value of the attribute '"+variable+\
                                               "' must be set to a positive floating-point number")
                return

        # title
        elif variable == "title":
            quoteTag = False
            if value[0] in ["'",'"'] and value[-1] in ["'",'"']:
                value=value[1:-1]
                self.title=value 
            else:
                logging.getLogger('MA5').error("the value of the attribute '"+variable+\
                              "' must be set to a string.")
                
        # other    
        else:
            logging.getLogger('MA5').error("the class dataset has no attribute denoted by '"+variable+"'.")
            

    def Display(self):
        logging.getLogger('MA5').info("   ******************************************" )
        logging.getLogger('MA5').info("   Name of the dataset = " + self.name + " (" + self.GetStringTag() + ")")
        self.user_DisplayParameter("title")
        self.user_DisplayParameter("xsection")
        self.user_DisplayParameter("scale_unc")
        self.user_DisplayParameter("PDF_unc")
        self.user_DisplayParameter("weight")
        self.user_DisplayParameter("weighted_events")
        self.user_DisplayParameter("linecolor")
        self.user_DisplayParameter("linestyle")
        self.user_DisplayParameter("linewidth")
        self.user_DisplayParameter("backcolor")
        self.user_DisplayParameter("backstyle")
        logging.getLogger('MA5').info("   List of event files included in this dataset:")
        for item in self.filenames:
            logging.getLogger('MA5').info("    - " + item) 
        logging.getLogger('MA5').info("   ******************************************" )
        msg = "   Cross section = "
        if self.measured_global.xsection==0 or self.measured_global.xerror!=0:
            msg+="("
        msg += Layout.DisplayXsection(self.measured_global.xsection,self.measured_global.xerror)
        if self.measured_global.xsection==0 or self.measured_global.xerror!=0:
            msg+=")"
        msg += " pb"
        logging.getLogger('MA5').info(msg)
        logging.getLogger('MA5').info("   Total number of events = " + str(self.measured_global.nevents))
        if (self.measured_global.sumw_positive + self.measured_global.sumw_negative)==0:
            msg='0.0'
        else:
            msg=str(Layout.Round_to_Ndigits(100.*self.measured_global.sumw_negative / \
                   (self.measured_global.sumw_positive + self.measured_global.sumw_negative ),2 ))
        logging.getLogger('MA5').info("   Ratio of negative weights = " + str(msg) + ' %')
        logging.getLogger('MA5').info("   ******************************************" )


    def user_DisplayParameter(self,parameter):
        if parameter=="weight":
            logging.getLogger('MA5').info("   User-imposed weight value for the set = "+str(self.weight))
        elif parameter=="xsection":
            logging.getLogger('MA5').info("   User-imposed cross section = "+str(self.xsection))
        elif parameter=="scale_unc":
            if not self.scaleup == None and not self.scaledn == None:
                logging.getLogger('MA5').info("   User-imposed scale uncertainties: "+\
                                              "-{:.1%}, +{:.1%}".format(self.scaledn,self.scaleup))
        elif parameter=="PDF_unc":
            if not self.pdfup == None and not self.pdfdn == None:
                logging.getLogger('MA5').info("   User-imposed PDF uncertainties: "+\
                                              "-{:.1%}, +{:.1%}".format(self.pdfdn,self.pdfup))
        elif parameter=="type":
            logging.getLogger('MA5').info("   Type = "+self.GetStringTag())
        elif parameter=="weighted_events":
            if self.weighted_events:
                logging.getLogger('MA5').info("   Taking account of event weight: true")
            else:
                logging.getLogger('MA5').info("   Taking account of event weight: false")
        elif parameter=="title":
            logging.getLogger('MA5').info("   Title = '"+self.title+"'")
        elif parameter=="linecolor":
            msg=ColorType.convert2string(self.linecolor)
            if self.lineshade!=0 and self.linecolor!=ColorType.AUTO:
                if self.lineshade>0:
                    msg+="+"+str(self.lineshade)
                else:
                    msg+=str(self.lineshade)
            logging.getLogger('MA5').info("   Line color in histograms = "+msg)
        elif parameter=="linestyle":
            logging.getLogger('MA5').info("   Line style in histograms = "+\
                                          LineStyleType.convert2string(self.linestyle))
        elif parameter=="linewidth":
            logging.getLogger('MA5').info("   Line width in histograms = "+str(self.linewidth))
        elif parameter=="backcolor":
            msg=ColorType.convert2string(self.backcolor)
            if self.backshade!=0 and self.backcolor!=ColorType.AUTO:
                if self.backshade>0:
                    msg+="+"+str(self.backshade)
                else:
                    msg+=str(self.backshade)
            logging.getLogger('MA5').info("   Background color in histograms = "+msg)
        elif parameter=="backstyle":
            logging.getLogger('MA5').info("   Background style in histograms = "+\
                                          BackStyleType.convert2string(self.backstyle))
        else:
            logging.getLogger('MA5').error(" the class dataset has no attribute denoted by '"+parameter+"'")

    def GetStringTag(self):
        if self.background:
            return "background"
        else:
            return "signal"

    def Find(self,file):
        if file in self.filenames:
            return True
        return False

    def Add(self,file):
        if not self.Find(file):
            self.filenames.append(file)
        
    def Remove(self,file):
        if self.Find(file):
            del self.filenames[file]

    def GetIds(self):
        return self.filenames
