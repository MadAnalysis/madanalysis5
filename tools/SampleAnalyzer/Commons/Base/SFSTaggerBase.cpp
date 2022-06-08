////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
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
//  You should have const Received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//
////////////////////////////////////////////////////////////////////////////////

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/SFSTaggerBase.h"
#include "SampleAnalyzer/Commons/Service/RandomService.h"
#include "SampleAnalyzer/Commons/Service/Physics.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"

namespace MA5 {

    void SFSTaggerBase::Execute(EventFormat &myEvent) const
    {
        /// Shortcut for global event variables
        MAfloat64 & THT  = myEvent.rec()->THT();
        MAfloat64 & Meff = myEvent.rec()->Meff();

        /// Truth level B/C tagging
        BJetTagging(myEvent);
        CJetTagging(myEvent);

        /// Run tau tagging
        if (_options.tautag_jetbased) JetBasedTauTagging(myEvent);

        /// Get initial number of taus.
        /// @attention if tau tagging is jet based Ntau will be zero
        MAuint32 Ntau = myEvent.rec()->taus().size();

        /// Jet tagging with detector effects
        std::vector<MAuint32> toRemove;
        MAuint32 ijet = -1;
        if (_isJetTaggingOn || _options.tautag_jetbased) {
            for (auto &jet: myEvent.rec()->jets())
            {
                ijet++;
                if (jet.tautag())
                {
                    if (RANDOM->flat() < tau_tagging_eff(jet))
                    {
                        RecTauFormat *newTau = myEvent.rec()->GetNewTau();
                        Jet2Tau(&jet, newTau, myEvent);
                        toRemove.push_back(ijet);
                        continue;
                    }
                    /// @attention This is for consistency. Tau tag is created for this application only.
                    jet.setTautag(false);
                }
                if (!_isJetTaggingOn) continue;

                /// We have a true b-jet: is it b-tagged?
                if (jet.true_btag())
                {
                    if (RANDOM->flat() > b_tagging_eff(jet)) jet.setBtag(false);
                }
                /// We have a true c-jet: is it b-tagged?
                else if (jet.true_ctag())
                {
                    if (RANDOM->flat() < c_mistag_b(jet)) jet.setBtag(true);
                }
                /// We have a true light-jet: is it b-tagged?
                else
                {
                    if (RANDOM->flat() < lightjet_mistag_b(jet)) jet.setBtag(true);
                }

                /// We have a b-tagged jet -> moving on with the next jet
                if (jet.btag()) continue;

                /// We have a true b-jet: is it c-tagged?
                if (jet.true_btag())
                {
                    if (RANDOM->flat() < b_mistag_c(jet)) { jet.setCtag(true); jet.setBtag(false); }
                }
                /// We have a true c-jet: is it c-tagged?
                else if (jet.true_ctag())
                {
                    if (RANDOM->flat() > c_tagging_eff(jet)) jet.setCtag(false);
                }
                /// We have a true light-jet: is it c-tagged?
                else
                {
                    if (RANDOM->flat() < lightjet_mistag_c(jet)) jet.setCtag(true);
                }

                /// We have a c-tagged jet -> moving on with the next jet
                if (jet.ctag()) continue;

                /// We have a true b/c-jet -> cannot be mistagged
                if (jet.ctag() || jet.btag()) continue;
                /// if not, is it mis-tagged as anything?
                else
                {
                    { /// Scope for light jet mistagging as tau
                        /// if not, is it Tau-tagged?
                        if (RANDOM->flat() < lightjet_mistag_tau(jet)) {
                            RecTauFormat *newTau = myEvent.rec()->GetNewTau();
                            Jet2Tau(&jet, newTau, myEvent);
                            toRemove.push_back(ijet);
                            continue;
                        }
                    }
                    { /// Scope for light jet mistagging for electron
                        /// if not, is it Electron-tagged?
                        if (RANDOM->flat() < lightjet_mistag_electron(jet))
                        {
                            RecLeptonFormat* NewParticle = myEvent.rec()->GetNewElectron();
                            NewParticle->setMomentum(jet.momentum());
                            NewParticle->setMc(jet.mc());
                            /// @attention charge can also be determined via total constituent charge
                            NewParticle->SetCharge(RANDOM->flat() > 0.5 ? 1. : -1.);
                            THT  -= jet.pt();
                            Meff -= jet.pt();
                            MALorentzVector MissHT = myEvent.rec()->MHT().momentum() + jet.momentum();
                            (&myEvent.rec()->MHT().momentum())->SetPxPyPzE(
                                MissHT.Px(), MissHT.Py(), 0., MissHT.E()
                            );
                            toRemove.push_back(ijet);
                        }
                    }
                    { /// Scope for light jet mistagging for photon
                        /// if not, is it Photon-tagged?
                        if (RANDOM->flat() < lightjet_mistag_photon(jet))
                        {
                            RecPhotonFormat* NewParticle = myEvent.rec()->GetNewPhoton();
                            NewParticle->setMomentum(jet.momentum());
                            NewParticle->setMc(jet.mc());
                            THT  -= jet.pt();
                            Meff -= jet.pt();
                            MALorentzVector MissHT = myEvent.rec()->MHT().momentum() + jet.momentum();
                            (&myEvent.rec()->MHT().momentum())->SetPxPyPzE(
                                MissHT.Px(), MissHT.Py(), 0., MissHT.E()
                            );
                            toRemove.push_back(ijet);
                            continue;
                        }
                    }
                }
            }
            /// Remove jets from the collection
            for (MAuint32 i = toRemove.size(); i > 0; i--)
                myEvent.rec()->jets().erase(myEvent.rec()->jets().begin() + toRemove[i-1]);
            toRemove.clear();
        }


        if (_isTauTaggingEffOn)
        {
            /// @attention In Jet based tau tagging this loop will not run
            for (MAuint32 itau = 0; itau < Ntau; itau++)
            {
                if (RANDOM->flat() > tau_tagging_eff(myEvent.rec()->taus()[itau]))
                {
                    RecJetFormat* NewParticle = myEvent.rec()->GetNewJet();
                    NewParticle->setMomentum((&myEvent.rec()->taus()[itau])->momentum());
                    NewParticle->setMc((&myEvent.rec()->taus()[itau])->mc());
                    NewParticle->setNtracks((&myEvent.rec()->taus()[itau])->ntracks());
                    toRemove.push_back(itau);
                }
            }
            /// Remove taus from the collection
            for (MAuint32 i=toRemove.size();i>0;i--)
                myEvent.rec()->taus().erase(myEvent.rec()->taus().begin() + toRemove[i-1]);
            toRemove.clear();
        }


        /// Muon mistagging
        if (_isMuonTaggingOn)
        {
            MAuint32 imu = -1;
            for (auto &muon: myEvent.rec()->muons())
            {
                imu++;
                /// Muon mistagging as electron
                if (RANDOM->flat() < muon_mistag_electron(muon))
                {
                    RecLeptonFormat* NewParticle = myEvent.rec()->GetNewElectron();
                    NewParticle->setMomentum(muon.momentum());
                    NewParticle->setMc(muon.mc());
                    NewParticle->SetCharge(muon.charge());
                    toRemove.push_back(imu);
                    continue;
                }
                /// Muon mistagging as photon
                if (RANDOM->flat() < muon_mistag_photon(muon))
                {
                    RecPhotonFormat* NewParticle = myEvent.rec()->GetNewPhoton();
                    NewParticle->setMomentum(muon.momentum());
                    NewParticle->setMc(muon.mc());
                    toRemove.push_back(imu);
                    continue;
                }
                /// Muon mistagging as light jet
                /// @attention this will cause problems if executed in substructure tools
                if (RANDOM->flat() < muon_mistag_lightjet(muon))
                {
                    RecJetFormat* NewParticle = myEvent.rec()->GetNewJet();
                    NewParticle->setMomentum(muon.momentum());
                    NewParticle->setMc(muon.mc());
                    NewParticle->setNtracks(1);
                    THT  += muon.pt();
                    Meff += muon.pt();
                    MALorentzVector MissHT = myEvent.rec()->MHT().momentum() - muon.momentum();
                    (&myEvent.rec()->MHT().momentum())->SetPxPyPzE(
                        MissHT.Px(), MissHT.Py(), 0., MissHT.E()
                    );
                    toRemove.push_back(imu);
                    continue;
                }
            }
            for (MAuint32 i=toRemove.size();i>0;i--)
                myEvent.rec()->muons().erase(myEvent.rec()->muons().begin() + toRemove[i-1]);
            toRemove.clear();
        }


        /// Electron mistagging
        if (_isElectronTaggingOn)
        {
            /// @todo complete this section
        }


        /// Photon mistaging
        if (_isPhotonTaggingOn)
        {
            /// @todo complete this section
        }
    }

