////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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
#include "SampleAnalyzer/Commons/Service/IsolationBase.h"


using namespace MA5;



/// -----------------------------------------------
/// sumPT Lepton vs Tracks
/// -----------------------------------------------
MAfloat64 IsolationBase::sumPT(const RecLeptonFormat* part, 
                              const std::vector<RecTrackFormat>& tracks,
                              const MAfloat64& DR, 
                              MAfloat64 PTmin) const
{
  MAfloat64 sumPT=0.;
  MAuint32 counter=0;

  // Loop over the towers
  for (MAuint32 i=0;i<tracks.size();i++)
  {
    const RecTrackFormat& track = tracks[i];

    // Cut on the PT
    if (track.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(track.momentum()) > DR) continue;

    // Cut on id
    //    if (track.isDelphesUnique(part->delphesTags())) continue;

    // Sum
    sumPT += track.pt();
    counter++;
  }

  // return PT sum of tracks in the cone
  return sumPT;
}


/// -----------------------------------------------
/// sumPT Lepton vs Towers
/// -----------------------------------------------
MAfloat64 IsolationBase::sumPT(const RecLeptonFormat* part, 
                              const std::vector<RecTowerFormat>& towers,
                              const MAfloat64& DR, 
                              MAfloat64 PTmin) const
{
  MAfloat64 sumPT=0.;
  MAuint32 counter=0;

  // Loop over the tracks
  for (MAuint32 i=0;i<towers.size();i++)
  {
    const RecTowerFormat& tower = towers[i];

    // Cut on the PT
    if (tower.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(tower.momentum()) > DR) continue;

    // Sum
    sumPT += tower.pt();
    counter++;
  }

  // return PT sum of towers in the cone
  return sumPT;
}


/// -----------------------------------------------
/// sumPT Lepton vs Eflow object
/// -----------------------------------------------
MAfloat64 IsolationBase::sumPT(const RecLeptonFormat* part, 
                              const std::vector<RecParticleFormat>& towers,
                              const MAfloat64& DR, 
                              MAfloat64 PTmin) const
{
  MAfloat64 sumPT=0.;
  MAuint32 counter=0;

  // Loop over the tracks
  for (MAuint32 i=0;i<towers.size();i++)
  {
    const RecParticleFormat& tower = towers[i];

    // Cut on the PT
    if (tower.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(tower.momentum()) > DR) continue;

    // Sum
    sumPT += tower.pt();
    counter++;
  }

  // return PT sum of towers in the cone
  return sumPT;
}


/// -----------------------------------------------
/// sumPT Photon vs Tracks
/// -----------------------------------------------
MAfloat64 IsolationBase::sumPT(const RecPhotonFormat* part, 
                              const std::vector<RecTrackFormat>& tracks,
                              const MAfloat64& DR, 
                              MAfloat64 PTmin) const
{
  MAfloat64 sumPT=0.;
  MAuint32 counter=0;

  // Loop over the towers
  for (MAuint32 i=0;i<tracks.size();i++)
  {
    const RecTrackFormat& track = tracks[i];

    // Cut on the PT
    if (track.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(track.momentum()) > DR) continue;

    // Sum
    sumPT += track.pt();
    counter++;
  }

  // return PT sum of tracks in the cone
  return sumPT;
}


/// -----------------------------------------------
/// sumPT Photon vs Towers
/// -----------------------------------------------
MAfloat64 IsolationBase::sumPT(const RecPhotonFormat* part, 
                         const std::vector<RecTowerFormat>& towers,
                         const MAfloat64& DR, 
                         MAfloat64 PTmin) const
{
  MAfloat64 sumPT=0.;
  MAuint32 counter=0;

  // Loop over the tracks
  for (MAuint32 i=0;i<towers.size();i++)
  {
    const RecTowerFormat& tower = towers[i];

    // Cut on the PT
    if (tower.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(tower.momentum()) > DR) continue;

    // Sum
    sumPT += tower.pt();
    counter++;
  }

  // return PT sum of towers in the cone
  return sumPT;
}


/// -----------------------------------------------
/// sumPT Photon vs Eflow object
/// -----------------------------------------------
MAfloat64 IsolationBase::sumPT(const RecPhotonFormat* part, 
                         const std::vector<RecParticleFormat>& towers,
                         const MAfloat64& DR, 
                         MAfloat64 PTmin) const
{
  MAfloat64 sumPT=0.;
  MAuint32 counter=0;

  // Loop over the tracks
  for (MAuint32 i=0;i<towers.size();i++)
  {
    const RecParticleFormat& tower = towers[i];

    // Cut on the PT
    if (tower.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(tower.momentum()) > DR) continue;

    // Cut on id
    //    if (tower.isDelphesUnique(part->delphesTags())) continue;

    // Sum
    sumPT += tower.pt();
    counter++;
  }

  // return PT sum of towers in the cone
  return sumPT;
}



