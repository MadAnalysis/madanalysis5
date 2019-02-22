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


from madanalysis.enumeration.ma5_running_type import MA5RunningType
import logging

def WriteHadronicList(file,main):
    file.write('  // definition of the multiparticle "hadronic"\n')
    for item in main.multiparticles.Get("hadronic"):
        file.write('  PHYSICS->mcConfig().AddHadronicId('+str(item)+');\n')


def WriteInvisibleList(file,main):
    file.write('  // definition of the multiparticle "invisible"\n')
    for item in main.multiparticles.Get("invisible"):
        file.write('  PHYSICS->mcConfig().AddInvisibleId('+str(item)+');\n')


def WriteJobInitialize(file,main):

    # Function header
    file.write('bool user::Initialize(const MA5::Configuration& cfg,\n')
    file.write('                      const std::map<std::string,std::string>& parameters)\n')
    file.write('{\n')

    # mcConfig initialization
    if main.mode!=MA5RunningType.RECO:
        file.write('  // Initializing PhysicsService for MC\n') 
        file.write('  PHYSICS->mcConfig().Reset();\n\n')
        WriteHadronicList(file,main)
        file.write('\n')
        WriteInvisibleList(file,main)
        file.write('\n')
    else:
        file.write('  // Initializing PhysicsService for MC\n') 
        file.write('  PHYSICS->mcConfig().Reset();\n\n')
        file.write('\n')
        file.write('  // definition of the multiparticle "hadronic"\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20543);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20533);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20523);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20513);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20433);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20423);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20413);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20323);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20313);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-20213);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10543);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10541);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10533);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10531);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10523);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10521);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10513);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10511);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10433);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10431);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10423);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10421);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10413);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10411);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10323);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10321);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10313);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10311);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10213);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-10211);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5554);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5544);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5542);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5534);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5532);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5524);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5522);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5514);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5512);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5503);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5444);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5442);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5434);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5432);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5424);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5422);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5414);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5412);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5403);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5401);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5342);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5334);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5332);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5324);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5322);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5314);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5312);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5303);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5301);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5242);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5232);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5222);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5201);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5142);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5132);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5122);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-5101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4444);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4434);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4432);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4424);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4422);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4414);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4412);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4403);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4334);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4332);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4324);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4322);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4314);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4312);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4303);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4301);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4232);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4222);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4201);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4132);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4122);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-4101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3334);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3324);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3322);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3314);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3312);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3303);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3222);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3201);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3122);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-3101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-2101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-1114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-1103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-545);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-543);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-541);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-535);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-533);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-531);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-525);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-523);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-521);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-515);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-513);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-511);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-435);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-433);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-431);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-425);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-423);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-421);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-415);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-413);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-411);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-325);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-323);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-321);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-315);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-313);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-311);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-215);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-213);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(-211);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(111);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(113);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(115);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(130);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(211);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(213);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(215);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(221);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(223);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(225);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(310);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(311);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(313);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(315);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(321);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(323);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(325);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(331);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(333);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(335);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(411);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(413);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(415);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(421);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(423);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(425);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(431);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(433);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(435);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(441);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(443);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(445);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(511);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(513);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(515);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(521);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(523);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(525);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(531);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(533);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(535);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(541);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(543);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(545);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(551);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(553);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(555);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(1103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(1114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(2224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3122);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3201);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3222);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3303);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3312);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3314);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3322);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3324);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(3334);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4122);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4132);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4201);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4222);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4232);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4301);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4303);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4312);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4314);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4322);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4324);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4332);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4334);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4403);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4412);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4414);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4422);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4424);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4432);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4434);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(4444);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5101);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5103);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5112);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5114);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5122);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5132);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5142);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5201);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5203);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5212);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5214);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5222);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5224);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5232);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5242);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5301);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5303);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5312);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5314);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5322);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5324);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5332);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5334);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5342);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5401);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5403);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5412);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5414);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5422);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5424);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5432);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5434);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5442);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5444);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5503);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5512);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5514);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5522);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5524);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5532);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5534);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5542);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5544);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(5554);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10111);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10113);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10211);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10213);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10221);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10223);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10311);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10313);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10321);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10323);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10331);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10333);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10411);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10413);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10421);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10423);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10431);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10433);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10441);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10443);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10511);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10513);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10521);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10523);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10531);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10533);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10541);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10543);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10551);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(10553);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20113);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20213);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20223);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20313);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20323);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20333);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20413);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20423);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20433);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20443);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20513);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20523);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20533);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20543);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(20553);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(100443);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(100553);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(9900440);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(9900441);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(9900443);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(9900551);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(9900553);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(9910441);\n')
        file.write('  PHYSICS->mcConfig().AddHadronicId(9910551);\n')
        file.write('\n')
        file.write('  // definition of the multiparticle "invisible"\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(-16);\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(-14);\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(-12);\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(12);\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(14);\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(16);\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(1000022);\n')
        file.write('  PHYSICS->mcConfig().AddInvisibleId(1000039);\n')
        for item in main.multiparticles.Get("invisible"):
          if item not in [-16,-14,-12,12,14,16,1000022,1000039]:
            file.write('  PHYSICS->mcConfig().AddInvisibleId('+str(item)+');\n')
        file.write('\n')


    # recConfig initialization
    if main.mode==MA5RunningType.RECO:
        file.write('  // Initializing PhysicsService for RECO\n') 
        file.write('  PHYSICS->recConfig().Reset();\n\n')
        if main.isolation.algorithm=='cone':
            file.write('  PHYSICS->recConfig().UseDeltaRIsolation('+\
                       str(main.isolation.isolation.radius) + ');\n')
        else:
            file.write('  PHYSICS->recConfig().UseSumPTIsolation('+\
                       str(main.isolation.isolation.sumPT) + ',' +\
                       str(main.isolation.isolation.ET_PT) + ');\n')
        file.write('\n')

    # Region initiatization
    file.write('  // ===== Signal region ===== //\n')
    if main.regions.GetNames() == []:
        file.write('  Manager()->AddRegionSelection(\"myregion\");\n');
    else:
        for reg in main.regions.GetNames():
            file.write('  Manager()->AddRegionSelection(\"' + reg + '\");\n');
    file.write('\n')

    # Cut initiatization
    counter = 0
    file.write('  // ===== Selections ===== //\n')
    for item in main.selection.table:
        if item.__class__.__name__=="Cut":
            if len(item.part)==0:
                counter+=1;
                if len(item.regions)!=len(main.regions):
                    if len(item.regions)!=1:
                        file.write('  std::string RNc'+str(counter)+'[]={'+\
                            (', '.join('"'+reg+'"' for reg in item.regions))+'};\n')
                        file.write('  Manager()->AddCut(\"' + str(counter) + '_' + item.conditions.GetStringDisplay() +\
                            '\", RNc'+ str(counter)+');\n');
                    else:
                        file.write('  Manager()->AddCut(\"' + str(counter) + '_' + item.conditions.GetStringDisplay() + '\", '+\
                           '\"'+item.regions[0]+'\");\n');
                else:
                    file.write('  Manager()->AddCut(\"' + str(counter) + '_' + item.conditions.GetStringDisplay() + '\");\n');
    file.write('\n')

    # Histo initiatization
    counter = 0
    file.write('  // ===== Histograms ===== //\n')
    for item in main.selection.table:
        if item.__class__.__name__=="Histogram":
            counter+=1;
            if len(item.regions)!=len(main.regions):
                if len(item.regions)>1:
                    file.write('  std::string RNh'+str(counter)+'[]={'+\
                        (', '.join('"'+reg+'"' for reg in item.regions))+'};\n')
                    # NPID and NAPID
                    if item.observable.name in ["NPID", "NAPID"] :
                        file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +\
                            '\", RNh'+ str(counter)+');\n');
                    # Histo with LogX
                    elif item.logX:
                        file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                            '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', RNh'+ str(counter)+');\n');
                    # Normal Histo
                    else:
                        file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name +\
                            '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', RNh'+ str(counter)+');\n');
                elif len(item.regions)==1:
                    # NPID and NAPID
                    if item.observable.name in ["NPID", "NAPID"] :
                        file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +\
                            '\", \"'+item.regions[0]+'\");\n');
                    # Histo with LogX
                    elif item.logX:
                        file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                          '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', \"'+item.regions[0]+'\");\n');
                    # Normal Histo
                    else:
                        file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name + '\", '+\
                           str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+', \"'+item.regions[0]+'\");\n');
                else:
                    # NPID and NAPID
                    if item.observable.name in ["NPID", "NAPID"] :
                        file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +');\n');
                    # Histo with LogX
                    elif item.logX:
                        file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                          '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
                    # Normal Histo
                    else:
                        file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name + '\", '+\
                           str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
            else:
               # NPID and NAPID
               if item.observable.name in ["NPID", "NAPID"] :
                   file.write('  Manager()->AddHistoFrequency(\"' + str(counter) + '_' + item.observable.name +'\");\n');
               # Histo with LogX
               elif item.logX:
                   file.write('  Manager()->AddHistoLogX(\"' + str(counter) + '_' + item.observable.name +\
                     '\", ' + str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
               # Normal Histo
               else:
                    file.write('  Manager()->AddHisto(\"' + str(counter) + '_' + item.observable.name + '\", ' +\
                      str(item.nbins)+','+ str(item.xmin)+','+ str(item.xmax)+');\n');
    file.write('\n')

    # End
    file.write('  // No problem during initialization\n')
    file.write('  return true;\n')
    file.write('}\n\n')
