################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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

SQRTS = ObservableBase( name          = 'SQRTS',
                        args          = [],
                        combination   = CombinationType.DEFAULT,
                        plot_auto     = False,
                        plot_nbins    = 100,
                        plot_xmin     = 0.,
                        plot_xmax     = 1000.,
                        plot_unitX_tlatex    = 'GeV',
                        plot_unitX_latex    = 'GeV',
                        code_parton   = 'PHYSICS->SqrtS(event.mc())',
                        code_hadron   = 'PHYSICS->SqrtS(event.mc())',
                        code_reco     = '',
                        cut_event     = True,
                        cut_candidate = True,
                        tlatex        = '#sqrt{#hat{s}}',
                        latex         = '$\sqrt{\hat{s}}$'
                      )

SCALE = ObservableBase( name          = 'SCALE',
                        args          = [],
                        combination   = CombinationType.DEFAULT,
                        plot_auto     = False,
                        plot_nbins    = 100,
                        plot_xmin     = 0.,
                        plot_xmax     = 10000.,
                        plot_unitX_tlatex    = 'GeV',
                        plot_unitX_latex    = 'GeV',
                        code_parton   = 'event.mc()->scale()',
                        code_hadron   = 'event.mc()->scale()',
                        code_reco     = '',
                        cut_event     = True,
                        cut_candidate = True,
                        tlatex        = 'event scale Q',
                        latex         = 'event scale Q',
                      )

ALPHA_QCD = ObservableBase( name          = 'ALPHA_QCD',
                            args          = [],
                            combination   = CombinationType.DEFAULT,
                            plot_auto     = False,
                            plot_nbins    = 100,
                            plot_xmin     = 0.,
                            plot_xmax     = .2,
                            plot_unitX_tlatex    = '',
                            plot_unitX_latex    = '',
                            code_parton   = 'event.mc()->alphaQCD()',
                            code_hadron   = 'event.mc()->alphaQCD()',
                            code_reco     = '',
                            cut_event     = True,
                            cut_candidate = True,
                            tlatex        = '#alpha_{QCD}',
                            latex         = '$\alpha_\textrm{QCD}$'
                          )

ALPHA_QED = ObservableBase( name          = 'ALPHA_QED',
                            args          = [],
                            combination   = CombinationType.DEFAULT,
                            plot_auto     = False,
                            plot_nbins    = 100,
                            plot_xmin     = 0.,
                            plot_xmax     = .01,
                            plot_unitX_tlatex    = '',
                            plot_unitX_latex    = '',
                            code_parton   = 'event.mc()->alphaQED()',
                            code_hadron   = 'event.mc()->alphaQED()',
                            code_reco     = '',
                            cut_event     = True,
                            cut_candidate = True,
                            tlatex        = '#alpha_{QED}',
                            latex         = '$\alpha_\textrm{QED}$'
                          )

ALPHAT = ObservableBase( name          = 'ALPHAT',
                         args          = [],
                         combination   = CombinationType.DEFAULT,
                         plot_auto     = False,
                         plot_nbins    = 100,
                         plot_xmin     = 0.,
                         plot_xmax     = 1.,
                         plot_unitX_tlatex    = '',
                         plot_unitX_latex    = '',
                         code_parton   = 'PHYSICS->Transverse->AlphaT(event.mc())',
                         code_hadron   = 'PHYSICS->Transverse->AlphaT(event.mc())',
                         code_reco     = 'PHYSICS->Transverse->AlphaT(event.rec())',
                         cut_event     = True,
                         cut_candidate = True,
                         tlatex        = '#alpha_T',
                         latex         = '$\alpha_T$'
                       )


TET = ObservableBase( name          = 'TET',
                      args          = [],
                      combination   = CombinationType.DEFAULT,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = 0.,
                      plot_xmax     = 1000.,
                      plot_unitX_tlatex    = 'GeV',
                      plot_unitX_latex    = 'GeV',
                      code_parton   = 'PHYSICS->Transverse->EventTET(event.mc())',
                      code_hadron   = 'PHYSICS->Transverse->EventTET(event.mc())',
                      code_reco     = 'PHYSICS->Transverse->EventTET(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tlatex        = 'E_{T}',
                      latex         = '$E_T$'
                    )

MET = ObservableBase( name          = 'MET',
                      args          = [],
                      combination   = CombinationType.DEFAULT,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = 0.,
                      plot_xmax     = 1000.,
                      plot_unitX_tlatex    = 'GeV',
                      plot_unitX_latex    = 'GeV',
                      code_parton   = 'PHYSICS->Transverse->EventMET(event.mc())',
                      code_hadron   = 'PHYSICS->Transverse->EventMET(event.mc())',
                      code_reco     = 'PHYSICS->Transverse->EventMET(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tlatex        = '#slash{E}_{T}',
                      latex         = '$\slash{E}_T$'
                    )

