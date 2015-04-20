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

#ifndef ISOLATION_SERVICE_h
#define ISOLATION_SERVICE_h

// STL headers

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/RecEventFormat.h"


namespace MA5
{

class Isolation
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:

    Double_t sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecTrackFormat>& tracks,
                   const double& DR,double PTmin) const; 

    Double_t sumPT(const RecLeptonFormat* part, 
                   const std::vector<RecParticleFormat>& towers,
                   const double& DR,double PTmin) const;


  public:

    /// Constructor
    Isolation() {}

    /// Destructor
    ~Isolation() {}


    Double_t relEFlowIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return relEFlowIsolation(&part, event, DR, PTmin); }

    Double_t relEFlowIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()==0) return 999.;
      return sumEFlowIsolation(part,event,DR,PTmin)/part->pt();
    }

    Double_t sumEFlowIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return sumEFlowIsolation(&part, event, DR, PTmin); }

    Double_t sumEFlowIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      Double_t sum=0.;
      sum += sumPT(part,event->EFlowTracks(),DR,PTmin);
      sum += sumPT(part,event->EFlowPhotons(),DR,PTmin);
      sum += sumPT(part,event->EFlowNeutralHadrons(),DR,PTmin);
      return sum;
    }

    Double_t relTrackerIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return relTrackerIsolation(&part, event, DR, PTmin); }

    Double_t relTrackerIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()==0) return 999.;
      return sumTrackerIsolation(part,event,DR,PTmin)/part->pt();
    }

    Double_t sumTrackerIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return sumTrackerIsolation(&part, event, DR, PTmin); }

    Double_t sumTrackerIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      Double_t sum=0.;
      sum += sumPT(part,event->tracks(),DR,PTmin);
      return sum;
    }

    Double_t relCalorimeterIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return relCalorimeterIsolation(&part, event, DR, PTmin); }

    Double_t relCalorimeterIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()==0) return 999.;
      return sumCalorimeterIsolation(part,event,DR,PTmin)/part->pt();
    }

    Double_t sumCalorimeterIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return sumCalorimeterIsolation(&part, event, DR, PTmin); }

    Double_t sumCalorimeterIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      Double_t sum=0.;
      sum += sumPT(part,event->towers(),DR,PTmin);
      return sum;
    }

    /// 
    std::vector<const RecLeptonFormat*> getRelTrackerIsolatedMuons(const RecEventFormat* event, 
                                                                   const double& threshold, const double& DR, double PTmin=0.5) const
    { 
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->muons().size();i++)
      {
        if (relTrackerIsolation(event->muons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->muons()[i]));
      }
      return isolated;
    }

    ///
    std::vector<const RecLeptonFormat*> getRelCalorimeterIsolatedMuons(const RecEventFormat* event, 
                                                                       const double& threshold, const double& DR, double PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->muons().size();i++)
      {
        if (relCalorimeterIsolation(event->muons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->muons()[i]));
      }
      return isolated;
    }

    ///
    std::vector<const RecLeptonFormat*> getRelEFlowIsolatedMuons(const RecEventFormat* event, 
                                                                 const double& threshold, const double& DR, double PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->muons().size();i++)
      {
        if (relEFlowIsolation(event->muons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->muons()[i]));
      }
      return isolated;
    }

    /// 
    std::vector<const RecLeptonFormat*> getRelTrackerIsolatedElectrons(const RecEventFormat* event, 
                                                                       const double& threshold, const double& DR, double PTmin=0.5) const
    { 
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->electrons().size();i++)
      {
        if (relTrackerIsolation(event->electrons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->electrons()[i]));
      }
      return isolated;
    }

    ///
    std::vector<const RecLeptonFormat*> getRelCalorimeterIsolatedElectrons(const RecEventFormat* event, 
                                                                           const double& threshold, const double& DR, double PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->electrons().size();i++)
      {
        if (relCalorimeterIsolation(event->electrons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->electrons()[i]));
      }
      return isolated;
    }

    ///
    std::vector<const RecLeptonFormat*> getRelEFlowIsolatedElectrons(const RecEventFormat* event, 
                                                                     const double& threshold, const double& DR, double PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->electrons().size();i++)
      {
        if (relEFlowIsolation(event->electrons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->electrons()[i]));
      }
      return isolated;
    }



    std::vector<const RecJetFormat*> cleanJets(const RecEventFormat* event,
                                               const std::vector<const RecLeptonFormat*>& muons, 
                                               const std::vector<const RecLeptonFormat*>& electrons,
                                               const double& threshold) const
    {
      std::vector<const RecJetFormat*> cleaned;
      for (unsigned int i=0;i<event->jets().size();i++)
      {
        const RecJetFormat& jet = event->jets()[i];
        bool isolated = true;
  
        for (unsigned int j=0;j<muons.size();j++)
        {
          const RecLeptonFormat* muon = muons[j];
          if (muon->momentum().DeltaR(jet.momentum())<threshold) isolated=false;
        }
        if (!isolated) continue;

        for (unsigned int j=0;j<electrons.size();j++)
        {
          const RecLeptonFormat* elec = electrons[j];
          if (elec->momentum().DeltaR(jet.momentum())<threshold) isolated=false;
        }
        if (!isolated) continue;
        
        cleaned.push_back(&jet);
      }
      return cleaned;
    }

};

}

#endif
