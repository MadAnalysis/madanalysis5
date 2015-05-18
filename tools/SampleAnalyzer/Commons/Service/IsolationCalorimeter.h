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

#ifndef ISOLATIONCALORIMETER_SERVICE_h
#define ISOLATIONCALORIMETER_SERVICE_h

// STL headers

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/IsolationBase.h"


namespace MA5
{

class IsolationCalorimeter : IsolationBase
{
  // -------------------------------------------------------------
  //                       data members
  // -------------------------------------------------------------
  private:


  public:

    /// Constructor
    IsolationCalorimeter() {}

    /// Destructor
    virtual ~IsolationCalorimeter() {}


    virtual Double_t relIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return relIsolation(&part, event, DR, PTmin); }

    virtual Double_t relIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      if (part->pt()<1e-9) return 999.;
      return sumIsolation(part,event,DR,PTmin)/part->pt();
    }

    virtual Double_t sumIsolation(const RecLeptonFormat& part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    { return sumIsolation(&part, event, DR, PTmin); }

    virtual Double_t sumIsolation(const RecLeptonFormat* part, const RecEventFormat* event, const double& DR, double PTmin=0.5) const
    {
      if (part==0) return 0;
      if (event==0) return 0;
      Double_t sum=0.;
      sum += sumPT(part,event->towers(),DR,PTmin);
      return sum;
    }


    virtual std::vector<const RecLeptonFormat*> getRelIsolatedMuons(const RecEventFormat* event, 
                                                                       const double& threshold, const double& DR, double PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->muons().size();i++)
      {
        if (relIsolation(event->muons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->muons()[i]));
      }
      return isolated;
    }

    ///
    virtual std::vector<const RecLeptonFormat*> getRelIsolatedElectrons(const RecEventFormat* event, 
                                                                           const double& threshold, const double& DR, double PTmin=0.5) const
    {
      std::vector<const RecLeptonFormat*> isolated;
      for (unsigned int i=0;i<event->electrons().size();i++)
      {
        if (relIsolation(event->electrons()[i],event,DR,PTmin)>threshold) continue;
        isolated.push_back(&(event->electrons()[i]));
      }
      return isolated;
    }


};

}

#endif