THT = ObservableBase( name          = 'THT',
                      args          = [],
                      combination   = CombinationType.DEFAULT,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = 0.,
                      plot_xmax     = 1000.,
                      plot_unitX_tlatex    = 'GeV',
                      plot_unitX_latex    = 'GeV',
                      code_parton   = 'PHYSICS->Transverse->EventTHT(event.mc())',
                      code_hadron   = 'PHYSICS->Transverse->EventTHT(event.mc())',
                      code_reco     = 'PHYSICS->Transverse->EventTHT(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tlatex        = 'H_{T}',
                      latex         = '$H_T$'
                    )

MEFF = ObservableBase( name          = 'MEFF',
                      args          = [],
                      combination   = CombinationType.DEFAULT,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = 0.,
                      plot_xmax     = 1000.,
                      plot_unitX_tlatex    = 'GeV',
                      plot_unitX_latex    = 'GeV',
                      code_parton   = 'PHYSICS->Transverse->EventMEFF(event.mc())',
                      code_hadron   = 'PHYSICS->Transverse->EventMEFF(event.mc())',
                      code_reco     = 'PHYSICS->Transverse->EventMEFF(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tlatex        = 'M_{eff}',
                      latex         = '$M_{eff}$'
                    )

MHT = ObservableBase( name          = 'MHT',
                      args          = [],
                      combination   = CombinationType.DEFAULT,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = 0.,
                      plot_xmax     = 1000.,
                      plot_unitX_tlatex    = 'GeV',
                      plot_unitX_latex    = 'GeV',
                      code_parton   = 'PHYSICS->Transverse->EventMHT(event.mc())',
                      code_hadron   = 'PHYSICS->Transverse->EventMHT(event.mc())',
                      code_reco     = 'PHYSICS->Transverse->EventMHT(event.rec())',
                      cut_event     = True,
                      cut_candidate = True,
                      tlatex        = '#slash{H}_{T}',
                      latex         = '$\slash{H}_T$'
                    )

WEIGHTS = ObservableBase( name      = 'WEIGHTS',
                      args          = [],
                      combination   = CombinationType.DEFAULT,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = -1.,
                      plot_xmax     = 1.,
                      plot_unitX_tlatex    = '',
                      plot_unitX_latex    = '',
                      code_parton   = 'PHYSICS->weights(event.mc())',
                      code_hadron   = 'PHYSICS->weights(event.mc())',
                      code_reco     = '',
                      cut_event     = True,
                      cut_candidate = False,
                      tlatex        = '#omega',
                      latex         = '$\omega$'
                    )


NPID = ObservableBase( name          = 'NPID',
                       args          = [],
                       combination   = CombinationType.DEFAULT,
                       plot_auto     = True,
                       plot_nbins    = 100,
                       plot_xmin     = 0.,
                       plot_xmax     = 100.,
                       plot_unitX_tlatex    = '',
                       plot_unitX_latex    = '',
                       code_parton   = 'NPID',
                       code_hadron   = 'NPID',
                       code_reco     = 'NPID',
                       cut_event     = True,
                       cut_candidate = True,
                       tlatex        = 'NPID',
                       latex         = 'NPID'
                     )

NAPID = ObservableBase( name          = 'NAPID',
                        args          = [],
                        combination   = CombinationType.DEFAULT,
                        plot_auto     = True,
                        plot_nbins    = 100,
                        plot_xmin     = 0.,
                        plot_xmax     = 100.,
                        plot_unitX_tlatex    = '',
                        plot_unitX_latex    = '',
                        code_parton   = 'NAPID',
                        code_hadron   = 'NAPID',
                        code_reco     = 'NAPID',
                        cut_event     = True,
                        cut_candidate = True,
                        tlatex        = '|NPID|',
                        latex         = '|NPID|'
                      )

E = ObservableBase( name          = 'E',
                    args          = [ArgumentType.COMBINATION],
                    combination   = CombinationType.SUMVECTOR,
                    plot_auto     = False,
                    plot_nbins    = 100,
                    plot_xmin     = 0.,
                    plot_xmax     = 100.,
                    plot_unitX_tlatex    = 'GeV',
                    plot_unitX_latex    = 'GeV',
                    code_parton   = 'e()',
                    code_hadron   = 'e()',
                    code_reco     = 'e()',
                    cut_event     = True,
                    cut_candidate = True,
                    tlatex        = 'E',
                    latex         = 'E'
                  )

vE  = E
sE  = ObservableBase.Clone(E, name='sE', combination=CombinationType.SUMSCALAR, tlatex='sE', latex='sE') 
sdE = dsE = ObservableBase.Clone(E, name='sdE', combination=CombinationType.DIFFSCALAR, tlatex='sdE', latex='sdE') 
dE  = dvE = vdE = ObservableBase.Clone(E, name='dE', combination=CombinationType.DIFFVECTOR, tlatex='dE', latex='dE')
rE  = ObservableBase.Clone(E, name='rE', combination=CombinationType.RATIO, tlatex='rE', latex='rE')