    /// @attention BJetTagging and CJetTagging methods do not include any detector effects
    /// this is due to the construction of the for loop over jets. If one applies detector
    /// efficiencies at this state an efficiency can be applied to a jet multiple times.
    /// Since one jet can contains multiple B/C hadrons within its cone.

    /// Truth B-Jet tagging
    void SFSTaggerBase::BJetTagging(EventFormat &myEvent) const
    {
        for (auto &bHadron: myEvent.rec()->MCBquarks_)
        {
            MAfloat32 DeltaRmax = _options.btag_matching_deltaR;
            MAuint32 current_ijet = -1;
            for (MAuint32 ijet = 0; ijet < myEvent.rec()->jets().size(); ijet++)
            {
                MAfloat32 dR = myEvent.rec()->jets()[ijet].dr(bHadron);
                if (dR <= DeltaRmax)
                {
                    if (_options.btag_exclusive)
                    {
                        current_ijet = ijet; DeltaRmax = dR;
                    }
                    else
                    {
                        myEvent.rec()->jets()[ijet].setTrueBtag(true);
                        myEvent.rec()->jets()[ijet].setBtag(true);
                    }
                }
            }
            if (current_ijet >= 0)
            {
                myEvent.rec()->jets()[current_ijet].setTrueBtag(true);
                myEvent.rec()->jets()[current_ijet].setBtag(true);
            }
        }
    }

