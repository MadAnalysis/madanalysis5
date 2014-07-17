////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
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

using namespace MA5;

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------
bool JetClusterer::Initialize(const std::map<std::string,std::string>& options)
{
  // algo defined ?
  if (algo_==0) return false;

  // configure tagger
  myBtagger_   = new bTagger();
  myCtagger_   = new cTagger();
  myTautagger_ = new TauTagger();

  // Loop over options
  for (std::map<std::string,std::string>::const_iterator
       it=options.begin();it!=options.end();it++)
  {
    std::string key = ClusterAlgoBase::Lower(it->first);
    bool result=false;

    // exclusive_id
    if (key=="exclusive_id")
    {
      int tmp=0;
      std::stringstream str;
      str << it->second;
      str >> tmp;
      if (tmp==1) ExclusiveId_=true;
      else if (tmp==0) ExclusiveId_=false;
      else
      {
        WARNING << "'exclusive_id' must be equal to 0 or 1. "
                << "Using default value 'exclusive_id' = " 
                << ExclusiveId_ << endmsg;
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
    if (!result) WARNING << "Parameter " << key << " unknown. It will be skipped." << endmsg;

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
}


// -----------------------------------------------------------------------------
// GetFinalState
// -----------------------------------------------------------------------------
void JetClusterer::GetFinalState(const MCParticleFormat* part, std::set<const MCParticleFormat*>& finalstates)
{
  for (unsigned int i=0; i<part->daughters().size(); i++)
  {
    if (PHYSICS->Id->IsFinalState(part->daughters()[i])) finalstates.insert(part->daughters()[i]);
    else return GetFinalState(part->daughters()[i],finalstates);
  }
}


// -----------------------------------------------------------------------------
// IsLast
// -----------------------------------------------------------------------------
Bool_t JetClusterer::IsLast(const MCParticleFormat* part, EventFormat& myEvent)
{
  for (unsigned int i=0; i<part->daughters().size(); i++)
  {
    if (part->daughters()[i]->pdgid()==part->pdgid()) return false;
  }
  return true;
}


// -----------------------------------------------------------------------------
// Execute
// -----------------------------------------------------------------------------
bool JetClusterer::Execute(SampleFormat& mySample, EventFormat& myEvent)
{
  // Safety
  if (mySample.mc()==0 || myEvent.mc()==0) return false;
  if (mySample.rec()==0) mySample.InitializeRec();
  if (myEvent.rec() ==0) myEvent.InitializeRec();

  // Reseting the reconstructed event
  myEvent.rec()->Reset();

  // Veto
  std::vector<bool> vetos(myEvent.mc()->particles().size(),false);
  std::set<const MCParticleFormat*> vetos2;

  // Filling the dataformat with electron/muon
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    const MCParticleFormat& part = myEvent.mc()->particles()[i];
    UInt_t absid = std::abs(part.pdgid());

    // Rejecting particle with a null pt (initial state ?)
    if (part.pt()<1e-10) continue;

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
        bool found=false;
        for (unsigned int j=0;j<myEvent.rec()->MCBquarks_.size();j++)
        {
          if (myEvent.rec()->MCBquarks_[j]==&(part)) 
          {found=true; break;}
        }
        if (!found) myEvent.rec()->MCBquarks_.push_back(&(part));
      }

      // looking for c quarks
      else if (absid==4)
      {
        bool found=false;
        for (unsigned int j=0;j<myEvent.rec()->MCCquarks_.size();j++)
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
        bool leptonic   = true;
        bool muonic     = false;
        bool electronic = false;
        for (unsigned int j=0;j<part.daughters().size();j++)
        {
          UInt_t pdgid = std::abs(part.daughters()[j]->pdgid());
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
          bool found=false;
          for (unsigned int j=0;j<myEvent.rec()->MCMuonicTaus_.size();j++)
          {
            if (myEvent.rec()->MCMuonicTaus_[j]==&(part)) 
            {found=true; break;}
          }
          if (!found) myEvent.rec()->MCMuonicTaus_.push_back(&(part));
        }

        // Saving taus decaying into electrons (only one copy)
        else if (electronic)
        {
          bool found=false;
          for (unsigned int j=0;j<myEvent.rec()->MCElectronicTaus_.size();j++)
          {
            if (myEvent.rec()->MCElectronicTaus_[j]==&(part)) 
            {found=true; break;}
          }
          if (!found) myEvent.rec()->MCElectronicTaus_.push_back(&(part));
        }

        // Saving taus decaying into hadrons (only copy)
        else
        {
          bool found=false;
          for (unsigned int j=0;j<myEvent.rec()->MCHadronicTaus_.size();j++)
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

            // Creating reco hadronic taus
            RecTauFormat* myTau = myEvent.rec()->GetNewTau();
            if (part.pdgid()>0) myTau->setCharge(-1);
            else myTau->setCharge(+1);
            myTau->setMomentum(part.momentum());
            myTau->setMc(&part);
            myTau->setDecayMode(PHYSICS->GetTauDecayMode(myTau->mc()));
            if (myTau->DecayMode()<=0) myTau->setNtracks(0); // ERROR case
            else if (myTau->DecayMode()==7 || 
                     myTau->DecayMode()==9) myTau->setNtracks(3); // 3-Prong
            else myTau->setNtracks(1); // 1-Prong

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
      if (ExclusiveId_ && LOOP->ComingFromHadronDecay(&part,mySample)) continue;

      // Muons
      if (absid==13)
      {
        vetos[i]=true;
        RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
        muon->setMomentum(part.momentum());
        muon->setMc(&(part));
        if (part.pdgid()==13) muon->SetCharge(-1);
        else muon->SetCharge(+1);
      }

      // Electrons
      else if (absid==11)
      {
        vetos[i]=true;
        RecLeptonFormat * elec = myEvent.rec()->GetNewElectron();
        elec->setMomentum(part.momentum());
        elec->setMc(&(part));
        if (part.pdgid()==11) elec->SetCharge(-1);
        else elec->SetCharge(+1);
      }

      // Photons
      else if (absid==22)
      {
        if (LOOP->IrrelevantPhoton(&part,mySample)) continue;
        vetos[i]=true;
        RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();
        photon->setMomentum(part.momentum());
        photon->setMc(&(part));
      }
    }
  }

  // Launching the clustering
  // -> Filling the collection: myEvent->rec()->jets()
  algo_->Execute(mySample,myEvent,ExclusiveId_,vetos,vetos2);

  // shortcut for TET & THT
  double & TET = myEvent.rec()->TET();
  //  double & THT = myEvent.rec()->THT();
  RecParticleFormat* MET = &(myEvent.rec()->MET());
  RecParticleFormat* MHT = &(myEvent.rec()->MHT());

  // End
  if (ExclusiveId_)
  {
    for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
    {
      (*MET) -= myEvent.rec()->electrons()[i].momentum();
      TET += myEvent.rec()->electrons()[i].pt();
    }
    for (unsigned int i=0;i<myEvent.rec()->photons().size();i++)
    {
      (*MET) -= myEvent.rec()->photons()[i].momentum();
      TET += myEvent.rec()->photons()[i].pt();
    }
    for (unsigned int i=0;i<myEvent.rec()->taus().size();i++)
    {
      (*MET) -= myEvent.rec()->taus()[i].momentum();
      TET += myEvent.rec()->taus()[i].pt();
    }
  }

  for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
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