M = ObservableBase( name          = 'M',
                    args          = [ArgumentType.COMBINATION],
                    combination   = CombinationType.SUMVECTOR,
                    plot_auto     = False,
                    plot_nbins    = 100,
                    plot_xmin     = 0.,
                    plot_xmax     = 1000.,
                    plot_unitX_tlatex    = 'GeV/c^{2}',
                    plot_unitX_latex    = 'GeV$^2$',
                    code_parton   = 'm()',
                    code_hadron   = 'm()',
                    code_reco     = 'm()',
                    cut_event     = True,
                    cut_candidate = True,
                    tlatex        = 'M',
                    latex         = 'M'
                  )

vM  = M
sM  = ObservableBase.Clone(M, name='sM', combination=CombinationType.SUMSCALAR, tlatex='sM', latex='sM') 
sdM = dsM = ObservableBase.Clone(M, name='sdM', combination=CombinationType.DIFFSCALAR, tlatex='sdM', latex='sdM') 
dM  = dvM = vdM = ObservableBase.Clone(M, name='dM', combination=CombinationType.DIFFVECTOR, tlatex='dM', latex='dM')
rM  = ObservableBase.Clone(M, name='rM', combination=CombinationType.RATIO, tlatex='rM', latex='rM')


P = ObservableBase( name          = 'P',
                    args          = [ArgumentType.COMBINATION],
                    combination   = CombinationType.SUMVECTOR,
                    plot_auto     = False,
                    plot_nbins    = 100,
                    plot_xmin     = 0.,
                    plot_xmax     = 1000.,
                    plot_unitX_tlatex    = 'GeV/c',
                    plot_unitX_latex    = 'GeV/c',
                    code_parton   = 'p()',
                    code_hadron   = 'p()',
                    code_reco     = 'p()',
                    cut_event     = True,
                    cut_candidate = True,
                    tlatex        = 'p',
                    latex         = 'p'
                  )

vP  = P
sP  = ObservableBase.Clone(P, name='sP', combination=CombinationType.SUMSCALAR, tlatex='sp', latex='sp') 
sdP = dsP = ObservableBase.Clone(P, name='sdP', combination=CombinationType.DIFFSCALAR, tlatex='sdp', latex='sdp') 
dP  = dvP = vdP = ObservableBase.Clone(P, name='dP', combination=CombinationType.DIFFVECTOR, tlatex='dp', latex='dp')
rP  = ObservableBase.Clone(P, name='rP', combination=CombinationType.RATIO, tlatex='rp', latex='rp')

ET = ObservableBase( name          = 'ET',
                     args          = [ArgumentType.COMBINATION],
                     combination   = CombinationType.SUMVECTOR,
                     plot_auto     = False,
                     plot_nbins    = 100,
                     plot_xmin     = 0.,
                     plot_xmax     = 100.,
                     plot_unitX_tlatex    = 'GeV',
                     plot_unitX_latex    = 'GeV',
                     code_parton   = 'et()',
                     code_hadron   = 'et()',
                     code_reco     = 'et()',
                     cut_event     = True,
                     cut_candidate = True,
                     tlatex        = 'E_{T}',
                     latex         = '$E_T$'
                   )
 
vET  = ET
sET  = ObservableBase.Clone(ET, name='sET', combination=CombinationType.SUMSCALAR, tlatex='sE_{T}', latex='$sE_T$') 
sdET = dsET = ObservableBase.Clone(ET, name='sdET', combination=CombinationType.DIFFSCALAR, tlatex='dsE_{T}', latex='$dsE_T$') 
dET  = dvET = vdET = ObservableBase.Clone(ET, name='dET', combination=CombinationType.DIFFVECTOR, tlatex='dE_{T}', latex='dE_T$')
rET  = ObservableBase.Clone(ET, name='rET', combination=CombinationType.RATIO, tlatex='rE_{T}', latex='$rE_T$')

MT = ObservableBase( name          = 'MT',
                     args          = [ArgumentType.COMBINATION],
                     combination   = CombinationType.SUMVECTOR,
                     plot_auto     = False,
                     plot_nbins    = 100,
                     plot_xmin     = 0.,
                     plot_xmax     = 1000.,
                     plot_unitX_tlatex    = 'GeV/c^{2}',
                     plot_unitX_latex    = 'GeV/c$^2$',
                     code_parton   = 'mt()',
                     code_hadron   = 'mt()',
                     code_reco     = 'mt()',
                     cut_event     = True,
                     cut_candidate = True,
                     tlatex        = 'M_{T}',
                     latex         = '$M_T$'
                   )

vMT  = MT
sMT  = ObservableBase.Clone(MT, name='sMT', combination=CombinationType.SUMSCALAR, tlatex='sM_{T}', latex='$sM_T$') 
sdMT = dsMT = ObservableBase.Clone(MT, name='sdMT', combination=CombinationType.DIFFSCALAR, tlatex='sdM_{T}', latex='$sdM_T$') 
dMT  = dvMT = vdMT = ObservableBase.Clone(MT, name='dMT', combination=CombinationType.DIFFVECTOR, tlatex='dM_{T}', latex='$dM_T$')
rMT  = ObservableBase.Clone(MT, name='rMT', combination=CombinationType.RATIO, tlatex='rM_{T}', latex='$rM_T$')