    /// Truth C-Jet tagging
    void SFSTaggerBase::CJetTagging(EventFormat &myEvent) const
    {
        for (auto &cHadron: myEvent.rec()->MCCquarks_)
        {
            MAfloat32 DeltaRmax = _options.ctag_matching_deltaR;
            MAuint32 current_ijet = -1;
            for (MAuint32 ijet = 0; ijet < myEvent.rec()->jets().size(); ijet++)
            {
                MAfloat32 dR = myEvent.rec()->jets()[ijet].dr(cHadron);
                if (dR <= DeltaRmax)
                {
                    if (_options.ctag_exclusive)
                    {
                        current_ijet = ijet; DeltaRmax = dR;
                    }
                    else
                    {
                        myEvent.rec()->jets()[ijet].setTrueCtag(true);
                        myEvent.rec()->jets()[ijet].setCtag(true);
                    }
                }
            }
            if (current_ijet >= 0)
            {
                myEvent.rec()->jets()[current_ijet].setTrueCtag(true);
                myEvent.rec()->jets()[current_ijet].setCtag(true);
            }
        }
    }

    /// This method implements tau matching for jets
    void SFSTaggerBase::JetBasedTauTagging(EventFormat &myEvent) const
    {
        for (auto &hadronicTau: myEvent.rec()->MCHadronicTaus())
        {
            MAfloat32 DeltaRmax = _options.tautag_matching_deltaR;
            MAuint32 current_jet = -1;
            for (MAuint32 ijet = 0; ijet < myEvent.rec()->jets().size(); ijet++)
            {
                if (myEvent.rec()->jets()[ijet].true_ctag() || myEvent.rec()->jets()[ijet].true_btag())
                    continue;

                MAfloat32 dR = myEvent.rec()->jets()[ijet].dr(hadronicTau);
                if (dR <= DeltaRmax)
                {
                    if (_options.tautag_exclusive)
                    {
                        DeltaRmax = dR; current_jet = ijet;
                    }
                    else myEvent.rec()->jets()[current_jet].setTautag(true);
                }
            }
            if (current_jet >= 0) myEvent.rec()->jets()[current_jet].setTautag(true);
        }
    }

    /// Convert Jet object to tau object
    void SFSTaggerBase::Jet2Tau(const RecJetFormat * myJet, RecTauFormat *myTau, EventFormat &myEvent) const
    {
        myTau->setMomentum(myJet->momentum());
        myTau->ntracks_   = myJet->ntracks();
        myTau->mc_        = myJet->mc_;
        myTau->DecayMode_ = PHYSICS->GetTauDecayMode(myTau->mc_);
        myTau->pseudojet_ = myJet->pseudojet_;

        MAint32 charge = 0;
        myTau->Constituents_.reserve(myJet->Constituents_.size());
        myTau->Constituents_.insert(
            myTau->Constituents_.end(), myJet->Constituents_.begin(), myJet->Constituents_.end()
        );
        for (auto &constit: myTau->Constituents_)
            charge += PDG->GetCharge(myEvent.mc()->particles()[constit].pdgid());

        myTau->charge_ = charge > 0 ? true : false;
    }

}