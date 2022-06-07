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

    // Truth B-Jet tagging
    void SFSTaggerBase::BJetTagging(EventFormat &myEvent) const
    {
        for (auto &bHadron: myEvent.rec()->MCBquarks_)
        {
            MAfloat32 DeltaRmax = _options.btag_matching_deltaR;
            RecJetFormat* current_jet = 0;
            for (auto &jet: myEvent.rec()->jets())
            {
                MAfloat32 dR = jet.dr(bHadron);
                if (dR <= DeltaRmax)
                {
                    if (_options.btag_exclusive)
                    {
                        current_jet = &jet; DeltaRmax = dR;
                    }
                    else jet.setTrueBtag(true);
                }
            }
            if (_options.btag_exclusive && current_jet != 0) current_jet->setTrueBtag(true);
        }
    }

    // Truth C-Jet tagging
    void SFSTaggerBase::CJetTagging(EventFormat &myEvent) const
    {
        for (auto &cHadron: myEvent.rec()->MCCquarks_)
        {
            MAfloat32 DeltaRmax = _options.ctag_matching_deltaR;
            RecJetFormat* current_jet = 0;
            for (auto &jet: myEvent.rec()->jets())
            {
                MAfloat32 dR = jet.dr(cHadron);
                if (dR <= DeltaRmax)
                {
                    if (_options.ctag_exclusive)
                    {
                        current_jet = &jet; DeltaRmax = dR;
                    }
                    else jet.setTrueCtag(true);
                }
            }
            if (_options.ctag_exclusive && current_jet != 0) current_jet->setTrueCtag(true);
        }
    }

    // Truth Hadronic tau tagging (only exclusive)
    void SFSTaggerBase::TauTagging(EventFormat &myEvent) const
    {
        if (_options.tautag_jetbased)
        {
            for (auto &hadronicTau: myEvent.rec()->MCHadronicTaus())
            {
                if (RANDOM->flat() < tau_tagging_eff(*hadronicTau)) continue;

                MAfloat32 DeltaRmax = _options.tautag_matching_deltaR;
                MAuint32 toRemove = -1;
                for (MAuint32 ijet = 0; ijet < myEvent.rec()->jets().size(); ijet++)
                {
                    if (myEvent.rec()->jets()[ijet].true_ctag() || myEvent.rec()->jets()[ijet].true_btag())
                        continue;

                    MAfloat32 dR = myEvent.rec()->jets()[ijet].dr(hadronicTau);
                    if (dR <= DeltaRmax) { DeltaRmax = dR; toRemove = ijet; }
                }
                if (toRemove >= 0)
                {
                    RecTauFormat* myTau;
                    Jet2Tau(&myEvent.rec()->jets()[toRemove], myTau, myEvent);
                    if (RANDOM->flat() < tau_tagging_eff(myTau))
                    {
                        myEvent.rec()->jets().erase(myEvent.rec()->jets().begin() + toRemove);
                        myEvent.rec()->taus_.push_back(*myTau);
                    }
                }
            }
        }
        else
        {
            std::vector<MAuint32> toRemove;
            for (MAuint32 itau = 0; itau < myEvent.rec()->taus().size(); itau++)
            {
                if (RANDOM->flat() > tau_tagging_eff(*myEvent.rec()->taus()[itau].mc()))
            }
        }
    }

    // Convert Jet object to tau object
    void SFSTaggerBase::Jet2Tau(const RecJetFormat * myJet, RecTauFormat *myTau, EventFormat &myEvent) const
    {
        myTau->setMomentum(myJet->momentum());
        myTau->ntracks_   = myJet->ntracks();
        myTau->mc_        = myJet->mc_;
        myTau->DecayMode_ = PHYSICS->GetTauDecayMode(myTau->mc_);
        myTau->pseudojet_ = myJet->pseudojet_;

        MAint32 charge = 0;
        myTau->Constituents_ = myJet->Constituents_;
        for (auto &constit: myTau->Constituents_)
            charge += PDG->GetCharge(myEvent.mc()->particles()[constit].pdgid());

        myTau->charge_ = charge > 0 ? true : false;
    }

    // Convert Tau object to Jet
    void SFSTaggerBase::Tau2Jet(RecJetFormat *myJet, const RecTauFormat *myTau, EventFormat &myEvent) const
    {
        myJet->setMomentum(myTau->momentum());
        myJet->ntracks_      = myTau->ntracks();
        myJet->mc_           = myTau->mc_;
        myJet->pseudojet_    = myTau->pseudojet_;
        myJet->Constituents_ = myTau->Constituents_;
    }



}