////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


#ifndef NULLSMEARER_H
#define NULLSMEARER_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"


namespace MA5
{
    class NullSmearer: public SmearerBase {

        private:
            MCParticleFormat output_;
        public:
            /// Constructor without argument
            NullSmearer() {}
            /// Destructor
            ~NullSmearer() {}

            // For all methods below, the only relevant part of the output object is the momentum
            // The reset allows to clear the left-over from the previous object

            // Electron smearing method
            MCParticleFormat ElectronSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }

            // Muon smearing method
            MCParticleFormat MuonSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }

            // Hadronic Tau smearing method
            MCParticleFormat TauSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }

            // Photon smearing method
            MCParticleFormat PhotonSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }


            // Jet smearing method
            MCParticleFormat JetSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }


            // Jet Constituent smearing method
            MCParticleFormat ConstituentSmearer(const MCParticleFormat * part)
            {
                output_.Reset();
                output_.momentum().SetPxPyPzE(part->px(),part->py(),part->pz(),part->e());
                return output_;
            }

            void Print()
            {
                DEBUG << "Null Smearer" << endmsg;
            }
    };
}

#endif
