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
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/JetClustering/JetClusterer.h"
#include "SampleAnalyzer/Commons/Service/LoopService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/ConvertService.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Process/JetClustering/NullSmearer.h"

#ifdef MA5_FASTJET_MODE
    #include "SampleAnalyzer/Interfaces/substructure/VariableR.h"
#endif

using namespace MA5;

/// Set isolation cones for tracks, e, mu, photon based on tower objects
template<class Type>
void SetConeRadius(
        std::vector<MAfloat64> cone_radius, std::vector<Type>& objects, MCParticleFormat part, MAbool addself=false
)
{
    for (MAuint32 iR=0; iR<cone_radius.size(); iR++)
    {
        for (MAuint32 i=0; i<objects.size(); i++)
        {
            IsolationConeType* current_isocone = objects[i].GetIsolCone(cone_radius[iR]);
            if (objects[i].dr(part.momentum()) < cone_radius[iR])
            {
                current_isocone->addsumPT(part.pt());
                current_isocone->addSumET(part.et());
                if (addself)
                {
                    current_isocone->setSelfPT(objects[i].pt());
                    current_isocone->setSelfET(objects[i].et());
                }
            }
        }
    }
}



// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
MAbool JetClusterer::Initialize(const std::map<std::string,std::string>& options)
{
    // algo defined ?
    if (algo_==0) return false;

    // configure tagger
    myBtagger_   = new bTagger();
    myCtagger_   = new cTagger();
    myTautagger_ = new TauTagger();
    mySmearer_   = new NullSmearer();
    mySmearer_->Initialize(true);

    // Loop over options
    for (std::map<std::string,std::string>::const_iterator
                 it=options.begin();it!=options.end();it++)
    {
        std::string key = ClusterAlgoBase::Lower(it->first);
        MAbool result=false;

        // exclusive_id
        if (key=="exclusive_id")
        {
            MAint32 tmp=0;
            std::stringstream str;
            str << it->second;
            str >> tmp;
            try
            {
                if (tmp==1) ExclusiveId_=true;
                else if (tmp==0) ExclusiveId_=false;
                else throw EXCEPTION_WARNING("'exclusive_id' must be equal to 0 or 1. Using default value 'exclusive_id' = "+CONVERT->ToString(ExclusiveId_),"",0);
            }
            catch(const std::exception& e)
            {
                MANAGE_EXCEPTION(e);
            }
            result=true;
        }

            // b-tagging
        else if (key.find("bjet_id.")==0)
        {
            result=myBtagger_->SetParameter(key.substr(8),it->second,"bjet_id.");
        }

            // c-tagging
            //    else if (key.find("cjet_id.")==0)
            //    {
            //      result=myCtagger_->SetParameter(key.substr(8),it->second,"cjet_id.");
            //    }

            // tau-tagging
        else if (key.find("tau_id.")==0)
        {
            result=myTautagger_->SetParameter(key.substr(7),it->second,"tau_id.");
        }

            // clustering algo
        else if (key.find("cluster.")==0)
        {
            result=algo_->SetParameter(key.substr(8),it->second);
        }

            // Primary Jet ID
        else if (key == "jetid")
        {
            JetID_ = it->second;
            result = true;
        }

            // Isolation cone radius for tracker
        else if (key.find("isolation")==0)
        {
            std::stringstream str(it->second);
            for (MAfloat64 tmp; str >> tmp;)
            {
                if (tmp>0. && key.substr(10) == "track.radius")    isocone_track_radius_.push_back(tmp);
                if (tmp>0. && key.substr(10) == "electron.radius") isocone_electron_radius_.push_back(tmp);
                if (tmp>0. && key.substr(10) == "muon.radius")     isocone_muon_radius_.push_back(tmp);
                if (tmp>0. && key.substr(10) == "photon.radius")   isocone_photon_radius_.push_back(tmp);
                if (str.peek() == ',' || str.peek() == ' ') str.ignore();
            }
            result = true;
        }

        // Other
        try
        {
            if (!result) throw EXCEPTION_WARNING("Parameter = "+key+" unknown. It will be skipped.","",0);
        }
        catch(const std::exception& e)
        {
            MANAGE_EXCEPTION(e);
        }

    }

    // configure algo
    algo_->Initialize();


    return true;
}