MT_MET = ObservableBase( name          = 'MT_MET',
                         args          = [ArgumentType.COMBINATION],
                         combination   = CombinationType.SUMVECTOR,
                         plot_auto     = False,
                         plot_nbins    = 100,
                         plot_xmin     = 0.,
                         plot_xmax     = 1000.,
                         plot_unitX_tlatex    = 'GeV/c^{2}',
                         plot_unitX_latex    = 'GeV/c$^2$',
                         code_parton   = 'mt_met(event.mc()->MET().momentum())',
                         code_hadron   = 'mt_met(event.mc()->MET().momentum())',
                         code_reco     = 'mt_met(event.rec()->MET().momentum())',
                         cut_event     = True,
                         cut_candidate = True,
                         tlatex        = 'M_{T}',
                         latex         = '$M_T$',
                       )

vMT_MET  = MT_MET
sMT_MET  = ObservableBase.Clone(MT_MET, name='sMT_MET', combination=CombinationType.SUMSCALAR, tlatex='sM_{T}', latex='$sM_T$') 
sdMT_MET = dsMT_MET = ObservableBase.Clone(MT_MET, name='sdMT_MET', combination=CombinationType.DIFFSCALAR, tlatex='sdM_{T}', latex='$sdM_T$') 
dMT_MET  = dvMT_MET = vdMT_MET = ObservableBase.Clone(MT_MET, name='dMT_MET', combination=CombinationType.DIFFVECTOR, tlatex='dM_{T}', latex='$dM_T$')
rMT_MET  = ObservableBase.Clone(MT_MET, name='rMT_MET', combination=CombinationType.RATIO, tlatex='rM_{T}', latex='$rM_T$')

PT = ObservableBase( name          = 'PT',
                     args          = [ArgumentType.COMBINATION],
                     combination   = CombinationType.SUMVECTOR,
                     plot_auto     = False,
                     plot_nbins    = 100,
                     plot_xmin     = 0.,
                     plot_xmax     = 1000.,
                     plot_unitX_tlatex    = 'GeV/c',
                     plot_unitX_latex    = 'GeV/c',
                     code_parton   = 'pt()',
                     code_hadron   = 'pt()',
                     code_reco     = 'pt()',
                     cut_event     = True,
                     cut_candidate = True,
                     tlatex        = 'p_{T}',
                     latex         = '$p_T$'
                   )

vPT  = PT
sPT  = ObservableBase.Clone(PT, name='sPT', combination=CombinationType.SUMSCALAR, tlatex='sp_{T}', latex='$sp_T$') 
sdPT = dsPT = ObservableBase.Clone(PT, name='sdPT', combination=CombinationType.DIFFSCALAR, tlatex='sdp_{T}', latex='$sdp_T$') 
dPT  = dvPT = vdPT = ObservableBase.Clone(PT, name='dPT', combination=CombinationType.DIFFVECTOR, tlatex='dp_{T}', latex='$dp_T$')
rPT  = ObservableBase.Clone(PT, name='rPT', combination=CombinationType.RATIO, tlatex='rp_{T}', latex='$rp_T$')

PX = ObservableBase( name          = 'PX',
                     args          = [ArgumentType.COMBINATION],
                     combination   = CombinationType.SUMVECTOR,
                     plot_auto     = False,
                     plot_nbins    = 100,
                     plot_xmin     = -1000.,
                     plot_xmax     = +1000.,
                     plot_unitX_tlatex    = 'GeV/c',
                     plot_unitX_latex    = 'GeV/c',
                     code_parton   = 'px()',
                     code_hadron   = 'px()',
                     code_reco     = 'px()',
                     cut_event     = True,
                     cut_candidate = True,
                     tlatex        = 'p_{x}',
                     latex         = '$p_x$'
                   )

vPX  = PX
sPX  = ObservableBase.Clone(PX, name='sPX', combination=CombinationType.SUMSCALAR, tlatex='sp_{x}', latex='$sp_x$') 
sdPX = dsPX = ObservableBase.Clone(PX, name='sdPX', combination=CombinationType.DIFFSCALAR, tlatex='sdp_{x}', latex='$sdp_x$') 
dPX  = dvPX = vdPX = ObservableBase.Clone(PX, name='dPX', combination=CombinationType.DIFFVECTOR, tlatex='dp_{x}', latex='$dp_x$')
rPX  = ObservableBase.Clone(PX, name='rPX', combination=CombinationType.RATIO, tlatex='rp_{x}', latex='$rp_x$')

PY = ObservableBase( name          = 'PY',
                     args          = [ArgumentType.COMBINATION],
                     combination   = CombinationType.SUMVECTOR,
                     plot_auto     = False,
                     plot_nbins    = 100,
                     plot_xmin     = -1000.,
                     plot_xmax     = +1000.,
                     plot_unitX_tlatex    = 'GeV/c',
                     plot_unitX_latex    = 'GeV/c',
                     code_parton   = 'py()',
                     code_hadron   = 'py()',
                     code_reco     = 'py()',
                     cut_event     = True,
                     cut_candidate = True,
                     tlatex        = 'p_{y}',
                     latex         = '$p_y$'
                   )

