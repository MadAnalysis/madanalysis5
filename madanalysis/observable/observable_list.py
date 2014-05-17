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


from madanalysis.enumeration.combination_type import CombinationType
from madanalysis.enumeration.argument_type    import ArgumentType
from madanalysis.observable.observable_base   import ObservableBase
import math


# Warning : all observable labels must be in uppercase

SQRTS = ObservableBase( name        = 'SQRTS',
                        args        = [],
                        combination = CombinationType.DEFAULT,
                        plot_auto   = False,
                        plot_nbins  = 100,
                        plot_xmin   = 0.,
                        plot_xmax   = 1000.,
                        plot_unitX  = 'GeV',
                        code_parton = 'PHYSICS->SqrtS(event.mc())',
                        code_hadron = 'PHYSICS->SqrtS(event.mc())',
                        code_reco   = '',
                        cut_event     = True,
                        cut_candidate = True,
                        tex           = '#sqrt{#hat{s}}'
                      )


ALPHAT = ObservableBase( name         = 'ALPHAT',
                        args          = [],
                        combination   = CombinationType.DEFAULT,
                        plot_auto     = False,
                        plot_nbins    = 100,
                        plot_xmin     = 0.,
                        plot_xmax     = 1.,
                        plot_unitX    = '',
                        code_parton   = 'PHYSICS->Transverse->AlphaT(event.mc())',
                        code_hadron   = 'PHYSICS->Transverse->AlphaT(event.mc())',
                        code_reco     = 'PHYSICS->Transverse->AlphaT(event.rec())',
                        cut_event     = True,
                        cut_candidate = True,
                        tex           = '#alpha_T'
                      )


TET = ObservableBase( name        = 'TET',
                      args        = [],
                      combination = CombinationType.DEFAULT,
                      plot_auto   = False,
                      plot_nbins  = 100,
                      plot_xmin   = 0.,
                      plot_xmax   = 1000.,
                      plot_unitX  = 'GeV',
                      code_parton = 'PHYSICS->Transverse->EventTET(event.mc())',
                      code_hadron = 'PHYSICS->Transverse->EventTET(event.mc())',
                      code_reco   = 'PHYSICS->Transverse->EventTET(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tex           = 'E_{T}'

                    )

MET = ObservableBase( name        = 'MET',
                      args        = [],
                      combination = CombinationType.DEFAULT,
                      plot_auto   = False,
                      plot_nbins  = 100,
                      plot_xmin   = 0.,
                      plot_xmax   = 1000.,
                      plot_unitX  = 'GeV',
                      code_parton = 'PHYSICS->Transverse->EventMET(event.mc())',
                      code_hadron = 'PHYSICS->Transverse->EventMET(event.mc())',
                      code_reco   = 'PHYSICS->Transverse->EventMET(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tex           = '#slash{E}_{T}'
                    )

THT = ObservableBase( name        = 'THT',
                      args        = [],
                      combination = CombinationType.DEFAULT,
                      plot_auto   = False,
                      plot_nbins  = 100,
                      plot_xmin   = 0.,
                      plot_xmax   = 1000.,
                      plot_unitX  = 'GeV',
                      code_parton = 'PHYSICS->Transverse->EventTHT(event.mc())',
                      code_hadron = 'PHYSICS->Transverse->EventTHT(event.mc())',
                      code_reco   = 'PHYSICS->Transverse->EventTHT(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tex           = 'H_{T}'
                    )

MHT = ObservableBase( name        = 'MHT',
                      args        = [],
                      combination = CombinationType.DEFAULT,
                      plot_auto   = False,
                      plot_nbins  = 100,
                      plot_xmin   = 0.,
                      plot_xmax   = 1000.,
                      plot_unitX  = 'GeV',
                      code_parton = 'PHYSICS->Transverse->EventMHT(event.mc())',
                      code_hadron = 'PHYSICS->Transverse->EventMHT(event.mc())',
                      code_reco   = 'PHYSICS->Transverse->EventMHT(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tex           = '#slash{H}_{T}'
                    )

