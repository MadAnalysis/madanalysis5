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


class InstanceName():
    table = {}

    @staticmethod
    def Find(name):
        if name in InstanceName.table.keys():
            return True
        return False

    @staticmethod
    def Clear():
        InstanceName.table.clear()
        
    @staticmethod
    def Get(name):
        if name in InstanceName.table.keys():
            return InstanceName.table[name]
        else:
            newname="_"+name.lstrip()
            newname=newname.replace('+','_p')
            newname=newname.replace('-','_m')
            newname=newname.replace('~','_t')
            newname=newname.replace(' ','_')
            newname=newname.replace('<','_l')
            newname=newname.replace('[','_I')
            newname=newname.replace(']','I_')
            for item in newname:
                if not (item.isalpha() or item.isdigit() or item=="_"):
                    item="X"

            if newname in InstanceName.table.values():
                parts = newname.split('__')
                try:
                    index=int(parts[-1])
                    parts[-1]=str(index+1)
                    newname='__'.join(parts)
                except:
                    newname+='__1'
            InstanceName.table[name]=newname
            return newname
                