vPY  = PY
sPY  = ObservableBase.Clone(PY, name='sPY', combination=CombinationType.SUMSCALAR, tlatex='sp_{y}', latex='$sp_y$') 
sdPY = dsPY = ObservableBase.Clone(PY, name='sdPY', combination=CombinationType.DIFFSCALAR, tlatex='sdp_{y}', latex='$sdp_y$') 
dPY  = dvPY = vdPY = ObservableBase.Clone(PY, name='dPY', combination=CombinationType.DIFFVECTOR, tlatex='dp_{y}', latex='$dp_y$')
rPY  = ObservableBase.Clone(PY, name='rPY', combination=CombinationType.RATIO, tlatex='rp_{y}', latex='$rp_y$')

PZ = ObservableBase( name          = 'PZ',
                     args          = [ArgumentType.COMBINATION],
                     combination   = CombinationType.SUMVECTOR,
                     plot_auto     = False,
                     plot_nbins    = 100,
                     plot_xmin     = -1000.,
                     plot_xmax     = +1000.,
                     plot_unitX_tlatex    = 'GeV/c',
                     plot_unitX_latex    = 'GeV/c',
                     code_parton   = 'pz()',
                     code_hadron   = 'pz()',
                     code_reco     = 'pz()',
                     cut_event     = True,
                     cut_candidate = True,
                     tlatex        = 'p_{z}',
                     latex         = '$p_z$'
                   )

vPZ  = PZ
sPZ  = ObservableBase.Clone(PZ, name='sPZ', combination=CombinationType.SUMSCALAR, tlatex='sp_{z}', latex='$sp_z$') 
sdPZ = dsPZ = ObservableBase.Clone(PZ, name='sdPZ', combination=CombinationType.DIFFSCALAR, tlatex='sdp_{z}', latex='$sdp_z$') 
dPZ  = dvPZ = vdPZ = ObservableBase.Clone(PZ, name='dPZ', combination=CombinationType.DIFFVECTOR, tlatex='dp_{z}', latex='$dp_z$')
rPZ  = ObservableBase.Clone(PZ, name='rPZ', combination=CombinationType.RATIO, tlatex='rp_{z}', latex='$rp_z$')

R = ObservableBase( name          = 'R',
                    args          = [ArgumentType.COMBINATION],
                    combination   = CombinationType.SUMVECTOR,
                    plot_auto     = False,
                    plot_nbins    = 100,
                    plot_xmin     = 0.,
                    plot_xmax     = 1000.,
                    plot_unitX_tlatex    = '',
                    plot_unitX_latex    = '',
                    code_parton   = 'r()',
                    code_hadron   = 'r()',
                    code_reco     = 'r()',
                    cut_event     = True,
                    cut_candidate = True,
                    tlatex        = 'R',
                    latex         = 'R'
                  )

vR  = R
sR  = ObservableBase.Clone(R, name='sR', combination=CombinationType.SUMSCALAR, tlatex='sR', latex='sR') 
sdR = dsR = ObservableBase.Clone(R, name='sdR', combination=CombinationType.DIFFSCALAR, tlatex='sdR', latex='sdR') 
dR  = dvR = vdR = ObservableBase.Clone(R, name='dR', combination=CombinationType.DIFFVECTOR, tlatex='dR', latex='dR')
rR  = ObservableBase.Clone(R, name='rR', combination=CombinationType.RATIO, tlatex='rR', latex='rR')


DELTAR = ObservableBase( name          = 'DELTAR',
                         args          = [ArgumentType.COMBINATION,\
                                         ArgumentType.COMBINATION],
                         combination   = CombinationType.SUMVECTOR,
                         plot_auto     = False,
                         plot_nbins    = 100,
                         plot_xmin     = 0.,
                         plot_xmax     = 6.28,
                         plot_unitX_tlatex    = '',
                         plot_unitX_latex    = '',
                         code_parton   = 'dr()',
                         code_hadron   = 'dr()',
                         code_reco     = 'dr()',
                         cut_event     = True,
                         cut_candidate = True,
                         tlatex        = '#DeltaR',
                         latex         = '$\Delta R$'
                       )

vDELTAR  = DELTAR
dDELTAR  = dvDELTAR = vdDELTAR = ObservableBase.Clone(DELTAR, name='dDELTAR', combination=CombinationType.DIFFVECTOR, tlatex='d#DeltaR', latex='$d\Delta R$')

DPHI_0_PI = ObservableBase( name          = 'DPHI_0_PI',
                            args          = [ArgumentType.COMBINATION,\
                                             ArgumentType.COMBINATION],
                            combination   = CombinationType.SUMVECTOR,
                            plot_auto     = False,
                            plot_nbins    = 100,
                            plot_xmin     = 0.,
                            plot_xmax     = 3.15,
                            plot_unitX_tlatex    = '',
                            plot_unitX_latex    = '',
                            code_parton   = 'dphi_0_pi()',
                            code_hadron   = 'dphi_0_pi()',
                            code_reco     = 'dphi_0_pi()',
                            cut_event     = True,
                            cut_candidate = True,
                            tlatex        = '#Delta#Phi_{0,#pi}',
                            latex         = '$\Delta\Phi_{0,\pi}$'
                          )