NPID = ObservableBase( name        = 'NPID',
                       args        = [],
                       combination = CombinationType.DEFAULT,
                       plot_auto   = True,
                       plot_nbins  = 100,
                       plot_xmin   = 0.,
                       plot_xmax   = 100.,
                       plot_unitX  = '',
                       code_parton = 'NPID',
                       code_hadron = 'NPID',
                       code_reco   = 'NPID',
                       cut_event     = True,
                       cut_candidate = True,
                       tex = 'NPID'
                     )

NAPID = ObservableBase( name        = 'NAPID',
                        args        = [],
                        combination = CombinationType.DEFAULT,
                        plot_auto   = True,
                        plot_nbins  = 100,
                        plot_xmin   = 0.,
                        plot_xmax   = 100.,
                        plot_unitX  = '',
                        code_parton = 'NAPID',
                        code_hadron = 'NAPID',
                        code_reco   = 'NAPID',
                        cut_event     = True,
                        cut_candidate = True,
                        tex           = '|NPID|'
                      )

E = ObservableBase( name        = 'E',
                    args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = 0.,
                    plot_xmax   = 100.,
                    plot_unitX  = 'GeV',
                    code_parton = 'e()',
                    code_hadron = 'e()',
                    code_reco   = 'e()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = 'E'
                  )

vE  = E
sE  = ObservableBase.Clone(E, name='sE', combination=CombinationType.SUMSCALAR, tex='sE') 
sdE = dsE = ObservableBase.Clone(E, name='sdE', combination=CombinationType.DIFFSCALAR, tex='sdE') 
dE  = dvE = vdE = ObservableBase.Clone(E, name='dE', combination=CombinationType.DIFFVECTOR, tex='dE')
rE  = ObservableBase.Clone(E, name='rE', combination=CombinationType.RATIO, tex='rE')

M = ObservableBase( name        = 'M',
                    args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = 0.,
                    plot_xmax   = 1000.,
                    plot_unitX  = 'GeV/c^{2}',
                    code_parton = 'm()',
                    code_hadron = 'm()',
                    code_reco   = 'm()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex          = 'M'
                  )

vM  = M
sM  = ObservableBase.Clone(M, name='sM', combination=CombinationType.SUMSCALAR, tex='sM') 
sdM = dsM = ObservableBase.Clone(M, name='sdM', combination=CombinationType.DIFFSCALAR, tex='sdM') 
dM  = dvM = vdM = ObservableBase.Clone(M, name='dM', combination=CombinationType.DIFFVECTOR, tex='dM')
rM  = ObservableBase.Clone(M, name='rM', combination=CombinationType.RATIO, tex='rM')


P = ObservableBase( name        = 'P',
                    args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = 0.,
                    plot_xmax   = 1000.,
                    plot_unitX  = 'GeV/c',
                    code_parton = 'p()',
                    code_hadron = 'p()',
                    code_reco   = 'p()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = 'p'
                  )

vP  = P
sP  = ObservableBase.Clone(P, name='sP', combination=CombinationType.SUMSCALAR, tex='sp') 
sdP = dsP = ObservableBase.Clone(P, name='sdP', combination=CombinationType.DIFFSCALAR, tex='sdp') 
dP  = dvP = vdP = ObservableBase.Clone(P, name='dP', combination=CombinationType.DIFFVECTOR, tex='dp')
rP  = ObservableBase.Clone(P, name='rP', combination=CombinationType.RATIO, tex='rp')

ET = ObservableBase( name        = 'ET',
                     args        = [ArgumentType.COMBINATION],
                     combination = CombinationType.SUMVECTOR,
                     plot_auto   = False,
                     plot_nbins  = 100,
                     plot_xmin   = 0.,
                     plot_xmax   = 100.,
                     plot_unitX  = 'GeV',
                     code_parton = 'et()',
                     code_hadron = 'et()',
                     code_reco   = 'et()',
                     cut_event     = True,
                     cut_candidate = True,
                     tex           = 'E_{T}'
                  )

