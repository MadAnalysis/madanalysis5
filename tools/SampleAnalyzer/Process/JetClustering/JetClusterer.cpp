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


// SampleAnalyzer headers
#include "SampleAnalyzer/Process/JetClustering/JetClusterer.h"
#include "SampleAnalyzer/Commons/Service/LoopService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"
#include "SampleAnalyzer/Commons/Service/ConvertService.h"
#include "SampleAnalyzer/Commons/Service/PDGService.h"
#include "SampleAnalyzer/Process/JetClustering/NullSmearer.h"


using namespace MA5;

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
// Sorting the reco objects (necessary after smearing)
// -----------------------------------------------------------------------------
MAbool sort_by_leptonPT(RecLeptonFormat const & a, RecLeptonFormat const & b) { return a.pt() < b.pt(); };
MAbool sort_by_photonPT(RecPhotonFormat const & a, RecPhotonFormat const & b) { return a.pt() < b.pt(); };
MAbool    sort_by_tauPT(RecTauFormat    const & a, RecTauFormat    const & b) { return a.pt() < b.pt(); };

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

    // Set up tracks as charged FS particles OR charged interstate particles with nonzero ctau
    if (PDG->IsCharged(part.pdgid()) && part.mothers().size()>0)
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
        if (LOOP->ComingFromHadronDecay(&part,mySample)) continue;

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
      if (absid!=22 && absid!=11 && absid!=13) continue;

      // rejecting particle if coming from hadronization
      if (ExclusiveId_ && LOOP->ComingFromHadronDecay(&part,mySample,myEvent.mc()->particles().size())) continue;

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
        if (LOOP->IrrelevantPhoton(&part,mySample)) continue;
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

  // Sorting the objecfts after smearing
  if (mySmearer_->isElectronSmearerOn())
      std::sort(myEvent.rec()->electrons_.begin(), myEvent.rec()->electrons_.end(), sort_by_leptonPT);
  if (mySmearer_->isMuonSmearerOn())
      std::sort(myEvent.rec()->muons_.begin(),     myEvent.rec()->muons_.end(),     sort_by_leptonPT);
  if (mySmearer_->isTauSmearerOn())
      std::sort(myEvent.rec()->taus_.begin(),      myEvent.rec()->taus_.end(),      sort_by_tauPT);
  if (mySmearer_->isPhotonSmearerOn())
      std::sort(myEvent.rec()->photons_.begin(),   myEvent.rec()->photons_.end(),   sort_by_photonPT);

  // Launching the clustering
  // -> Filling the collection: myEvent->rec()->jets()
  algo_->Execute(mySample,myEvent,ExclusiveId_,vetos,vetos2,mySmearer_);

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


  return true;
}
