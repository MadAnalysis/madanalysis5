################################################################################
#  
#  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <http://madanalysis.irmp.ucl.ac.be>
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


from madanalysis.enumeration.ma5_running_type import MA5RunningType
class ObservableBase():
    def __init__(self,
                 name,        args,       combination,
                 plot_auto,   plot_nbins, plot_xmin,
                 plot_xmax,   plot_unitX, code_parton,
                 code_hadron, code_reco,  cut_event, cut_candidate, tex):
        self.name          = name
        self.args          = args
        self.plot_auto     = plot_auto
        self.plot_nbins    = plot_nbins
        self.plot_xmin     = plot_xmin
        self.plot_xmax     = plot_xmax
        self.plot_unitX    = plot_unitX
        self.code_parton   = code_parton
        self.code_hadron   = code_hadron
        self.code_reco     = code_reco
        self.cut_event     = cut_event
        self.cut_candidate = cut_candidate
        self.combination   = combination
        self.tex          = tex


    def code(self,level):
        if level==MA5RunningType.PARTON:
            return self.code_parton
        elif level==MA5RunningType.HADRON:
            return self.code_hadron
        elif level==MA5RunningType.RECO:
            return self.code_reco
        else:
            return None
    

    @staticmethod
    def Clone(obs,
              name=None,        args=None,        combination=None,
              plot_auto=None,   plot_nbins=None, plot_xmin=None,
              plot_xmax=None,   plot_unitX=None, code_parton=None,
              code_hadron=None, code_reco=None,  cut_event=None,     cut_candidate=None, tex=None):

        # create clone of obs
        newobs = ObservableBase(obs.name,        obs.args,        obs.combination,
                                obs.plot_auto,   obs.plot_nbins,  obs.plot_xmin,
                                obs.plot_xmax,   obs.plot_unitX,  obs.code_parton,
                                obs.code_hadron, obs.code_reco,   obs.cut_event,   obs.cut_candidate, obs.tex)

        # replace
        if name!=None:
            newobs.name=name
        if args!=None:
            newobs.args=args
        if combination!=None:
            newobs.combination=combination
        if plot_auto!=None:
            newobs.plot_auto=plot_auto
        if plot_nbins!=None:
            newobs.plot_nbins=plot_nbins
        if plot_xmin!=None:
            newobs.plot_xmin=plot_xmin
        if plot_xmax!=None:
            newobs.plot_xmax=plot_xmax
        if plot_unitX!=None:
            newobs.plot_unitX=plot_unitX
        if code_parton!=None:
            newobs.code_parton=codeparton
        if code_hadron!=None:
            newobs.code_hadron=code_hadron
        if code_reco!=None:
            newobs.code_reco=code_reco
        if cut_event!=None:
            newobs.cut_event=cut_event
        if cut_candidate!=None:
            newobs.cut_candidate=cut_candidate
        if tex!=None:
            newobs.tex=tex

        # return the clone
        return newobs
         