vET  = ET
sET  = ObservableBase.Clone(ET, name='sET', combination=CombinationType.SUMSCALAR, tex='sE_{T}') 
sdET = dsET = ObservableBase.Clone(ET, name='sdET', combination=CombinationType.DIFFSCALAR, tex='dsE_{T}') 
dET  = dvET = vdET = ObservableBase.Clone(ET, name='dET', combination=CombinationType.DIFFVECTOR, tex='dE_{T}')
rET  = ObservableBase.Clone(ET, name='rET', combination=CombinationType.RATIO, tex='rE_{T}')

MT = ObservableBase( name        = 'MT',
                     args        = [ArgumentType.COMBINATION],
                     combination = CombinationType.SUMVECTOR,
                     plot_auto   = False,
                     plot_nbins  = 100,
                     plot_xmin   = 0.,
                     plot_xmax   = 1000.,
                     plot_unitX  = 'GeV/c^{2}',
                     code_parton = 'mt()',
                     code_hadron = 'mt()',
                     code_reco   = 'mt()',
                     cut_event     = True,
                     cut_candidate = True,
                     tex           = 'M_{T}'
                  )

vMT  = MT
sMT  = ObservableBase.Clone(MT, name='sMT', combination=CombinationType.SUMSCALAR, tex='sM_{T}') 
sdMT = dsMT = ObservableBase.Clone(MT, name='sdMT', combination=CombinationType.DIFFSCALAR, tex='sdM_{T}') 
dMT  = dvMT = vdMT = ObservableBase.Clone(MT, name='dMT', combination=CombinationType.DIFFVECTOR, tex='dM_{T}')
rMT  = ObservableBase.Clone(MT, name='rMT', combination=CombinationType.RATIO, tex='rM_{T}')


MT_MET = ObservableBase( name        = 'MT_MET',
                     args        = [ArgumentType.COMBINATION],
                     combination = CombinationType.SUMVECTOR,
                     plot_auto   = False,
                     plot_nbins  = 100,
                     plot_xmin   = 0.,
                     plot_xmax   = 1000.,
                     plot_unitX  = 'GeV/c^{2}',
                     code_parton = 'mt_met(event.mc()->MET().momentum())',
                     code_hadron = 'mt_met(event.mc()->MET().momentum())',
                     code_reco   = 'mt_met(event.rec()->MET().momentum())',
                     cut_event     = True,
                     cut_candidate = True,
                     tex           = 'M_{T}'
                  )

vMT_MET  = MT_MET
sMT_MET  = ObservableBase.Clone(MT_MET, name='sMT_MET', combination=CombinationType.SUMSCALAR, tex='sM_{T}') 
sdMT_MET = dsMT_MET = ObservableBase.Clone(MT_MET, name='sdMT_MET', combination=CombinationType.DIFFSCALAR, tex='sdM_{T}') 
dMT_MET  = dvMT_MET = vdMT_MET = ObservableBase.Clone(MT_MET, name='dMT_MET', combination=CombinationType.DIFFVECTOR, tex='dM_{T}')
rMT_MET  = ObservableBase.Clone(MT_MET, name='rMT_MET', combination=CombinationType.RATIO, tex='rM_{T}')

PT = ObservableBase( name        = 'PT',
                     args        = [ArgumentType.COMBINATION],
                     combination = CombinationType.SUMVECTOR,
                     plot_auto   = False,
                     plot_nbins  = 100,
                     plot_xmin   = 0.,
                     plot_xmax   = 1000.,
                     plot_unitX  = 'GeV/c',
                     code_parton = 'pt()',
                     code_hadron = 'pt()',
                     code_reco   = 'pt()',
                     cut_event     = True,
                     cut_candidate = True,
                     tex           = 'p_{T}'
                  )