DPHI_0_2PI = ObservableBase( name          = 'DPHI_0_2PI',
                             args          = [ArgumentType.COMBINATION,\
                                              ArgumentType.COMBINATION],
                             combination   = CombinationType.SUMVECTOR,
                             plot_auto     = False,
                             plot_nbins    = 100,
                             plot_xmin     = 0.,
                             plot_xmax     = 6.29,
                             plot_unitX_tlatex    = '',
                             plot_unitX_latex    = '',
                             code_parton   = 'dphi_0_2pi()',
                             code_hadron   = 'dphi_0_2pi()',
                             code_reco     = 'dphi_0_2pi()',
                             cut_event     = True,
                             cut_candidate = True,
                             tlatex        = '#Delta#Phi_{0,2#pi}',
                             latex         = '$\Delta\Phi_{0,2\pi}$'
                           )

RECOIL = ObservableBase( name          = 'RECOIL',
                         args          = [ArgumentType.COMBINATION,\
                                         ArgumentType.COMBINATION],
                         combination   = CombinationType.SUMVECTOR,
                         plot_auto     = False,
                         plot_nbins    = 100,
                         plot_xmin     = 0.,
                         plot_xmax     = 500.,
                         plot_unitX_tlatex    = 'GeV/c^{2}',
                         plot_unitX_latex    = 'GeV$^2$',
                         code_parton   = 'recoil()',
                         code_hadron   = 'recoil()',
                         code_reco     = 'recoil()',
                         cut_event     = True,
                         cut_candidate = True,
                         tlatex        = '#Delta M',
                         latex         = '$\Delta M$'
                       )



ETA = ObservableBase( name          = 'ETA',
                      args          = [ArgumentType.COMBINATION],
                      combination   = CombinationType.SUMVECTOR,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = -8.0,
                      plot_xmax     = +8.0,
                      plot_unitX_tlatex    = '',
                      plot_unitX_latex    = '',
                      code_parton   = 'eta()',
                      code_hadron   = 'eta()',
                      code_reco     = 'eta()',
                      cut_event     = True,
                      cut_candidate = True,
                      tlatex        = '#eta',
                      latex         = '$\eta$'
                    )

vETA  = ETA
sETA  = ObservableBase.Clone(ETA, name='sETA', combination=CombinationType.SUMSCALAR, tlatex='s#eta', latex='$s\eta$') 
sdETA = dsETA = ObservableBase.Clone(ETA, name='sdETA', combination=CombinationType.DIFFSCALAR, tlatex='sd#eta', latex='$sd\eta$') 
dETA  = dvETA = vdETA = ObservableBase.Clone(ETA, name='dETA', combination=CombinationType.DIFFVECTOR, tlatex='d#eta', latex='$d\eta$')
rETA  = ObservableBase.Clone(ETA, name='rETA', combination=CombinationType.RATIO, tlatex='r#eta', latex='$r\eta$')

ABSETA = ObservableBase( name          = 'ABSETA',
                         args          = [ArgumentType.COMBINATION],
                         combination   = CombinationType.SUMVECTOR,
                         plot_auto     = False,
                         plot_nbins    =  50,
                         plot_xmin     =  0.0,
                         plot_xmax     = +8.0,
                         plot_unitX_tlatex    = '',
                         plot_unitX_latex    = '',
                         code_parton   = 'abseta()',
                         code_hadron   = 'abseta()',
                         code_reco     = 'abseta()',
                         cut_event     = True,
                         cut_candidate = True,
                         tlatex        = '|#eta|',
                         latex         = '$|\eta|$'
                       )

vABSETA  = ABSETA
sABSETA  = ObservableBase.Clone(ABSETA, name='sABSETA', combination=CombinationType.SUMSCALAR, tlatex='s|#eta|', latex='$s|\eta|$') 
sdABSETA = dsABSETA = ObservableBase.Clone(ABSETA, name='sdABSETA', combination=CombinationType.DIFFSCALAR, tlatex='sd|#eta|', latex='$sd|\eta|$') 
dABSETA  = dvABSETA = vdABSETA = ObservableBase.Clone(ABSETA, name='dABSETA', combination=CombinationType.DIFFVECTOR, tlatex='d|#eta|', latex='$d|\eta|$')
rABSETA  = ObservableBase.Clone(ABSETA, name='rABSETA', combination=CombinationType.RATIO, tlatex='r|#eta|', latex='$r|\eta|$')


THETA = ObservableBase( name          = 'THETA',
                        args          = [ArgumentType.COMBINATION],
                        combination   = CombinationType.SUMVECTOR,
                        plot_auto     = False,
                        plot_nbins    = 100,
                        plot_xmin     = 0.,
                        plot_xmax     = 2*math.pi+0.01,
                        plot_unitX_tlatex    = '',
                        plot_unitX_latex    = '',
                        code_parton   = 'theta()',
                        code_hadron   = 'theta()',
                        code_reco     = 'theta()',
                        cut_event     = True,
                        cut_candidate = True,
                        tlatex        = '#theta',
                        latex         = '$\theta$'
                  )

