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

    /// Print parameters
    void SFSTaggerBase::PrintParam() const
    {
        /// Print B-taggging options
        std::string excl = _options.btag_exclusive ? "Exclusive" : "Inclusive";
        INFO << "        with bjet: matching ΔR = "
             << _options.btag_matching_deltaR
             << " ; " << excl << endmsg;
        excl = _options.ctag_exclusive ? "Exclusive" : "Inclusive";

        /// Print C-tagging options
        if (_options.enable_ctagging) {
            INFO << "        with cjet: matching ΔR = "
                 << _options.ctag_matching_deltaR
                 << " ; " << excl << endmsg;
        }

        /// Print Tau-tagging options
        if (_options.tautag_jetbased) {
            excl = _options.ctag_exclusive ? "Exclusive" : "Inclusive";
            INFO << "        with tau : matching ΔR = "
                 << _options.tautag_matching_deltaR
                 << " ; " << excl << endmsg;
        } else {
            INFO << "        with tau : hadron-based tagging" << endmsg;
        }
    }

    /// Convert jet to tau
    MAfloat32 SFSTaggerBase::tau_tagging_eff(const RecJetFormat &jet, TaggerStatus status) const
    {
        RecTauFormat myTau;
        myTau.setMomentum(jet.momentum());
        myTau.ntracks_   = jet.ntracks();
        myTau.mc_        = jet.mc_;

        if (status == TaggerStatus::MID) return mid_tau_tagging_eff(myTau);
        else if (status == TaggerStatus::TIGHT) return tight_tau_tagging_eff(myTau);
        else return loose_tau_tagging_eff(myTau);
    }

    /// Execute tagger
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
        std::vector<MAuint32> toRemove;
        /// Jet tagging with detector effects
        if (_isJetTaggingOn)
        {
            MAint32 ijet = -1;
            for (auto &jet: myEvent.rec()->jets())
            {
                ijet++;
                MAfloat64 flat = RANDOM->flat();
                /// We have a true b-jet: is it b-tagged?
                if (jet.true_btag())
                {
                    if (flat > loose_b_tagging_eff(jet)) jet.setLooseBtag(false);
                    if (flat > mid_b_tagging_eff(jet)) jet.setMidBtag(false);
                    if (flat > tight_b_tagging_eff(jet)) jet.setTightBtag(false);
                }
                /// We have a true c-jet: is it b-tagged?
                else if (jet.true_ctag())
                {
                    if (flat < loose_c_mistag_b(jet)) jet.setLooseBtag(true);
                    if (flat < mid_c_mistag_b(jet)) jet.setMidBtag(true);
                    if (flat < tight_c_mistag_b(jet)) jet.setTightBtag(true);
                }
                /// We have a true light-jet: is it b-tagged?
                else
                {
                    if (flat < lightjet_mistag_b_loose(jet)) jet.setLooseBtag(true);
                    if (flat < lightjet_mistag_b_mid(jet)) jet.setMidBtag(true);
                    if (flat < lightjet_mistag_b_tight(jet)) jet.setTightBtag(true);
                }

                /// We have a true b-jet: is it c-tagged?
                if (jet.true_btag() && !jet.loose_btag())
                {
                    /// We have a b-tagged jet -> moving on with the next jet
                    if (flat < loose_b_mistag_c(jet)) jet.setLooseCtag(true);
                    if (flat < mid_b_mistag_c(jet)) jet.setMidCtag(true);
                    if (flat < tight_b_mistag_c(jet)) jet.setTightCtag(true);
                }
                /// We have a true c-jet: is it c-tagged?
                else if (jet.true_ctag() && !jet.loose_btag())
                {
                    if (flat > loose_c_tagging_eff(jet)) jet.setLooseCtag(false);
                    if (flat > mid_c_tagging_eff(jet)) jet.setMidCtag(false);
                    if (flat > tight_c_tagging_eff(jet)) jet.setTightCtag(false);
                }
                /// We have a true light-jet: is it c-tagged?
                else if (!jet.loose_btag() && !jet.loose_ctag())
                {
                    if (flat < lightjet_mistag_c_loose(jet)) jet.setLooseCtag(true);
                    if (flat < lightjet_mistag_c_mid(jet)) jet.setMidCtag(true);
                    if (flat < lightjet_mistag_c_tight(jet)) jet.setTightCtag(true);
                }

                if (_options.tautag_jetbased)
                {
                    if (jet.true_tautag() && !jet.loose_btag() && !jet.loose_ctag())
                    {
                        if (flat > tau_tagging_eff(jet, LOOSE)) jet.setLooseTautag(false);
                        if (flat > tau_tagging_eff(jet, MID)) jet.setMidTautag(false);
                        if (flat > tau_tagging_eff(jet, TIGHT)) jet.setTightTautag(false);
                    }
                    else if (!jet.loose_btag() && !jet.loose_ctag())
                    {
                        if (flat < lightjet_mistag_tau_loose(jet)) jet.setLooseTautag(true);
                        if (flat < lightjet_mistag_tau_mid(jet)) jet.setMidTautag(true);
                        if (flat < lightjet_mistag_tau_tight(jet)) jet.setTightTautag(true);
                    }
                }

                /// We have a true b/c-jet -> cannot be mistagged
                /// @attention Loose tag is always the default
                if (jet.loose_ctag() || jet.loose_btag() || jet.loose_tautag()) continue;

                /// if not, is it mis-tagged as anything?
                else
                {
                    /// Scope for light jet mistagging as tau
                    if (!_options.tautag_jetbased) {
                        /// if not, is it Tau-tagged?
                        if (flat < lightjet_mistag_tau_loose(jet)) {
                            RecTauFormat *newTau = myEvent.rec()->GetNewTau();
                            Jet2Tau(&jet, newTau, myEvent);
                            toRemove.push_back(ijet);
                            continue;
                        }
                    }
                    /// Scope for light jet mistagging for electron
                    {
                        /// if not, is it Electron-tagged?
                        if (flat < lightjet_mistag_electron(jet))
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
                    /// Scope for light jet mistagging for photon
                    {
                        /// if not, is it Photon-tagged?
                        if (flat < lightjet_mistag_photon(jet))
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
        }
        /// Remove jets from the collection
        for (MAuint32 i = toRemove.size(); i > 0; i--)
            myEvent.rec()->jets().erase(myEvent.rec()->jets().begin() + toRemove[i-1]);
        toRemove.clear();

        if (_isTauTaggingEffOn && !_options.tautag_jetbased)
        {
            /// @attention In Jet based tau tagging this loop will not run. If its runnning thats a bug
            for (MAuint32 itau = 0; itau < Ntau; itau++)
            {
                if (RANDOM->flat() > loose_tau_tagging_eff(myEvent.rec()->taus()[itau]))
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
                /// @warning this will cause problems if executed in substructure tools
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
            MAuint32 ielec = -1;
            for (auto &electron: myEvent.rec()->electrons())
            {
                ielec++;
                /// Electron mistagging as muon
                if (RANDOM->flat() < electron_mistag_muon(electron))
                {
                    RecLeptonFormat* NewParticle = myEvent.rec()->GetNewMuon();
                    NewParticle->setMomentum(electron.momentum());
                    NewParticle->setMc(electron.mc());
                    NewParticle->SetCharge(electron.charge());
                    toRemove.push_back(ielec);
                    continue;
                }
                /// Electron mistagging as photon
                if (RANDOM->flat() < electron_mistag_photon(electron))
                {
                    RecPhotonFormat* NewParticle = myEvent.rec()->GetNewPhoton();
                    NewParticle->setMomentum(electron.momentum());
                    NewParticle->setMc(electron.mc());
                    toRemove.push_back(ielec);
                    continue;
                }
                /// Electron mistagging as jet
                /// @warning This will cause problems during execution with substructure module
                if (RANDOM->flat() < electron_mistag_lightjet(electron))
                {
                    RecJetFormat* NewParticle = myEvent.rec()->GetNewJet();
                    NewParticle->setMomentum(electron.momentum());
                    NewParticle->setMc(electron.mc());
                    NewParticle->setNtracks(1);
                    THT    += electron.pt();
                    Meff   += electron.pt();
                    MALorentzVector MissHT = myEvent.rec()->MHT().momentum() - electron.momentum();
                    (&myEvent.rec()->MHT().momentum())->SetPxPyPzE(
                        MissHT.Px(), MissHT.Py(), 0., MissHT.E()
                    );
                    toRemove.push_back(ielec);
                    continue;
                }
            }
            /// Remove mistagged electrons
            for (MAuint32 i=toRemove.size();i>0;i--)
                myEvent.rec()->electrons().erase(myEvent.rec()->electrons().begin() + toRemove[i-1]);
            toRemove.clear();
        }


        /// Photon mistaging
        if (_isPhotonTaggingOn)
        {
            MAuint32 iph = -1;
            for (auto &photon: myEvent.rec()->photons())
            {
                iph++;
                /// Photon mistagging as electron
                if (RANDOM->flat() < photon_mistag_electron(photon))
                {
                    RecLeptonFormat* NewParticle = myEvent.rec()->GetNewElectron();
                    NewParticle->setMomentum(photon.momentum());
                    NewParticle->setMc(photon.mc());
                    NewParticle->SetCharge(RANDOM->flat() > 0.5 ? 1. : -1.);
                    toRemove.push_back(iph);
                    continue;
                }
                /// Photon mistagging as muon
                if (RANDOM->flat() < photon_mistag_muon(photon))
                {
                    RecLeptonFormat* NewParticle = myEvent.rec()->GetNewMuon();
                    NewParticle->setMomentum(photon.momentum());
                    NewParticle->setMc(photon.mc());
                    NewParticle->SetCharge(RANDOM->flat() > 0.5 ? 1. : -1.);
                    toRemove.push_back(iph);
                    continue;
                }
                /// Photon mistagging as jet
                /// @warning This will cause problems during execution with substructure module
                if (RANDOM->flat() < photon_mistag_lightjet(photon))
                {
                    RecJetFormat* NewParticle = myEvent.rec()->GetNewJet();
                    NewParticle->setMomentum(photon.momentum());
                    NewParticle->setMc(photon.mc());
                    NewParticle->setNtracks(1);
                    THT    += photon.pt();
                    Meff   += photon.pt();
                    MALorentzVector MissHT = myEvent.rec()->MHT().momentum() - photon.momentum();
                    (&myEvent.rec()->MHT().momentum())->SetPxPyPzE(
                        MissHT.Px(), MissHT.Py(), 0., MissHT.E()
                    );
                    toRemove.push_back(iph);
                    continue;
                }
            }
            /// Remove mistagged photons
            for (MAuint32 i=toRemove.size();i>0;i--)
                myEvent.rec()->photons().erase(myEvent.rec()->photons().begin() + toRemove[i-1]);
            toRemove.clear();
        }
    }

    /// @attention BJetTagging and CJetTagging methods do not include any detector effects
    /// this is due to the construction of the for loop over jets. If one applies detector
    /// efficiencies at this state an efficiency can be applied to a jet multiple times.
    /// Since one jet can contains multiple B/C hadrons within its cone.

    /// Truth B-Jet tagging
    void SFSTaggerBase::BJetTagging(EventFormat &myEvent) const
    {
        /// Loop over B-hadrons
        for (auto &bHadron: myEvent.rec()->MCBquarks_)
        {
            MAfloat32 DeltaRmax = _options.btag_matching_deltaR;
            /// @attention If not exclusive `current_ijet` will always be -1
            /// thus jets will only be tagged with respect to dR
            MAint32 current_ijet = -1;
            /// Loop over jets
            for (MAuint32 ijet = 0; ijet < myEvent.rec()->jets().size(); ijet++)
            {
                MAfloat32 dR = myEvent.rec()->jets()[ijet].dr(bHadron);
                if (dR <= DeltaRmax)
                {
                    if (_options.btag_exclusive) { current_ijet = ijet; DeltaRmax = dR; }
                    else myEvent.rec()->jets()[ijet].setAllBtags(true);
                }
            }
            if (current_ijet >= 0) myEvent.rec()->jets()[current_ijet].setAllBtags(true);
        }
    }

    /// Truth C-Jet tagging
    void SFSTaggerBase::CJetTagging(EventFormat &myEvent) const
    {
        /// Loop over C-hadrons
        for (auto &cHadron: myEvent.rec()->MCCquarks_)
        {
            MAfloat32 DeltaRmax = _options.ctag_matching_deltaR;
            /// @attention If not exclusive `current_ijet` will always be -1
            /// thus jets will only be tagged with respect to dR
            MAint32 current_ijet = -1;

            /// Loop over jets
            for (MAuint32 ijet = 0; ijet < myEvent.rec()->jets().size(); ijet++)
            {
                if (myEvent.rec()->jets()[ijet].true_btag()) continue;
                MAfloat32 dR = myEvent.rec()->jets()[ijet].dr(cHadron);
                if (dR <= DeltaRmax)
                {
                    if (_options.ctag_exclusive) { current_ijet = ijet; DeltaRmax = dR; }
                    else {
                        if (_options.enable_ctagging)
                            myEvent.rec()->jets()[ijet].setAllCtags(true);
                        else myEvent.rec()->jets()[ijet].setTrueCtag(true);
                    }
                }
            }
            if (current_ijet >= 0) {
                if (_options.enable_ctagging)
                    myEvent.rec()->jets()[current_ijet].setAllCtags(true);
                else myEvent.rec()->jets()[current_ijet].setTrueCtag(true);
            }
        }
    }

    /// This method implements tau matching for jets
    void SFSTaggerBase::JetBasedTauTagging(EventFormat &myEvent) const
    {
        for (auto &hadronicTau: myEvent.rec()->MCHadronicTaus())
        {
            MAfloat32 DeltaRmax = _options.tautag_matching_deltaR;
            MAint32 current_jet = -1;
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
                    else myEvent.rec()->jets()[current_jet].setAllTautags(true);
                }
            }
            if (current_jet >= 0) myEvent.rec()->jets()[current_jet].setAllTautags(true);
        }
    }

    /// Convert Jet object to tau object
    void SFSTaggerBase::Jet2Tau(const RecJetFormat * myJet, RecTauFormat *myTau, EventFormat &myEvent) const
    {
        myTau->setMomentum(myJet->momentum());
        myTau->setNtracks(myJet->ntracks());
        myTau->setMc(myJet->mc_);
        myTau->setDecayMode(PHYSICS->GetTauDecayMode(myTau->mc_));
#ifdef MA5_FASTJET_MODE
        myTau->setPseudoJet(myJet->pseudojet_);
#endif

        MAint32 charge = 0;
        myTau->Constituents_.reserve(myJet->Constituents_.size());
        myTau->Constituents_.insert(
            myTau->Constituents_.end(), myJet->Constituents_.begin(), myJet->Constituents_.end()
        );
        for (auto &constit: myTau->Constituents_)
            charge += PDG->GetCharge(myEvent.mc()->particles()[constit].pdgid());

        myTau->setCharge(charge > 0);
    }

}