vPT  = PT
sPT  = ObservableBase.Clone(PT, name='sPT', combination=CombinationType.SUMSCALAR, tex='sp_{T}') 
sdPT = dsPT = ObservableBase.Clone(PT, name='sdPT', combination=CombinationType.DIFFSCALAR, tex='sdp_{T}') 
dPT  = dvPT = vdPT = ObservableBase.Clone(PT, name='dPT', combination=CombinationType.DIFFVECTOR, tex='dp_{T}')
rPT  = ObservableBase.Clone(PT, name='rPT', combination=CombinationType.RATIO, tex='rp_{T}')

PX = ObservableBase( name        = 'PX',
                     args        = [ArgumentType.COMBINATION],
                     combination = CombinationType.SUMVECTOR,
                     plot_auto   = False,
                     plot_nbins  = 100,
                     plot_xmin   = -1000.,
                     plot_xmax   = +1000.,
                     plot_unitX  = 'GeV/c',
                     code_parton = 'px()',
                     code_hadron = 'px()',
                     code_reco   = 'px()',
                     cut_event     = True,
                     cut_candidate = True,
                     tex           = 'p_{x}'
                  )

vPX  = PX
sPX  = ObservableBase.Clone(PX, name='sPX', combination=CombinationType.SUMSCALAR, tex='sp_{x}') 
sdPX = dsPX = ObservableBase.Clone(PX, name='sdPX', combination=CombinationType.DIFFSCALAR, tex='sdp_{x}') 
dPX  = dvPX = vdPX = ObservableBase.Clone(PX, name='dPX', combination=CombinationType.DIFFVECTOR, tex='dp_{x}')
rPX  = ObservableBase.Clone(PX, name='rPX', combination=CombinationType.RATIO, tex='rp_{x}')

PY = ObservableBase( name        = 'PY',
                     args        = [ArgumentType.COMBINATION],
                     combination = CombinationType.SUMVECTOR,
                     plot_auto   = False,
                     plot_nbins  = 100,
                     plot_xmin   = -1000.,
                     plot_xmax   = +1000.,
                     plot_unitX  = 'GeV/c',
                     code_parton = 'py()',
                     code_hadron = 'py()',
                     code_reco   = 'py()',
                     cut_event     = True,
                     cut_candidate = True,
                     tex           = 'p_{y}'
                  )

vPY  = PY
sPY  = ObservableBase.Clone(PY, name='sPY', combination=CombinationType.SUMSCALAR, tex='sp_{y}') 
sdPY = dsPY = ObservableBase.Clone(PY, name='sdPY', combination=CombinationType.DIFFSCALAR, tex='sdp_{y}') 
dPY  = dvPY = vdPY = ObservableBase.Clone(PY, name='dPY', combination=CombinationType.DIFFVECTOR, tex='dp_{y}')
rPY  = ObservableBase.Clone(PY, name='rPY', combination=CombinationType.RATIO, tex='rp_{y}')

PZ = ObservableBase( name        = 'PZ',
                     args        = [ArgumentType.COMBINATION],
                     combination = CombinationType.SUMVECTOR,
                     plot_auto   = False,
                     plot_nbins  = 100,
                     plot_xmin   = -1000.,
                     plot_xmax   = +1000.,
                     plot_unitX  = 'GeV/c',
                     code_parton = 'pz()',
                     code_hadron = 'pz()',
                     code_reco   = 'pz()',
                     cut_event     = True,
                     cut_candidate = True,
                     tex           = 'p_{z}'
                  )

vPZ  = PZ
sPZ  = ObservableBase.Clone(PZ, name='sPZ', combination=CombinationType.SUMSCALAR, tex='sp_{z}') 
sdPZ = dsPZ = ObservableBase.Clone(PZ, name='sdPZ', combination=CombinationType.DIFFSCALAR, tex='sdp_{z}') 
dPZ  = dvPZ = vdPZ = ObservableBase.Clone(PZ, name='dPZ', combination=CombinationType.DIFFVECTOR, tex='dp_{z}')
rPZ  = ObservableBase.Clone(PZ, name='rPZ', combination=CombinationType.RATIO, tex='rp_{z}')