vTHETA  = THETA
sTHETA  = ObservableBase.Clone(THETA, name='sTHETA', combination=CombinationType.SUMSCALAR, tlatex='s#theta', latex='$s\theta$') 
sdTHETA = dsTHETA = ObservableBase.Clone(THETA, name='sdTHETA', combination=CombinationType.DIFFSCALAR, tlatex='sd#theta', latex='$sd\theta$' ) 
dTHETA  = dvTHETA = vdTHETA = ObservableBase.Clone(THETA, name='dTHETA', combination=CombinationType.DIFFVECTOR, tlatex='d#theta', latex='$d\theta$')
rTHETA  = ObservableBase.Clone(THETA, name='rTHETA', combination=CombinationType.RATIO, tlatex='r#theta', latex='$r\theta$')

PHI = ObservableBase( name          = 'PHI',
                      args          = [ArgumentType.COMBINATION],
                      combination   = CombinationType.SUMVECTOR,
                      plot_auto     = False,
                      plot_nbins    = 100,
                      plot_xmin     = -math.pi-0.01,
                      plot_xmax     = math.pi+0.01,
                      plot_unitX_tlatex    = '',
                      plot_unitX_latex    = '',
                      code_parton   = 'phi()',
                      code_hadron   = 'phi()',
                      code_reco     = 'phi()',
                      cut_event     = True,
                      cut_candidate = True,
                      tlatex        = '#phi',
                      latex         = '$\phi$'
                  )

vPHI  = PHI
sPHI  = ObservableBase.Clone(PHI, name='sPHI', combination=CombinationType.SUMSCALAR, tlatex='s#phi', latex='$s\phi$',
  plot_xmin=-2.*math.pi-0.01, plot_xmax=2.*math.pi+0.01)
sdPHI = dsPHI = ObservableBase.Clone(PHI, name='sdPHI', combination=CombinationType.DIFFSCALAR, tlatex='sd#phi', latex='$sd\phi$',
  plot_xmin=-2.*math.pi-0.01, plot_xmax=2.*math.pi+0.01)
dPHI  = dvPHI = vdPHI = ObservableBase.Clone(PHI, name='dPHI', combination=CombinationType.DIFFVECTOR, tlatex='d#phi', latex='$d\phi$')
rPHI  = ObservableBase.Clone(PHI, name='rPHI', combination=CombinationType.RATIO, tlatex='r#phi', latex='$r\phi$')


Y = ObservableBase( name          = 'Y',
                    args          = [ArgumentType.COMBINATION],
                    combination   = CombinationType.SUMVECTOR,
                    plot_auto     = False,
                    plot_nbins    = 100,
                    plot_xmin     = -8.0,
                    plot_xmax     = +8.0,
                    plot_unitX_tlatex    = '',
                    plot_unitX_latex    = '',
                    code_parton   = 'y()',
                    code_hadron   = 'y()',
                    code_reco     = 'y()',
                    cut_event     = True,
                    cut_candidate = True,
                    tlatex        = 'y',
                    latex         = 'y'
                  )

vY  = Y
sY  = ObservableBase.Clone(Y, name='sY', combination=CombinationType.SUMSCALAR, tlatex='sy', latex='sy') 
sdY = dsY = ObservableBase.Clone(Y, name='sdY', combination=CombinationType.DIFFSCALAR, tlatex='sdy', latex='sdy') 
dY  = dvY = vdY = ObservableBase.Clone(Y, name='dY', combination=CombinationType.DIFFVECTOR, tlatex='dy', latex='dy')
rY  = ObservableBase.Clone(Y, name='rY', combination=CombinationType.RATIO, tlatex='ry', latex='ry')

BETA = ObservableBase( name          = 'BETA',
                       args          = [ArgumentType.COMBINATION],
                       combination   = CombinationType.SUMVECTOR,
                       plot_auto     = False,
                       plot_nbins    = 100,
                       plot_xmin     = 0.,
                       plot_xmax     = 1.,
                       plot_unitX_tlatex    = '',
                       plot_unitX_latex    = '',
                       code_parton   = 'beta()',
                       code_hadron   = 'beta()',
                       code_reco     = 'beta()',
                       cut_event     = True,
                       cut_candidate = True,
                       tlatex        = '#beta',
                       latex         = '$\beta$'
                     )

vBETA  = BETA
sBETA  = ObservableBase.Clone(BETA, name='sBETA', combination=CombinationType.SUMSCALAR, tlatex='s#beta', latex='$s\beta$') 
sdBETA = dsBETA = ObservableBase.Clone(BETA, name='sdBETA', combination=CombinationType.DIFFSCALAR, tlatex='sd#beta', latex='$sd\beta$') 
dBETA  = dvBETA = vdBETA = ObservableBase.Clone(BETA, name='dBETA', combination=CombinationType.DIFFVECTOR, tlatex='d#beta', latex='$d\beta$')
rBETA  = ObservableBase.Clone(BETA, name='rBETA', combination=CombinationType.RATIO, tlatex='r#beta', latex='$r\beta$')