// -----------------------------------------------------------------------------
// Finalize
// -----------------------------------------------------------------------------
void JetClusterer::Finalize()
{
    if (algo_!=0)        delete algo_;
    if (myBtagger_!=0)   delete myBtagger_;
    if (myCtagger_!=0)   delete myCtagger_;
    if (myTautagger_!=0) delete myTautagger_;
    if (mySmearer_!=0)   delete mySmearer_;
}


// -----------------------------------------------------------------------------
// GetFinalState
// -----------------------------------------------------------------------------
void JetClusterer::GetFinalState(const MCParticleFormat* part, std::set<const MCParticleFormat*>& finalstates)
{
    for (MAuint32 i=0; i<part->daughters().size(); i++)
    {
        if (PHYSICS->Id->IsFinalState(part->daughters()[i])) finalstates.insert(part->daughters()[i]);
        else return GetFinalState(part->daughters()[i],finalstates);
    }
}


// -----------------------------------------------------------------------------
// IsLast
// -----------------------------------------------------------------------------
MAbool JetClusterer::IsLast(const MCParticleFormat* part, EventFormat& myEvent)
{
    for (MAuint32 i=0; i<part->daughters().size(); i++)
    {
        if (part->daughters()[i]->pdgid()==part->pdgid()) return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Execute
// -----------------------------------------------------------------------------
MAbool JetClusterer::Execute(SampleFormat& mySample, EventFormat& myEvent)
{
    // Safety
    if (mySample.mc()==0 || myEvent.mc()==0) return false;
    if (mySample.rec()==0) mySample.InitializeRec();
    if (myEvent.rec() ==0) myEvent.InitializeRec();

    // Reseting the reconstructed event
    myEvent.rec()->Reset();

    // Veto
    std::vector<MAbool> vetos(myEvent.mc()->particles().size(),false);
    std::set<const MCParticleFormat*> vetos2;

    // Filling the dataformat with electron/muon
    for (MAuint32 i=0;i<myEvent.mc()->particles().size();i++)
    {
        const MCParticleFormat& part = myEvent.mc()->particles()[i];
        MAuint32 absid = std::abs(part.pdgid());

        // Rejecting particle with a null pt (initial state ?)
        if (part.pt()<1e-10) continue;

        // Run particle propagator
        if (mySmearer_->isPropagatorOn() && part.mothers().size()>0)
            mySmearer_->ParticlePropagator(const_cast<MCParticleFormat*>(&part));

        // @JACK delphes based analyses already has tracks
        // Set up tracks as charged FS particles OR charged interstate particles with nonzero ctau
        if (PDG->IsCharged(part.pdgid()) && part.mothers().size()>0 && algo_!=0)
        {
            // Minimum tracking requirement is around 0.5 mm see ref. 1007.1988
            if (part.ctau() > 0. || PHYSICS->Id->IsFinalState(part))
            {
                // Reminder: -1 is reserved for the tracks
                MCParticleFormat smeared_track = mySmearer_->Execute(&part, -1);
                if (smeared_track.pt() > 1e-5)
                {
                    RecTrackFormat * track = myEvent.rec()->GetNewTrack();
                    MALorentzVector trk_mom;
                    trk_mom.SetPtEtaPhiM(smeared_track.pt(),
                                         smeared_track.eta(),
                                         smeared_track.phi(),0.0);
                    track->setMomentum(trk_mom);
                    track->setD0(smeared_track.d0());
                    track->setDZ(smeared_track.dz());
                    track->setD0Approx(smeared_track.d0_approx());
                    track->setDZApprox(smeared_track.dz_approx());
                    MAdouble64 ctau = PHYSICS->Id->IsFinalState(part) ? 0.0 : part.mothers()[0]->ctau();
                    MALorentzVector new_vertex(part.mothers()[0]->decay_vertex().X(),
                                               part.mothers()[0]->decay_vertex().Y(),
                                               part.mothers()[0]->decay_vertex().Z(), ctau);
                    track->setProductionVertex(new_vertex);
                    track->setClosestApproach(smeared_track.closest_approach());
                    track->setMc(&(part));
                    track->SetCharge(PDG->GetCharge(part.pdgid()) / 3.);
                }
            }
        }

        // Treating intermediate particles
        if (PHYSICS->Id->IsInterState(part))
        {
            // rejecting not interesting particles
            if (absid!=5 && absid!=4 && absid!=15) continue;

            // keeping the last particle with the same id in the decay chain
            if (!IsLast(&part, myEvent)) continue;

            // looking for b quarks
            if (absid==5)
            {
                MAbool found=false;
                for (MAuint32 j=0;j<myEvent.rec()->MCBquarks_.size();j++)
                {
                    if (myEvent.rec()->MCBquarks_[j]==&(part))
                    {found=true; break;}
                }
                if (!found) myEvent.rec()->MCBquarks_.push_back(&(part));
            }

                // looking for c quarks
            else if (absid==4)
            {
                MAbool found=false;
                for (MAuint32 j=0;j<myEvent.rec()->MCCquarks_.size();j++)
                {
                    if (myEvent.rec()->MCCquarks_[j]==&(part))
                    {found=true; break;}
                }
                if (!found) myEvent.rec()->MCCquarks_.push_back(&(part));
            }

                // looking for taus
            else if (absid==15)
            {
                // rejecting particle if coming from hadronization
                if (LOOP->ComingFromHadronDecay(&part,mySample,myEvent.mc()->particles().size())) continue;

                // Looking taus daughters id
                MAbool leptonic   = true;
                MAbool muonic     = false;
                MAbool electronic = false;
                for (MAuint32 j=0;j<part.daughters().size();j++)
                {
                    MAuint32 pdgid = std::abs(part.daughters()[j]->pdgid());
                    if      (pdgid==13) muonic=true;
                    else if (pdgid==11) electronic=true;
                    else if (pdgid!=22 /*photons*/ &&
                             !(pdgid>=11 && pdgid<=16) /*neutrinos*/)
                        leptonic=false;
                }
                if (!leptonic) {muonic=false; electronic=false;}

                // Saving taus decaying into muons (only one copy)
                if (muonic)
                {
                    MAbool found=false;
                    for (MAuint32 j=0;j<myEvent.rec()->MCMuonicTaus_.size();j++)
                    {
                        if (myEvent.rec()->MCMuonicTaus_[j]==&(part))
                        {found=true; break;}
                    }
                    if (!found) myEvent.rec()->MCMuonicTaus_.push_back(&(part));
                }

                    // Saving taus decaying into electrons (only one copy)
                else if (electronic)
                {
                    MAbool found=false;
                    for (MAuint32 j=0;j<myEvent.rec()->MCElectronicTaus_.size();j++)
                    {
                        if (myEvent.rec()->MCElectronicTaus_[j]==&(part))
                        {found=true; break;}
                    }
                    if (!found) myEvent.rec()->MCElectronicTaus_.push_back(&(part));
                }

                    // Saving taus decaying into hadrons (only copy)
                else
                {
                    MAbool found=false;
                    for (MAuint32 j=0;j<myEvent.rec()->MCHadronicTaus_.size();j++)
                    {
                        if (myEvent.rec()->MCHadronicTaus_[j]==&(part))
                        {found=true; break;}
                    }
                    if (!found)
                    {
                        // Saving the hadrons in MC container
                        myEvent.rec()->MCHadronicTaus_.push_back(&(part));

                        // Applying efficiency
                        if (!myTautagger_->IsIdentified()) continue;

                        // Smearing the hadronic taus
                        MCParticleFormat smeared = mySmearer_->Execute(&part, static_cast<MAint32>(absid));
                        // If smeared pt is zero, no need to count the particle but it still needs
                        // to be vetoed for jet clustering.
                        if (smeared.pt() > 1e-10)
                        {
                            // Creating reco hadronic taus
                            RecTauFormat* myTau = myEvent.rec()->GetNewTau();
                            if (part.pdgid()>0) myTau->setCharge(-1);
                            else myTau->setCharge(+1);
                            myTau->setMomentum(smeared.momentum());
                            myTau->setD0(smeared.d0());
                            myTau->setDZ(smeared.dz());
                            myTau->setD0Approx(smeared.d0_approx());
                            myTau->setDZApprox(smeared.dz_approx());
                            myTau->setProductionVertex(MALorentzVector(part.mothers()[0]->decay_vertex().X(),
                                                                       part.mothers()[0]->decay_vertex().Y(),
                                                                       part.mothers()[0]->decay_vertex().Z(),0.0));
                            myTau->setClosestApproach(smeared.closest_approach());
                            myTau->setMc(&part);
                            myTau->setDecayMode(PHYSICS->GetTauDecayMode(myTau->mc()));
                            if (myTau->DecayMode()<=0) myTau->setNtracks(0); // ERROR case
                            else if (myTau->DecayMode()==7 ||
                                     myTau->DecayMode()==9) myTau->setNtracks(3); // 3-Prong
                            else myTau->setNtracks(1); // 1-Prong
                        }

                        // Searching final state
                        GetFinalState(&part,vetos2);
                    }
                }
            }
        }

            // Keeping only final states
        else if (PHYSICS->Id->IsFinalState(part))
        {
            // keeping only electron, muon and photon
            if (absid==22 || absid==11 || absid==13)
            {
                // rejecting particle if coming from hadronization
                if ( !(ExclusiveId_ && LOOP->ComingFromHadronDecay(&part,mySample)))
                {

                    // Muons
                    if (absid==13)
                    {
                        vetos[i]=true;

                        // Smearing its momentum
                        MCParticleFormat smeared = mySmearer_->Execute(&part, static_cast<MAint32>(absid));
                        if (smeared.pt() <= 1e-10) continue;

                        RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
                        muon->setMomentum(smeared.momentum());
                        muon->setD0(smeared.d0());
                        muon->setDZ(smeared.dz());
                        muon->setD0Approx(smeared.d0_approx());
                        muon->setDZApprox(smeared.dz_approx());
                        muon->setProductionVertex(MALorentzVector(part.mothers()[0]->decay_vertex().X(),
                                                                  part.mothers()[0]->decay_vertex().Y(),
                                                                  part.mothers()[0]->decay_vertex().Z(),0.0));
                        muon->setClosestApproach(smeared.closest_approach());
                        muon->setMc(&(part));
                        if (part.pdgid()==13) muon->SetCharge(-1);
                        else muon->SetCharge(+1);
                    }

                        // Electrons
                    else if (absid==11)
                    {
                        vetos[i]=true;

                        // Smearing the electron momentum
                        MCParticleFormat smeared = mySmearer_->Execute(&part, static_cast<MAint32>(absid));
                        if (smeared.pt() <= 1e-10) continue;

                        RecLeptonFormat * elec = myEvent.rec()->GetNewElectron();
                        elec->setMomentum(smeared.momentum());
                        elec->setD0(smeared.d0());
                        elec->setDZ(smeared.dz());
                        elec->setD0Approx(smeared.d0_approx());
                        elec->setDZApprox(smeared.dz_approx());
                        elec->setProductionVertex(MALorentzVector(part.mothers()[0]->decay_vertex().X(),
                                                                  part.mothers()[0]->decay_vertex().Y(),
                                                                  part.mothers()[0]->decay_vertex().Z(),0.0));
                        elec->setClosestApproach(smeared.closest_approach());
                        elec->setMc(&(part));
                        if (part.pdgid()==11) elec->SetCharge(-1);
                        else elec->SetCharge(+1);
                    }

                        // Photons
                    else if (absid==22)
                    {
                        if (!LOOP->IrrelevantPhoton(&part,mySample))
                        {
                            vetos[i]=true;

                            // Smearing the photon momentum
                            MCParticleFormat smeared = mySmearer_->Execute(&part, static_cast<MAint32>(absid));
                            if (smeared.pt() <= 1e-10) continue;

                            RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();
                            photon->setMomentum(smeared.momentum());
                            photon->setD0(smeared.d0());
                            photon->setDZ(smeared.dz());
                            photon->setD0Approx(smeared.d0_approx());
                            photon->setDZApprox(smeared.dz_approx());
                            photon->setProductionVertex(MALorentzVector(part.mothers()[0]->decay_vertex().X(),
                                                                        part.mothers()[0]->decay_vertex().Y(),
                                                                        part.mothers()[0]->decay_vertex().Z(),0.0));
                            photon->setClosestApproach(smeared.closest_approach());
                            photon->setMc(&(part));
                        }
                    }
                }
            }
            // Putting the good inputs into the containter
            // Good inputs = - final state
            //               - visible
            //               - if exclusiveID=1: particles not vetoed
            //               - if exclusiveID=0: all particles except muons
            if (PHYSICS->Id->IsInvisible(part) || algo_==0) continue;

            // ExclusiveId mode
            if (ExclusiveId_)
            {
                if (vetos[i]) continue;
                if (vetos2.find(&part)!=vetos2.end()) continue;
            }

                // NonExclusive Id mode
            else if (std::abs(part.pdgid())==13) continue;

            // Smearer module returns a smeared MCParticleFormat object
            // Default: NullSmearer, that does nothing
            // Reminder: 0 is reserved for the jet constituents
            MCParticleFormat smeared = mySmearer_->Execute(&part, 0);
            if (smeared.pt() <= 1e-10) continue;

            // Filling good particle for clustering
            myEvent.rec()->AddHadron(smeared, i);
        }
    }

    // Sorting the objecfts after smearing
    if (mySmearer_->isElectronSmearerOn())
        std::sort(myEvent.rec()->electrons_.begin(), myEvent.rec()->electrons_.end(),
                  [](RecLeptonFormat const & lep1, RecLeptonFormat const & lep2){ return lep1.pt() > lep2.pt(); });
    if (mySmearer_->isMuonSmearerOn())
        std::sort(myEvent.rec()->muons_.begin(), myEvent.rec()->muons_.end(),
                  [](RecLeptonFormat const & lep1, RecLeptonFormat const & lep2){ return lep1.pt() > lep2.pt(); });
    if (mySmearer_->isTauSmearerOn())
        std::sort(myEvent.rec()->taus_.begin(),      myEvent.rec()->taus_.end(),
                  [](RecTauFormat const & ta1, RecTauFormat const & ta2){ return  ta1.pt() > ta2.pt(); });
    if (mySmearer_->isPhotonSmearerOn())
        std::sort(myEvent.rec()->photons_.begin(), myEvent.rec()->photons_.end(),
                  [](RecPhotonFormat const & ph1, RecPhotonFormat const & ph2){ return  ph1.pt() > ph2.pt(); });

    // Set Primary Jet ID
    myEvent.rec()->SetPrimaryJetID(JetID_);
    // Launching the clustering
    // -> Filling the collection: myEvent->rec()->jets()
    algo_->Execute(mySample,myEvent,mySmearer_);

#ifdef MA5_FASTJET_MODE
    // Cluster additional jets separately. In order to save time Execute function
  // saves hadron inputs into memory and that configuration is used for the rest
  // of the jets.
  for (auto &collection_item: cluster_collection_)
      collection_item.second->Cluster(myEvent, collection_item.first);
  for (auto &substructure: substructure_collection_)
      substructure.second->Execute(myEvent, substructure.first);
#endif


    // shortcut for TET & THT
    MAfloat64 & TET = myEvent.rec()->TET();
    //  MAfloat64 & THT = myEvent.rec()->THT();
    RecParticleFormat* MET = &(myEvent.rec()->MET());
    RecParticleFormat* MHT = &(myEvent.rec()->MHT());

    // End
    if (ExclusiveId_)
    {
        for (MAuint32 i=0;i<myEvent.rec()->electrons().size();i++)
        {
            (*MET) -= myEvent.rec()->electrons()[i].momentum();
            TET += myEvent.rec()->electrons()[i].pt();
        }
        for (MAuint32 i=0;i<myEvent.rec()->photons().size();i++)
        {
            (*MET) -= myEvent.rec()->photons()[i].momentum();
            TET += myEvent.rec()->photons()[i].pt();
        }
        for (MAuint32 i=0;i<myEvent.rec()->taus().size();i++)
        {
            (*MET) -= myEvent.rec()->taus()[i].momentum();
            TET += myEvent.rec()->taus()[i].pt();
        }
    }

    for (MAuint32 i=0;i<myEvent.rec()->muons().size();i++)
    {
        (*MET) -= myEvent.rec()->muons()[i].momentum();
        TET += myEvent.rec()->muons()[i].pt();
    }

    MET->momentum().SetPz(0.);
    MET->momentum().SetE(MET->momentum().Pt());
    MHT->momentum().SetPz(0.);
    MHT->momentum().SetE(MHT->momentum().Pt());

    myBtagger_->Execute(mySample,myEvent);
    myTautagger_->Execute(mySample,myEvent);

#ifdef MA5_FASTJET_MODE
    // Setup isolation cones
  if (isocone_track_radius_.size() > 0 || isocone_electron_radius_.size() > 0 || \
      isocone_muon_radius_.size() > 0  || isocone_photon_radius_.size() > 0)
  {
    for (auto &part: myEvent.rec()->cluster_inputs())
    {
        MCParticleFormat current_jet;
        current_jet.momentum().SetPxPyPzE(part.px(),part.py(),part.pz(),part.e());
        // Set track isolation
        // Isolation cone is applied to each particle that deposits energy in HCAL;
        // all hadronic activity assumed to reach to HCAL
        SetConeRadius(isocone_track_radius_,    myEvent.rec()->tracks(),    current_jet, false);
        // Set Electron isolation
        SetConeRadius(isocone_electron_radius_, myEvent.rec()->electrons(), current_jet, !ExclusiveId_);
        // Set Muon isolation
        SetConeRadius(isocone_muon_radius_,     myEvent.rec()->muons(),     current_jet, false);
        // Set Photon isolation
        SetConeRadius(isocone_photon_radius_,   myEvent.rec()->photons(),   current_jet, !ExclusiveId_);
    }

  }
#endif

    return true;
}

// Load additional Jets
MAbool JetClusterer::LoadJetConfiguration(std::map<std::string,std::string> options)
{
#ifdef MA5_FASTJET_MODE
    std::string new_jetid;
        std::string algorithm;
        if (options.find("algorithm") == options.end())
        {
            ERROR << "Jet configuration needs to have `algorithm` option. Jet configuration will be ignored." << endmsg;
            return true;
        }
        else algorithm = options["algorithm"];
        if (options.find("JetID") == options.end())
        {
            ERROR << "Jet configuration needs to have `JetID` option. Jet configuration will be ignored." << endmsg;
            return true;
        }
        if (substructure_collection_.find(options["JetID"]) != substructure_collection_.end() || \
                cluster_collection_.find(options["JetID"]) != cluster_collection_.end() )
        {
            ERROR << "Jet ID " + options["JetID"] + \
                " already exists. Jet configuration will be ignored." << endmsg;
            return true;
        }

        if (algorithm != "VariableR")
        {
            std::map<std::string, std::string> clustering_params;

            // decide if its good to keep this jet
            ClusterAlgoBase* new_algo;
            // Loop over options
            for (const auto &it: options)
            {
                std::string key = ClusterAlgoBase::Lower(it.first);
                if (key=="jetid")
                {
                    // Check if JetID is used before
                    new_jetid = it.second;
                    continue;
                }

                // Find the clustering algorithm
                if (key=="algorithm")
                {
                    if (it.second == "antikt")           new_algo = new ClusterAlgoStandard("antikt");
                    else if (it.second == "cambridge")   new_algo = new ClusterAlgoStandard("cambridge");
                    else if (it.second == "genkt")       new_algo = new ClusterAlgoStandard("genkt");
                    else if (it.second == "kt")          new_algo = new ClusterAlgoStandard("kt");
                    else if (it.second == "siscone")     new_algo = new ClusterAlgoSISCone();
                    else if (it.second == "cdfmidpoint") new_algo = new ClusterAlgoCDFMidpoint();
                    else if (it.second == "cdfjetclu")   new_algo = new ClusterAlgoCDFJetClu();
                    else if (it.second == "gridjet")     new_algo = new ClusterAlgoGridJet();
                    else {
                        ERROR << "Unknown algorithm : " << it.second << ". It will be ignored." << endmsg;
                        return true;
                    }
                    continue;
                }
                // clustering algo -> keep the previous syntax
                else if (key.find("cluster.")==0)
                {
                    clustering_params.insert(std::pair<std::string,std::string>(key.substr(8),it.second));
                    continue;
                }

                // Other
                try
                {
                  throw EXCEPTION_WARNING("Parameter = "+key+" unknown. It will be skipped.","",0);
                }
                catch(const std::exception& e)
                {
                  MANAGE_EXCEPTION(e);
                  return false;
                }
            }

            cluster_collection_.insert(std::pair<std::string,ClusterAlgoBase*>(new_jetid,new_algo));
            for (const auto &it: clustering_params)
                cluster_collection_[new_jetid]->SetParameter(it.first, it.second);
            std::string algoname = cluster_collection_[new_jetid]->GetName();
            std::string params   = cluster_collection_[new_jetid]->GetParameters();
            INFO << "      - Adding Jet ID : " << new_jetid << endmsg;
            INFO << "            with algo : " << algoname << ", " << params << endmsg;
            cluster_collection_[new_jetid]->Initialize();
        }
        else if (algorithm == "VariableR")
        {
            for (std::string key: {"rho", "minR", "maxR", "PTmin", "clustertype", "strategy", "exclusive"})
            {
                if (options.find("cluster."+key) == options.end())
                {
                    ERROR << "Option 'cluster." + key + "' is missing. VariableR clustering will be ignored." << endmsg;
                    return true;
                }
            }
            MAfloat32 rho   = std::stof(options["cluster.rho"]);
            MAfloat32 minR  = std::stof(options["cluster.minR"]);
            MAfloat32 maxR  = std::stof(options["cluster.maxR"]);
            MAfloat32 ptmin = std::stof(options["cluster.PTmin"]);
            MAbool isExclusive = (options["cluster.exclusive"] == "1");

            Substructure::VariableR::ClusterType ctype = Substructure::VariableR::AKTLIKE;
            if (options["cluster.clustertype"] == "CALIKE")       ctype = Substructure::VariableR::CALIKE;
            else if (options["cluster.clustertype"] == "KTLIKE")  ctype = Substructure::VariableR::KTLIKE;
            else if (options["cluster.clustertype"] == "AKTLIKE") ctype = Substructure::VariableR::AKTLIKE;

            Substructure::VariableR::Strategy strategy = Substructure::VariableR::Best;
            if (options["cluster.strategy"] == "Best")         strategy = Substructure::VariableR::Best;
            else if (options["cluster.strategy"] == "N2Tiled") strategy = Substructure::VariableR::N2Tiled;
            else if (options["cluster.strategy"] == "N2Plain") strategy = Substructure::VariableR::N2Plain;
            else if (options["cluster.strategy"] == "NNH")     strategy = Substructure::VariableR::NNH;
            else if (options["cluster.strategy"] == "Native")  strategy = Substructure::VariableR::Native;

            Substructure::VariableR* variableR;
            variableR = new Substructure::VariableR(rho, minR, maxR, ctype, strategy, ptmin, isExclusive);

            substructure_collection_.insert(
                std::pair<std::string, Substructure::VariableR*>(options["JetID"], variableR)
            );

            std::string exclusive = isExclusive ? "True" : "False";
            INFO << "      - Adding Jet ID : " << options["JetID"] << endmsg;
            INFO << "            with algo : VariableR" << ", "
                 << "rho = " << options["cluster.rho"] << ", "
                 << "minR = " << options["cluster.minR"] << ", "
                 << "maxR = " << options["cluster.maxR"] << ", "
                 << "ptmin = " << options["cluster.PTmin"] << ", \n"
                 << "                                   "
                 << "isExclusive = " << exclusive << ", "
                 << "clustertype = " << options["cluster.clustertype"] << ", "
                 << "strategy = " << options["cluster.strategy"]
                 << endmsg;
        }
        else
        {
            ERROR << "Unknown algorithm: " << algorithm << endmsg;
            return false;
        }

        return true;
#else
    ERROR << "FastJet has not been enabled. Can not add jets to the analysis." << endmsg;
    return true;
#endif
}