R = ObservableBase( name        = 'R',
                    args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = 0.,
                    plot_xmax   = 1000.,
                    plot_unitX  = '',
                    code_parton = 'r()',
                    code_hadron = 'r()',
                    code_reco   = 'r()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = 'R'
                  )

vR  = R
sR  = ObservableBase.Clone(R, name='sR', combination=CombinationType.SUMSCALAR, tex='sR') 
sdR = dsR = ObservableBase.Clone(R, name='sdR', combination=CombinationType.DIFFSCALAR, tex='sdR') 
dR  = dvR = vdR = ObservableBase.Clone(R, name='dR', combination=CombinationType.DIFFVECTOR, tex='dR')
rR  = ObservableBase.Clone(R, name='rR', combination=CombinationType.RATIO, tex='rR')


DELTAR = ObservableBase( name        = 'DELTAR',
                         args        = [ArgumentType.COMBINATION,\
                                        ArgumentType.COMBINATION],
                         combination = CombinationType.SUMVECTOR,
                         plot_auto   = False,
                         plot_nbins  = 100,
                         plot_xmin   = 0.,
                         plot_xmax   = 6.28,
                         plot_unitX  = '',
                         code_parton = 'dr()',
                         code_hadron = 'dr()',
                         code_reco   = 'dr()',
                         cut_event     = True,
                         cut_candidate = True,
                         tex           = '#DeltaR'
                       )

vDELTAR  = DELTAR
dDELTAR  = dvDELTAR = vdDELTAR = ObservableBase.Clone(DELTAR, name='dDELTAR', combination=CombinationType.DIFFVECTOR, tex='d#DeltaR')

DPHI_0_PI = ObservableBase( name        = 'DPHI_0_PI',
                            args        = [ArgumentType.COMBINATION,\
                                           ArgumentType.COMBINATION],
                            combination = CombinationType.SUMVECTOR,
                            plot_auto   = False,
                           plot_nbins  = 100,
                           plot_xmin   = 0.,
                           plot_xmax   = 3.15,
                           plot_unitX  = '',
                           code_parton = 'dphi_0_pi()',
                           code_hadron = 'dphi_0_pi()',
                           code_reco   = 'dphi_0_pi()',
                         cut_event     = True,
                         cut_candidate = True,
                         tex           = '#Delta#Phi_{0,#pi}'
                       )
DPHI_0_2PI = ObservableBase( name        = 'DPHI_0_2PI',
                            args        = [ArgumentType.COMBINATION,\
                                           ArgumentType.COMBINATION],
                            combination = CombinationType.SUMVECTOR,
                            plot_auto   = False,
                           plot_nbins  = 100,
                           plot_xmin   = 0.,
                           plot_xmax   = 6.29,
                           plot_unitX  = '',
                           code_parton = 'dphi_0_2pi()',
                           code_hadron = 'dphi_0_2pi()',
                           code_reco   = 'dphi_0_2pi()',
                         cut_event     = True,
                         cut_candidate = True,
                         tex           = '#Delta#Phi_{0,2#pi}'
                       )

ETA = ObservableBase( name        = 'ETA',
                      args        = [ArgumentType.COMBINATION],
                      combination = CombinationType.SUMVECTOR,
                      plot_auto   = False,
                      plot_nbins  = 100,
                      plot_xmin   = -8.0,
                      plot_xmax   = +8.0,
                      plot_unitX  = '',
                      code_parton = 'eta()',
                      code_hadron = 'eta()',
                      code_reco   = 'eta()',
                      cut_event     = True,
                      cut_candidate = True,
                      tex           = '#eta'
                    )

vETA  = ETA
sETA  = ObservableBase.Clone(ETA, name='sETA', combination=CombinationType.SUMSCALAR, tex='s#eta') 
sdETA = dsETA = ObservableBase.Clone(ETA, name='sdETA', combination=CombinationType.DIFFSCALAR, tex='sd#eta') 
dETA  = dvETA = vdETA = ObservableBase.Clone(ETA, name='dETA', combination=CombinationType.DIFFVECTOR, tex='d#eta')
rETA  = ObservableBase.Clone(ETA, name='rETA', combination=CombinationType.RATIO, tex='r#eta')