GAMMA = ObservableBase( name          = 'GAMMA',
                        args          = [ArgumentType.COMBINATION],
                        combination   = CombinationType.SUMVECTOR,
                        plot_auto     = False,
                        plot_nbins    = 100,
                        plot_xmin     = 1,
                        plot_xmax     = 1000,
                        plot_unitX_tlatex    = '',
                        plot_unitX_latex    = '',
                        code_parton   = 'gamma()',
                        code_hadron   = 'gamma()',
                        code_reco     = 'gamma()',
                        cut_event     = True,
                        cut_candidate = True,
                        tlatex        = '#gamma',
                        latex         = '$\gamma$'
                      )

vGAMMA  = GAMMA
sGAMMA  = ObservableBase.Clone(GAMMA, name='sGAMMA', combination=CombinationType.SUMSCALAR, tlatex='s#gamma', latex='$s\gamma$') 
sdGAMMA = dsGAMMA = ObservableBase.Clone(GAMMA, name='sdGAMMA', combination=CombinationType.DIFFSCALAR, tlatex='sd#gamma', latex='$sd\gamma$') 
dGAMMA  = dvGAMMA = vdGAMMA = ObservableBase.Clone(GAMMA, name='dGAMMA', combination=CombinationType.DIFFVECTOR, tlatex='d#gamma', latex='$d\gamma$')
rGAMMA  = ObservableBase.Clone(GAMMA, name='rGAMMA', combination=CombinationType.RATIO, tlatex='r#gamma', latex='$r\gamma$')

N = ObservableBase( name          = 'N',
                    args          = [ArgumentType.COMBINATION],
                    combination   = CombinationType.DEFAULT,
                    plot_auto     = False,
                    plot_nbins    = 20,
                    plot_xmin     = 0.,
                    plot_xmax     = 20.,
                    plot_unitX_tlatex    = '',
                    plot_unitX_latex    = '',
                    code_parton   = 'N()',
                    code_hadron   = 'N()',
                    code_reco     = 'N()',
                    cut_event     = True,
                    cut_candidate = False,
                    tlatex        = 'N',
                    latex         = 'N'
                  )

vN  = N
sN  = ObservableBase.Clone(N, name='sN', combination=CombinationType.SUMSCALAR, tlatex='sN', latex='sN') 
sdN = dsN = ObservableBase.Clone(N, name='sdN', combination=CombinationType.DIFFSCALAR, tlatex='sdN', latex='sdN') 
dN  = dvN = vdN = ObservableBase.Clone(N, name='dN', combination=CombinationType.DIFFVECTOR, tlatex='dN', latex='dN')
rN  = ObservableBase.Clone(N, name='rN', combination=CombinationType.RATIO, tlatex='rN', latex='rN')

HE_EE = ObservableBase( name          = 'HE_EE',
                        args          = [ArgumentType.PARTICLE],
                        combination   = CombinationType.DEFAULT,
                        plot_auto     = False,
                        plot_nbins    = 100,
                        plot_xmin     = 0.,
                        plot_xmax     = 100.,
                        plot_unitX_tlatex    = '',
                        plot_unitX_latex    = '',
                        code_parton   = '',
                        code_hadron   = '',
                        code_reco     = 'HEoverEE()',
                        cut_event     = True,
                        cut_candidate = True,
                        tlatex        = 'E_{H}/E_{E}',
                        latex         = '$E_H/E_E$',
                      )

EE_HE = ObservableBase( name          = 'EE_HE',
                        args          = [ArgumentType.PARTICLE],
                        combination   = CombinationType.DEFAULT,
                        plot_auto     = False,
                        plot_nbins    = 100,
                        plot_xmin     = 0.,
                        plot_xmax     = 100.,
                        plot_unitX_tlatex    = '',
                        plot_unitX_latex    = '',
                        code_parton   = '',
                        code_hadron   = '',
                        code_reco     = 'EEoverHE()',
                        cut_event     = True,
                        cut_candidate = True,
                        tlatex        = 'E_{E}/E_{H}',
                        latex         = '$E_E/E_H$',
                      )

NTRACKS = ObservableBase( name          = 'NTRACKS',
                          args          = [ArgumentType.PARTICLE],
                          combination   = CombinationType.DEFAULT,
                          plot_auto     = False,
                          plot_nbins    = 100,
                          plot_xmin     = 0.,
                          plot_xmax     = 100.,
                          plot_unitX_tlatex    = '',
                          plot_unitX_latex    = '',
                          code_parton   = '',
                          code_hadron   = '',
                          code_reco     = 'ntracks()',
                          cut_event     = True,
                          cut_candidate = True,  
                          tlatex        = 'n_{tracks}',
                          latex         = '$n_\textrm{tracks}$'
                        )
