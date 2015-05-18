#include "SampleAnalyzer/Commons/Service/IsolationBase.h"
using namespace MA5;


/// -----------------------------------------------
/// sumPT
/// -----------------------------------------------
Double_t IsolationBase::sumPT(const RecLeptonFormat* part, 
                         const std::vector<RecTrackFormat>& tracks,
                         const double& DR, 
                         double PTmin) const
{
  double sumPT=0.;
  unsigned int counter=0;

  // Loop over the towers
  for (unsigned int i=0;i<tracks.size();i++)
  {
    const RecTrackFormat& track = tracks[i];

    // Cut on the PT
    if (track.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(track.momentum()) > DR) continue;

    // Cut on id
    if (track.refmc()==part->refmc()) continue;

    // Sum
    sumPT += track.pt();
    counter++;
  }

  // return PT sum of tracks in the cone
  return sumPT;
}


/// -----------------------------------------------
/// sumPT
/// -----------------------------------------------
Double_t IsolationBase::sumPT(const RecLeptonFormat* part, 
                         const std::vector<RecTowerFormat>& towers,
                         const double& DR, 
                         double PTmin) const
{
  double sumPT=0.;
  unsigned int counter=0;

  // Loop over the tracks
  for (unsigned int i=0;i<towers.size();i++)
  {
    const RecTowerFormat& tower = towers[i];

    // Cut on the PT
    if (tower.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(tower.momentum()) > DR) continue;

    /*    // Cut on id
    bool id=false;
    for (unsigned int j=0;j<tower.refmcs().size();j++)
    {
      if (tower.refmcs()[j]==part->refmc()) {id=true;break;}
    }
    if (id) continue;
    */

    // Sum
    sumPT += tower.pt();
    counter++;
  }

  // return PT sum of towers in the cone
  return sumPT;
}


/// -----------------------------------------------
/// sumPT
/// -----------------------------------------------
Double_t IsolationBase::sumPT(const RecLeptonFormat* part, 
                         const std::vector<RecParticleFormat>& towers,
                         const double& DR, 
                         double PTmin) const
{
  double sumPT=0.;
  unsigned int counter=0;

  // Loop over the tracks
  for (unsigned int i=0;i<towers.size();i++)
  {
    const RecParticleFormat& tower = towers[i];

    // Cut on the PT
    if (tower.pt()<PTmin) continue;

    // Cut on the DR
    if (part->momentum().DeltaR(tower.momentum()) > DR) continue;

    // Cut on id
    //    if (tower.refmc()==part->refmc()) continue;

    // Sum
    sumPT += tower.pt();
    counter++;
  }

  // return PT sum of towers in the cone
  return sumPT;
}