THETA = ObservableBase( name    = 'THETA',
                        args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = 0.,
                    plot_xmax   = 2*math.pi+0.01,
                    plot_unitX  = '',
                    code_parton = 'theta()',
                    code_hadron = 'theta()',
                    code_reco   = 'theta()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = '#theta'
                  )

vTHETA  = THETA
sTHETA  = ObservableBase.Clone(THETA, name='sTHETA', combination=CombinationType.SUMSCALAR, tex='s#theta') 
sdTHETA = dsTHETA = ObservableBase.Clone(THETA, name='sdTHETA', combination=CombinationType.DIFFSCALAR, tex='sd#theta') 
dTHETA  = dvTHETA = vdTHETA = ObservableBase.Clone(THETA, name='dTHETA', combination=CombinationType.DIFFVECTOR, tex='d#theta')
rTHETA  = ObservableBase.Clone(THETA, name='rTHETA', combination=CombinationType.RATIO, tex='r#theta')

PHI = ObservableBase( name      = 'PHI',
                      args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = -math.pi-0.01,
                    plot_xmax   = math.pi+0.01,
                    plot_unitX  = '',
                    code_parton = 'phi()',
                    code_hadron = 'phi()',
                    code_reco   = 'phi()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = '#phi'
                  )

vPHI  = PHI
sPHI  = ObservableBase.Clone(PHI, name='sPHI', combination=CombinationType.SUMSCALAR, tex='s#phi',
  plot_xmin=-2.*math.pi-0.01, plot_xmax=2.*math.pi+0.01)
sdPHI = dsPHI = ObservableBase.Clone(PHI, name='sdPHI', combination=CombinationType.DIFFSCALAR, tex='sd#phi',
  plot_xmin=-2.*math.pi-0.01, plot_xmax=2.*math.pi+0.01)
dPHI  = dvPHI = vdPHI = ObservableBase.Clone(PHI, name='dPHI', combination=CombinationType.DIFFVECTOR, tex='d#phi')
rPHI  = ObservableBase.Clone(PHI, name='rPHI', combination=CombinationType.RATIO, tex='r#phi')


Y = ObservableBase( name        = 'Y',
                    args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = -8.0,
                    plot_xmax   = +8.0,
                    plot_unitX  = '',
                    code_parton = 'y()',
                    code_hadron = 'y()',
                    code_reco   = 'y()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = 'y'
                  )

vY  = Y
sY  = ObservableBase.Clone(Y, name='sY', combination=CombinationType.SUMSCALAR, tex='s#y') 
sdY = dsY = ObservableBase.Clone(Y, name='sdY', combination=CombinationType.DIFFSCALAR, tex='sd#y') 
dY  = dvY = vdY = ObservableBase.Clone(Y, name='dY', combination=CombinationType.DIFFVECTOR, tex='d#y')
rY  = ObservableBase.Clone(Y, name='rY', combination=CombinationType.RATIO, tex='r#y')

BETA = ObservableBase( name     = 'BETA',
                       args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = 0.,
                    plot_xmax   = 1.,
                    plot_unitX  = '',
                    code_parton = 'beta()',
                    code_hadron = 'beta()',
                    code_reco   = 'beta()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = '#beta'
                  )

vBETA  = BETA
sBETA  = ObservableBase.Clone(BETA, name='sBETA', combination=CombinationType.SUMSCALAR, tex='s#beta') 
sdBETA = dsBETA = ObservableBase.Clone(BETA, name='sdBETA', combination=CombinationType.DIFFSCALAR, tex='sd#beta') 
dBETA  = dvBETA = vdBETA = ObservableBase.Clone(BETA, name='dBETA', combination=CombinationType.DIFFVECTOR, tex='d#beta')
rBETA  = ObservableBase.Clone(BETA, name='rBETA', combination=CombinationType.RATIO, tex='r#beta')



GAMMA = ObservableBase( name    = 'GAMMA',
                        args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.SUMVECTOR,
                    plot_auto   = False,
                    plot_nbins  = 100,
                    plot_xmin   = 1,
                    plot_xmax   = 1000,
                    plot_unitX  = '',
                    code_parton = 'gamma()',
                    code_hadron = 'gamma()',
                    code_reco   = 'gamma()',
                    cut_event     = True,
                    cut_candidate = True,
                    tex           = '#gamma'
                  )

vGAMMA  = GAMMA
sGAMMA  = ObservableBase.Clone(GAMMA, name='sGAMMA', combination=CombinationType.SUMSCALAR, tex='s#gamma') 
sdGAMMA = dsGAMMA = ObservableBase.Clone(GAMMA, name='sdGAMMA', combination=CombinationType.DIFFSCALAR, tex='sd#gamma') 
dGAMMA  = dvGAMMA = vdGAMMA = ObservableBase.Clone(GAMMA, name='dGAMMA', combination=CombinationType.DIFFVECTOR, tex='d#gamma')
rGAMMA  = ObservableBase.Clone(GAMMA, name='rGAMMA', combination=CombinationType.RATIO, tex='r#gamma')

N = ObservableBase( name        = 'N',
                    args        = [ArgumentType.COMBINATION],
                    combination = CombinationType.DEFAULT,
                    plot_auto   = False,
                    plot_nbins  = 20,
                    plot_xmin   = 0.,
                    plot_xmax   = 20.,
                    plot_unitX  = '',
                    code_parton = 'N()',
                    code_hadron = 'N()',
                    code_reco   = 'N()',
                    cut_event     = True,
                    cut_candidate = False,
                    tex           = 'N'
                  )

vN  = N
sN  = ObservableBase.Clone(N, name='sN', combination=CombinationType.SUMSCALAR, tex='sN') 
sdN = dsN = ObservableBase.Clone(N, name='sdN', combination=CombinationType.DIFFSCALAR, tex='sdN') 
dN  = dvN = vdN = ObservableBase.Clone(N, name='dN', combination=CombinationType.DIFFVECTOR, tex='dN')
rN  = ObservableBase.Clone(N, name='rN', combination=CombinationType.RATIO, tex='rN')

HE_EE = ObservableBase( name        = 'HE_EE',
                        args        = [ArgumentType.PARTICLE],
                        combination = CombinationType.DEFAULT,
                        plot_auto   = False,
                        plot_nbins  = 100,
                        plot_xmin   = 0.,
                        plot_xmax   = 100.,
                        plot_unitX  = '',
                        code_parton = '',
                        code_hadron = '',
                        code_reco   = 'HEoverEE()',
                        cut_event     = True,
                        cut_candidate = True,
                        tex           = 'E_{H}/E_{E}'

                      )

EE_HE = ObservableBase( name        = 'EE_HE',
                        args        = [ArgumentType.PARTICLE],
                        combination = CombinationType.DEFAULT,
                        plot_auto   = False,
                        plot_nbins  = 100,
                        plot_xmin   = 0.,
                        plot_xmax   = 100.,
                        plot_unitX  = '',
                        code_parton = '',
                        code_hadron = '',
                        code_reco   = 'EEoverHE()',
                        cut_event     = True,
                        cut_candidate = True,
                        tex           = 'E_{E}/E_{H}'
                      )

NTRACKS = ObservableBase( name        = 'NTRACKS',
                          args        = [ArgumentType.PARTICLE],
                          combination = CombinationType.DEFAULT,
                          plot_auto   = False,
                          plot_nbins  = 100,
                          plot_xmin   = 0.,
                          plot_xmax   = 100.,
                          plot_unitX  = '',
                          code_parton = '',
                          code_hadron = '',
                          code_reco   = 'ntracks()',
                          cut_event     = True,
                          cut_candidate = True,  
                          tex           = 'n_{tracks}'
                        )
