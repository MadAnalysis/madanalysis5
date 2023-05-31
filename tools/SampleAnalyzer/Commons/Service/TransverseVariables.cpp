////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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


// STL headers
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/TransverseVariables.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/SortingService.h"
#include "SampleAnalyzer/Commons/Vector/MARotation3axis.h"


using namespace MA5;

/// -----------------------------------------------
/// Funcions related to the computation of the mt2
/// -----------------------------------------------

inline MAint32 signchange_n(MAfloat128 t1,MAfloat128 t2,MAfloat128 t3,MAfloat128 t4,MAfloat128 t5)
{
  MAint32 nsc=0;
  if(t1*t2>0) nsc++;
  if(t2*t3>0) nsc++;
  if(t3*t4>0) nsc++;
  if(t4*t5>0) nsc++;
  return nsc;
}

inline MAint32 signchange_p(MAfloat128 t1,MAfloat128 t2,MAfloat128 t3,MAfloat128 t4,MAfloat128 t5)
{
   MAint32 nsc=0;
   if(t1*t2<0) nsc++;
   if(t2*t3<0) nsc++;
   if(t3*t4<0) nsc++;
   if(t4*t5<0) nsc++;
   return nsc;
}

MAint32 TransverseVariables::Nsolutions(const MAfloat64 &E)
{
  //obtain the coefficients for the 4th order equation
  //divided by Ea^n to make the variable dimensionless
  MAfloat128 A4 = -4.*C2_[0]*C1_[1]*C2_[1]*C1_[2] + 4.*C1_[0]*C2_[1]*C2_[1]*C1_[2] +
    C2_[0]*C2_[0]*C1_[2]*C1_[2] + 4.*C2_[0]*C1_[1]*C1_[1]*C2_[2] -
    4.*C1_[0]*C1_[1]*C2_[1]*C2_[2] - 2.*C1_[0]*C2_[0]*C1_[2]*C2_[2] + C1_[0]*C1_[0]*C2_[2]*C2_[2];
  MAfloat128 A3 = (-4.*C2_[0]*C2_[1]*C1_[2]*C1_[3] + 8.*C2_[0]*C1_[1]*C2_[2]*C1_[3] -
    4.*C1_[0]*C2_[1]*C2_[2]*C1_[3] - 4.*C2_[0]*C1_[1]*C1_[2]*C2_[3] +
    8.*C1_[0]*C2_[1]*C1_[2]*C2_[3] - 4.*C1_[0]*C1_[1]*C2_[2]*C2_[3] -
    8.*C2_[0]*C1_[1]*C2_[1]*C1_[4] + 8.*C1_[0]*C2_[1]*C2_[1]*C1_[4] +
    4.*C2_[0]*C2_[0]*C1_[2]*C1_[4] - 4.*C1_[0]*C2_[0]*C2_[2]*C1_[4] +
    8.*C2_[0]*C1_[1]*C1_[1]*C2_[4] - 8.*C1_[0]*C1_[1]*C2_[1]*C2_[4] -
    4.*C1_[0]*C2_[0]*C1_[2]*C2_[4] + 4.*C1_[0]*C1_[0]*C2_[2]*C2_[4])/E;
  MAfloat128 A2 = (4.*C2_[0]*C2_[2]*C1_[3]*C1_[3] - 4.*C2_[0]*C1_[2]*C1_[3]*C2_[3] -
    4.*C1_[0]*C2_[2]*C1_[3]*C2_[3] + 4.*C1_[0]*C1_[2]*C2_[3]*C2_[3] -
    8.*C2_[0]*C2_[1]*C1_[3]*C1_[4] - 8.*C2_[0]*C1_[1]*C2_[3]*C1_[4] +
    16.*C1_[0]*C2_[1]*C2_[3]*C1_[4] + 4.*C2_[0]*C2_[0]*C1_[4]*C1_[4] +
    16.*C2_[0]*C1_[1]*C1_[3]*C2_[4] - 8.*C1_[0]*C2_[1]*C1_[3]*C2_[4] -
    8.*C1_[0]*C1_[1]*C2_[3]*C2_[4] - 8.*C1_[0]*C2_[0]*C1_[4]*C2_[4] +
    4.*C1_[0]*C1_[0]*C2_[4]*C2_[4] - 4.*C2_[0]*C1_[1]*C2_[1]*C1_[5] +
    4.*C1_[0]*C2_[1]*C2_[1]*C1_[5] + 2.*C2_[0]*C2_[0]*C1_[2]*C1_[5] -
    2.*C1_[0]*C2_[0]*C2_[2]*C1_[5] + 4.*C2_[0]*C1_[1]*C1_[1]*C2_[5] -
    4.*C1_[0]*C1_[1]*C2_[1]*C2_[5] - 2.*C1_[0]*C2_[0]*C1_[2]*C2_[5] +
    2.*C1_[0]*C1_[0]*C2_[2]*C2_[5])/pow(E,2.);
  MAfloat128 A1 = (-8.*C2_[0]*C1_[3]*C2_[3]*C1_[4] + 8.*C1_[0]*C2_[3]*C2_[3]*C1_[4] +
    8.*C2_[0]*C1_[3]*C1_[3]*C2_[4] - 8.*C1_[0]*C1_[3]*C2_[3]*C2_[4] -
    4.*C2_[0]*C2_[1]*C1_[3]*C1_[5] - 4.*C2_[0]*C1_[1]*C2_[3]*C1_[5] +
    8.*C1_[0]*C2_[1]*C2_[3]*C1_[5] + 4.*C2_[0]*C2_[0]*C1_[4]*C1_[5] -
    4.*C1_[0]*C2_[0]*C2_[4]*C1_[5] + 8.*C2_[0]*C1_[1]*C1_[3]*C2_[5] -
    4.*C1_[0]*C2_[1]*C1_[3]*C2_[5] - 4.*C1_[0]*C1_[1]*C2_[3]*C2_[5] -
    4.*C1_[0]*C2_[0]*C1_[4]*C2_[5] + 4.*C1_[0]*C1_[0]*C2_[4]*C2_[5])/pow(E,3);
  MAfloat128 A0 = (-4.*C2_[0]*C1_[3]*C2_[3]*C1_[5] + 4.*C1_[0]*C2_[3]*C2_[3]*C1_[5] +
    C2_[0]*C2_[0]*C1_[5]*C1_[5] + 4.*C2_[0]*C1_[3]*C1_[3]*C2_[5] -
    4.*C1_[0]*C1_[3]*C2_[3]*C2_[5] - 2.*C1_[0]*C2_[0]*C1_[5]*C2_[5] +
    C1_[0]*C1_[0]*C2_[5]*C2_[5])/pow(E,4);

  MAfloat128 C2 = -(A2/2. - 3.*pow(A3,2)/(16.*A4));
  MAfloat128 C1 = -(3.*A1/4. -A2*A3/(8.*A4));
  MAfloat128 C0 = -A0 + A1*A3/(16.*A4);
  MAfloat128 D1 = -2.*A2 - (4.*A4*C1*C1/C2 - 4.*A4*C0 -3.*A3*C1)/C2;
  MAfloat128 D0 = -A1 - 4.*A4*C0*C1/pow(C2,2) + 3.*A3*C0/C2;
  MAfloat128 E0 = -C0 - C2*D0*D0/(D1*D1) + C1*D0/D1;

  // Find the coefficients for the leading term in the Sturm sequence
  // The number of solutions depends on diffence of number of sign changes
  // for x->Inf and x->-Inf
  MAint32 nsol = signchange_n(A4,A4,C2,D1,E0) - signchange_p(A4,A4,C2,D1,E0);
  if (nsol < 0) nsol = 0; //rounding effects
  return nsol;
}


template <typename T> MAint32 sgn(T val) { return (T(0) < val) - (val < T(0)); }

MAint32 TransverseVariables::Nsolutions_massless(const MAfloat64 &dsq)
{
  //obtain the coefficients for the 4th order equation
  //divided by Ea^n to make the variable dimensionless
  MAfloat128 a = sgn(p2_.Px())*p2_.Mt()/dsq;
  MAfloat128 b = sgn(p2_.Px())*(msq_*p2_.Mt()/dsq - dsq/(4.*p2_.Mt()));
  MAfloat128 A4 = a*a*C2_[0];
  MAfloat128 A3 = 2.*a*C2_[1]/p2_.Mt();
  MAfloat128 A2 = (2.*a*C2_[0]*b+C2_[2]+2.*a*C2_[3])/p2_.Mt2();
  MAfloat128 A1 = (2.*b*C2_[1]+2.*C2_[4])/pow(p2_.Mt(),3);
  MAfloat128 A0 = (C2_[0]*b*b+2.*b*C2_[3]+C2_[5])/pow(p2_.Mt2(),2);

  MAfloat128 C2 = -(A2/2.-3.*pow(A3,2)/(16.*A4));
  MAfloat128 C1 = -(3.*A1/4.-A2*A3/(8.*A4));
  MAfloat128 C0 = -A0+A1*A3/(16.*A4);
  MAfloat128 D1 = -2.*A2-(4.*A4*C1*C1/C2 -4.*A4*C0-3.*A3*C1)/C2;
  MAfloat128 D0 = -A1-4.*A4*C0*C1/(C2*C2)+3.*A3*C0/C2;
  MAfloat128 E0 = -C0 - C2*D0*D0/(D1*D1) + C1*D0/D1;

  // Find the coefficients for the leading term in the Sturm sequence
  // The number of solutions depends on diffence of number of sign changes
  // for x->Inf and x->-Inf
  MAint32 nsol = signchange_n(A4,A4,C2,D1,E0)-signchange_p(A4,A4,C2,D1,E0);
  if( nsol < 0 ) nsol=0;  // possible rounding effects
  return nsol;
}



MAbool TransverseVariables::FindHigh(MAfloat64 &dsqH)
{
   MAfloat64 x0 = (C1_[2]*C1_[3]-C1_[1]*C1_[4])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
   MAfloat64 y0 = (C1_[0]*C1_[4]-C1_[1]*C1_[3])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
   MAfloat64 dsqL = p2_.M()*(2.*m_+p2_.M());
   do
   {
      MAfloat64 dsqM = (dsqH + dsqL)/2.;
      UpdateC1((dsqM-p2_.M2())/(2.*p2_.Mt2()));
      UpdateC2(((dsqM-p1_.M2())/2.+p1met_)/p1_.Mt2());
      MAint32 nsolM = Nsolutions(p2_.Mt());
      if     (nsolM==2) { dsqH = dsqM; return true; }
      else if(nsolM==4) { dsqH = dsqM; continue; }
      else if(nsolM==0)
      {
        UpdateC1((dsqM-p2_.M2())/(2.*p2_.Mt2()));
        UpdateC2(((dsqM-p1_.M2())/2.+p1met_)/p1_.Mt2());
        // Does the larger ellipse contain the smaller one? 
        MAfloat64 dis = C2_[0]*x0*x0+2.*C2_[1]*x0*y0+C2_[2]*y0*y0+2.*C2_[3]*x0+2.*C2_[4]*y0+C2_[5];
        if(dis<0) dsqH=dsqM;
        else      dsqL=dsqM;
      }
   } while ((dsqH-dsqL)>0.001);
   return false;
}

MAfloat64 TransverseVariables::GetMT2()
{
  // massless case
  if(p1_.M()<=0.1 && p2_.M()<=0.1)  { return GetMT2_massless(); }

  // Solving the two quadratic equations: initialization of the coefficients
  MAfloat64 dsq0 = p2_.M()*(p2_.M() + 2.*m_);
  InitCoefs();
  UpdateC1( (dsq0-p2_.M2())/(2.*p2_.Mt2()) );
  UpdateC2( ((dsq0-p1_.M2())/2.+p1met_)/p1_.Mt2() );

  // Get the center of the ellipses amd check if the larger ellipse contains
  // the smaller one
  MAfloat64 x0 = (C1_[2]*C1_[3]-C1_[1]*C1_[4])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
  MAfloat64 y0 = (C1_[0]*C1_[4]-C1_[1]*C1_[3])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
  MAfloat64 dis= C2_[0]*x0*x0+2.*C2_[1]*x0*y0+C2_[2]*y0*y0+2.*C2_[3]*x0+2.*C2_[4]*y0+C2_[5];
  if(dis<=0.01) { return sqrt(msq_+dsq0); }

  // If not, check if the larger ellipse contains the center of the smaller one
  // and get two estimates for an upper bound on MT2 (dsqH)
  MAfloat64 p2x0 = pmx_-x0, p2y0 = pmy_-y0;
  MAfloat64 dsqH = 2.*(p1_.Mt()*sqrt(pow(p2x0,2)+pow(p2y0,2)+msq_)-p1_.Px()*p2x0-p1_.Py()*p2y0)
    +p1_.M2();
  MAfloat64 dsqH2 = 2.*(p1_.Mt()*sqrt(pmtm_)-p1met_)+p1_.M2();
  MAfloat64 dsqH3 = 2.*p2_.Mt()*m_ + p2_.M2();
  if(dsqH3 > dsqH2) dsqH2 = dsqH3;
  if(dsqH  > dsqH2) dsqH  = dsqH2;

  // Calculating the number of solutions: coefficients for the two quadratic equations
  // bissection method
  MAint32 nsolL = Nsolutions(p2_.Mt());
  if(nsolL>0) { return sqrt(msq_+dsq0); }

  UpdateC1( (dsqH-p2_.M2())/(2.*p2_.Mt2()) );
  UpdateC2( ((dsqH-p1_.M2())/2.+p1met_)/p1_.Mt2() );
  MAint32 nsolH = Nsolutions(p2_.Mt());
  if(nsolH==nsolL || nsolH==4) { if(!FindHigh(dsqH)) { return sqrt(dsq0+msq_); } }

  while(sqrt(dsqH+msq_) - sqrt(dsq0+msq_) > 0.001)
  {
    MAfloat64 dsqM = (dsqH+dsq0)/2.;
    UpdateC1( (dsqM-p2_.M2())/(2.*p2_.Mt2()) );
    UpdateC2( ((dsqM-p1_.M2())/2.+p1met_)/p1_.Mt2() );
    MAint32 nsolM = Nsolutions(p2_.Mt());
    if(nsolM==4) { dsqH=dsqM; FindHigh(dsqH); continue; }
    if(nsolM!=nsolL) dsqH=dsqM;
    if(nsolM==nsolL) dsq0=dsqM;
  }
  return sqrt(msq_+dsqH);
}

MAfloat64 TransverseVariables::GetMT2_massless()
{
  // Rotation of all four-momenta so that p2_.Py() = 0
  MAfloat64 th=-atan(p2_.Py()/p2_.Px());
  MARotation3axis rot(th,MARotation3axis::Zaxis);
  rot.rotate(p2_);
  rot.rotate(p1_);
  MAfloat64 pxtmp = pmx_*cos(th)-pmy_*sin(th);
  MAfloat64 pytmp = sin(th)*pmx_+cos(th)*pmy_;
  pmx_ = pxtmp;
  pmy_ = pytmp;

  // Initialization of the C2 coefficients + dsq0 + proceed with the calculation
  // of the number of solutions for the lower bourd
  MAfloat64 dsq0 = 0.0005/p2_.Mt2();
  InitC(p1_,C2_);
  UpdateC2( (dsq0+p1met_)/p1_.Mt2() );

  // Calculating the number of solutions: coefficients for the two quadratic equations
  // bissection method
  MAint32 nsolL = Nsolutions_massless(dsq0);
  if(nsolL>0) { return sqrt(msq_+dsq0); }

  // When both parabolas contain origin: two estimates for an upper bound on MT2 (dsqH)
  MAfloat64 dsqH  = 2.*(p1_.Mt()*sqrt(pmtm_) - p1met_);
  MAfloat64 dsqH2 = 2.*m_*p2_.Mt();
  if(dsqH  < dsqH2) dsqH = dsqH2;

  UpdateC2( (dsqH/2.+p1met_)/p1_.Mt2() );
  MAint32 nsolH = Nsolutions_massless(dsqH);

  // Scanning to get a new lower bound (bissection method)
  MAbool found=false;
  if (nsolH==nsolL)
  {
    for(MAfloat64 mass = m_+0.1; mass < sqrt(msq_+dsqH); mass+=0.1)
    {
      dsqH = pow(mass,2) - msq_;
      UpdateC2( (dsqH/2.+p1met_)/p1_.Mt2() );
      nsolH = Nsolutions_massless(dsqH);
      if(nsolH>0)  { found=true; dsq0=pow(mass-0.1,2)-msq_; break; }
    }
    if(!found) { return sqrt(dsq0+msq_); }
  }
  if(nsolH==nsolL) { return sqrt(dsq0+msq_); }

  // Now we apply the bissection method
  while(sqrt(dsqH+msq_) - sqrt(dsq0+msq_) > 0.001)
  {
    MAfloat64 dsqM = (dsqH+dsq0)/2.;
    UpdateC2( (dsqM/2.+p1met_)/p1_.Mt2() );
    MAint32 nsolM = Nsolutions_massless(dsqM);
    if(nsolM!=nsolL) dsqH=dsqM;
    if(nsolM==nsolL) dsq0=dsqM;
  }
  return sqrt(dsqH+msq_);
}

/// -----------------------------------------------
/// Funcions related to the computation of the mt2w
/// -----------------------------------------------
/// Core function for the computation of mt2w
MAfloat64 TransverseVariables::GetMT2W(const ParticleBaseFormat* lep,const ParticleBaseFormat* j1,
  const ParticleBaseFormat*j2,const ParticleBaseFormat&met)
{
  /// We define a mt2w region in which we will search for the bissection
  /// (default: from mw+mb to 500 GeV)
  InitializeMT2W(lep->momentum(), j1->momentum(), j2->momentum(), met.momentum());
  MAfloat64 mt_high = 500., upper=500.;
  MAfloat64 mt_low  = mw_ + std::max(p2_.M(), p3_.M());

  /// First, we need to check the 500 GeV hypothesis -> otherwise, we start at threshold
  if(!TestComp(mt_high)) mt_high = mt_low;

  // Scan to find the upper bound
  MAfloat64 step=0.5;
  while(!TestComp(mt_high) && mt_high < upper+2.*step)
  {
    mt_low = mt_high;
    mt_high += step;
  }

  // No compatible region found under the upper bound -> return upper bound - 1 GeV
  if (mt_high > upper) { return upper-2.*step; }

  // mt_high is compatible -> bissection method
  while(mt_high-mt_low>0.001)
  {
    MAfloat64 mt_mid = (mt_high+mt_low)/2.; 
    if(!TestComp(mt_mid)) mt_low  = mt_mid;
    else                  mt_high = mt_mid;
  }
  return mt_high;
}

/// Test if for a given event, the trial top mass is compatible with the real top mass
MAbool TransverseVariables::TestComp(const MAfloat64 &mt)
{
  // Quick check if the trial top mass is larger than the two possible thresholds
  if(mt<(p2_.M()+mw_) || mt<(p3_.M()+mw_)) { return false;}

  // Calculate the delta,  delta1 and delta2 quantities
  MAfloat64 delta = (pow(mt,2.) - mw2_ - p3_.M2())/(2.*p3_.Mt2());
  MAfloat64 del1 = mw2_ - p1_.M2();
  MAfloat64 del2 = pow(mt,2.) - mw2_ - p2_.M2() - 2.*plpb1_;

  // Removing pbz
  MAfloat64 aa = (p1_.E()*p2_.Px()-p2_.E()*p1_.Px())/ (p2_.E()*p1_.Pz()-p1_.E()*p2_.Pz());
  MAfloat64 bb = (p1_.E()*p2_.Py()-p2_.E()*p1_.Py())/ (p2_.E()*p1_.Pz()-p1_.E()*p2_.Pz());
  MAfloat64 cc = (p1_.E()*del2-p2_.E()*del1)/(2.*(p2_.E()*p1_.Pz()-p1_.E()*p2_.Pz()));

  // Computing the coefficients of the two quadratic equations
  C1_.resize(0);
  C1_.push_back(E2sq_*(1.+pow(aa,2.))-pow(p2_.Px()+p2_.Pz()*aa,2.));
  C1_.push_back(E2sq_*aa*bb-(p2_.Px()+p2_.Pz()*aa)*(p2_.Py()+p2_.Pz()*bb));
  C1_.push_back(E2sq_*(1.+pow(bb,2.))-pow(p2_.Py()+p2_.Pz()*bb,2.));
  C1_.push_back(E2sq_*aa*cc-(p2_.Px()+p2_.Pz()*aa)*(del2/2.+p2_.Pz()*cc));
  C1_.push_back(E2sq_*bb*cc-(p2_.Py()+p2_.Pz()*bb)*(del2/2.+p2_.Pz()*cc));
  C1_.push_back(E2sq_*pow(cc,2.)-pow(del2/2.+p2_.Pz()*cc,2.));

  // Checking if the first equation admits real solutions
  if( ((C1_[0]*(C1_[2]*C1_[5]-pow(C1_[4],2.))-C1_[1]*(C1_[1]*C1_[5]-C1_[3]*C1_[4])+
    C1_[3]*(C1_[1]*C1_[4]-C1_[2]*C1_[3]))/(C1_[0]+C1_[2]))>0.) { return false; }

  // Defining the coefficients for the second ellipse
  C2_.resize(0);
  C2_.push_back(1.-pow(p3_.Px(),2.)/p3_.Mt2());
  C2_.push_back(-p3_.Px()*p3_.Py()/p3_.Mt2());
  C2_.push_back(1.-pow(p3_.Py(),2.)/p3_.Mt2());
  C2_.push_back(delta*p3_.Px());
  C2_.push_back(delta*p3_.Py());
  C2_.push_back(C2_[0]*pow(pmx_,2.)+2.*C2_[1]*pmx_*pmy_+C2_[2]*pow(pmy_,2.)-2.*C2_[3]*pmx_
    -2.*C2_[4]*pmy_+mw2_-pow(delta,2.)*p3_.Et2());
  C2_[3] += -C2_[0]*pmx_-C2_[1]*pmy_;
  C2_[4] += -C2_[2]*pmy_-C2_[1]*pmx_;

  // Get a point on the 1st ellipse and checks if it lies within the 2nd ellipse
  // It is always possible to define (x0,y0) has ellipse 1 admits real solutions
  // if True, then mt is compatible
  MAfloat64 x0 = (C1_[2]*C1_[3]-C1_[1]*C1_[4])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
  MAfloat64 x0sq = pow(x0,2.);
  MAfloat64 y0 = (-C1_[1]*x0-C1_[4]+
    sqrt(pow(C1_[1]*x0+C1_[4],2.)-C1_[2]*(C1_[0]*x0sq+2.*C1_[3]*x0+C1_[5])))/C1_[2];
  MAfloat64 y0sq = pow(y0,2.);
  if((C2_[0]*x0sq+2.*C2_[1]*x0*y0+C2_[2]*y0sq+2.*C2_[3]*x0+2.*C2_[4]*y0+C2_[5])<0.)
    return true;

  // Computing the number of intersections between the two ellipses and returning the
  // result as a function of the number of intersections
  if (Nsolutions(p2_.E())==0) { return false;}
  return true;
}


/// Wrapper fuction for the computation of mt2w
MAfloat64 TransverseVariables::MT2W(std::vector<const RecJetFormat*> jets, const RecLeptonFormat* lep, const ParticleBaseFormat& met)
{
  /// We need at least 2 jets
  if(jets.size()<2) return 0.;

  /// Split the jet collection according to b-tags
  std::vector<const RecParticleFormat*> bjets, nbjets;
  for(MAuint32 ii=0 ;ii<jets.size(); ii++)
  {
    if(jets[ii]->btag())  bjets.push_back(jets[ii]);
    else                  nbjets.push_back(jets[ii]);
  }
  /// pt-ordering
  SORTER->sort(nbjets,PTordering);
  SORTER->sort(bjets,PTordering);

  /// We neglect the fourth jets and all the others. If less than 3 jets in total
  /// only light jets are considered.
  MAuint32 N=3;
  if(jets.size()<=3) N = nbjets.size();

  /// no b-jets
  /// We select the minimum mt2w obtained from all possible jet combinations
  if(bjets.size()==0)
  {
    MAfloat64 min_mt2w=1e9;
    for (MAuint32 ii=0; ii<N; ii++)
    {
      for (MAuint32 jj=0; jj<N; jj++)
      {
        if (ii==jj) continue;
        MAfloat64 tmp_mt2w = GetMT2W(lep, nbjets[ii], nbjets[jj],met);
        if(tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
      }
    }
    return min_mt2w;
  }

  /// 1 b-jet
  else if (bjets.size()==1)
  {
    MAfloat64 min_mt2w=1e9;
    for (MAuint32 ii=0; ii<N; ii++)
    {
      MAfloat64 tmp_mt2w = GetMT2W(lep,bjets[0],nbjets[ii],met);
      if (tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
      tmp_mt2w = GetMT2W(lep,nbjets[ii],bjets[0],met);
      if (tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
    }
    return min_mt2w;
  }

  /// More than 1 b-tag
  else
  {
    MAfloat64 min_mt2w=1e9;
    for (MAuint32 ii=0; ii<bjets.size(); ii++)
    {
      for (MAuint32 jj=0; jj<bjets.size(); jj++)
      {
        if (ii==jj) continue;
        MAfloat64 tmp_mt2w = GetMT2W(lep, bjets[ii], bjets[jj],met);
        if (tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
      }
    }
    return min_mt2w;
  }
}

MAfloat64 TransverseVariables::MT2W(std::vector<const MCParticleFormat*> jets, const MCParticleFormat* lep, const ParticleBaseFormat& met)
{
  /// We need at least 2 jets
  if(jets.size()<2) return 0.;

  /// Split the jet collection according to b-tags
  std::vector<const MCParticleFormat*> bjets, nbjets;
  for(MAuint32 ii=0 ;ii<jets.size(); ii++)
  {
    if(abs(jets[ii]->pdgid())==5) bjets.push_back(jets[ii]);
    else                          nbjets.push_back(jets[ii]);
  }
  /// pt-ordering
  SORTER->sort(nbjets,PTordering);
  SORTER->sort(bjets,PTordering);

  /// We neglect the fourth jets and all the others. If less than 3 jets in total
  /// only light jets are considered.
  MAuint32 N=3;
  if(jets.size()<=3) N = nbjets.size();

  /// no b-jets
  /// We select the minimum mt2w obtained from all possible jet combinations
  if(bjets.size()==0)
  {
    MAfloat64 min_mt2w=1e9;
    for (MAuint32 ii=0; ii<N; ii++)
    {
      for (MAuint32 jj=0; jj<N; jj++)
      {
        if (ii==jj) continue;
        MAfloat64 tmp_mt2w = GetMT2W(lep, nbjets[ii], nbjets[jj],met);
        if(tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
      }
    }
    return min_mt2w;
  }

  /// 1 b-jet
  else if (bjets.size()==1)
  {
    MAfloat64 min_mt2w=1e9;
    for (MAuint32 ii=0; ii<N; ii++)
    {
      MAfloat64 tmp_mt2w = GetMT2W(lep,bjets[0],nbjets[ii],met);
      if (tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
      tmp_mt2w = GetMT2W(lep,nbjets[ii],bjets[0],met);
      if (tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
    }
    return min_mt2w;
  }

  /// More than 1 b-tag
  else
  {
    MAfloat64 min_mt2w=1e9;
    for (MAuint32 ii=0; ii<bjets.size(); ii++)
    {
      for (MAuint32 jj=0; jj<bjets.size(); jj++)
      {
        if (ii==jj) continue;
        MAfloat64 tmp_mt2w = GetMT2W(lep, bjets[ii], bjets[jj],met);
        if (tmp_mt2w < min_mt2w) min_mt2w = tmp_mt2w;
      }
    }
    return min_mt2w;
  }
}



/// The alphaT variable
void LoopForAlphaT(const MAuint32 n1, const std::vector<RecJetFormat>& jets,
                   MAfloat64 &MinDHT, const MAint32 last, std::vector<bool>& Ids, MAuint32 nIds)
{
   // We have enough information to form the pseudo jets
   if(nIds==n1)
   {
     // Computing the DeltaHT of the pseudo jets and checking if minimum
     MAfloat64 THT1 = 0; 
     MAfloat64 THT2 = 0;
     for (MAuint32 i=0;i<Ids.size();i++)
     {
       if (Ids[i]) THT1+=jets[i].et();
       else THT2+=jets[i].et();
     }
     MAfloat64 DeltaHT = std::abs(THT1-THT2);
     if (DeltaHT<MinDHT) MinDHT=DeltaHT;

    // Exit
    return;
   }

   // The first pseudo jet is incomplete -> adding one element
   for(MAuint32 i=(last+1); i<=(jets.size()-n1+nIds); i++)
   {
     // Saving the current state
     Ids[i]=true;
     LoopForAlphaT(n1, jets, MinDHT, i, Ids,nIds+1);
     Ids[i]=false; 
   }
}


/// The alphaT variable
void SlowLoopForAlphaT(const MAuint32 n1, const std::vector<const MCParticleFormat*> jets,
  MAfloat64 &MinDHT, const MAint32 last, std::vector<MAuint32> Ids)
{
   // We have enough information to form the pseudo jets
   if(Ids.size()==n1)
   {
     // Forming the two pseudo jets
     std::vector<const MCParticleFormat*> jets1;
     std::vector<const MCParticleFormat*> jets2=jets;
     for (MAint32 j=n1-1; j>=0; j--)
     { 
        jets1.push_back(jets[Ids[j]]);
        jets2.erase(jets2.begin()+Ids[j]);
     }

     // Computing the DeltaHT of the pseudo jets and checking if minimum
     MAfloat64 THT1 = 0; MAfloat64 THT2 = 0;
     for (MAuint32 i=0;i<jets1.size();i++) THT1+=jets1[i]->et();
     for (MAuint32 i=0;i<jets2.size();i++) THT2+=jets2[i]->et();
     MAfloat64 DeltaHT = std::abs(THT1-THT2);
     if (DeltaHT<MinDHT) MinDHT=DeltaHT;

    // Exit
    return;
   }

   // The first pseudo jet is incomplete -> adding one element
   std::vector<MAuint32> Save=Ids;
   for(MAuint32 i=last+1; i<=jets.size()-n1+Save.size(); i++)
   {
     Ids = Save;   
     Ids.push_back(i);   
     SlowLoopForAlphaT(n1, jets, MinDHT, i, Ids); 
   }
}

void SlowLoopForAlphaT(const MAuint32 n1, std::vector<RecJetFormat> jets,
  MAfloat64 &MinDHT, const MAint32 last, std::vector<MAuint32> Ids)
{
   // We have enough information to form the pseudo jets
   if(Ids.size()==n1)
   {
     // Forming the two pseudo jets
     std::vector<RecJetFormat> jets1;
     std::vector<RecJetFormat> jets2=jets;
     for (MAint32 j=n1-1; j>=0; j--)
     { 
        jets1.push_back(jets[Ids[j]]);
        jets2.erase(jets2.begin()+Ids[j]);
     }

     // Computing the DeltaHT of the pseudo jets and checking if minimum
     MAfloat64 THT1 = 0; MAfloat64 THT2 = 0;
     for (MAuint32 i=0;i<jets1.size();i++) THT1+=jets1[i].et();
     for (MAuint32 i=0;i<jets2.size();i++) THT2+=jets2[i].et();
     MAfloat64 DeltaHT = std::abs(THT1-THT2);
     if (DeltaHT<MinDHT) MinDHT=DeltaHT;

    // Exit
    return;
   }

   // The first pseudo jet is incomplete -> adding one element
   std::vector<MAuint32> Save=Ids;
   for(MAuint32 i=last+1; i<=jets.size()-n1+Save.size(); i++)
   {
     Ids = Save;   
     Ids.push_back(i);   
     SlowLoopForAlphaT(n1, jets, MinDHT, i, Ids); 
   }
}

MAfloat64 TransverseVariables::AlphaT(const MCEventFormat* event)
{
  std::vector<const MCParticleFormat*> jets;

  // Creating jet collection
  for (MAuint32 i=0;i<event->particles().size();i++)
  {
    if (event->particles()[i].statuscode()!=event->particles()[event->particles().size()-1].statuscode()) continue;
    if (event->particles()[i].pdgid()!=21 && (abs(event->particles()[i].pdgid())<1 || 
        abs(event->particles()[i].pdgid())>5)) continue;
    jets.push_back(&event->particles()[i]);
  }

  // safety
  if (jets.size()<2) return 0;

  // dijet event
  if (jets.size()==2) return std::min(jets[0]->et(),jets[1]->et()) /
                             ( jets[0]->momentum()+jets[1]->momentum() ).Mt();

  MAfloat64 MinDeltaHT = 1e6;

  // compute vectum sum of jet momenta
  MALorentzVector q(0.,0.,0.,0.);
  for (MAuint32 i=0;i<jets.size();i++) q+=jets[i]->momentum();
  MAfloat64 MHT = q.Pt();

  // compute HT
  MAfloat64 THT = 0;
  for (MAuint32 i=0;i<jets.size();i++) THT+=jets[i]->et();

  // Safety
  if (THT==0) return -1.;
  else if (MHT/THT>=1) return -1.;

  // more than 3 jets : split into 2 sets
  // n1 = number of jets in the first set
  // n2 = number of jets in the second set
  for (MAuint32 n1=1; n1<=(jets.size()/2); n1++)
  {
    std::vector<MAuint32> DummyJet;
    SlowLoopForAlphaT(n1,jets,MinDeltaHT,-1,DummyJet);
  }

  // Final
  return 0.5*(1.-MinDeltaHT/THT)/sqrt(1.-MHT/THT*MHT/THT);
}

MAfloat64 TransverseVariables::SlowAlphaT(const MCEventFormat* event)
{
  std::vector<const MCParticleFormat*> jets;

  // Creating jet collection
  for (MAuint32 i=0;i<event->particles().size();i++)
  {
    if (event->particles()[i].statuscode()!=event->particles()[event->particles().size()-1].statuscode()) continue;
    if (event->particles()[i].pdgid()!=21 && (abs(event->particles()[i].pdgid())<1 || 
        abs(event->particles()[i].pdgid())>5)) continue;
    jets.push_back(&event->particles()[i]);
  }

  // safety
  if (jets.size()<2) return 0;

  // dijet event
  if (jets.size()==2) return std::min(jets[0]->et(),jets[1]->et()) /
                             ( jets[0]->momentum()+jets[1]->momentum() ).Mt();

  MAfloat64 MinDeltaHT = 1e6;

  // compute vectum sum of jet momenta
  MALorentzVector q(0.,0.,0.,0.);
  for (MAuint32 i=0;i<jets.size();i++) q+=jets[i]->momentum();
  MAfloat64 MHT = q.Pt();

  // compute HT
  MAfloat64 THT = 0;
  for (MAuint32 i=0;i<jets.size();i++) THT+=jets[i]->et();

  // Safety
  if (THT==0) return -1.;
  else if (MHT/THT>=1) return -1.;

  // more than 3 jets : split into 2 sets
  // n1 = number of jets in the first set
  // n2 = number of jets in the second set
  for (MAuint32 n1=1; n1<=(jets.size()/2); n1++)
  {
    std::vector<MAuint32> DummyJet;
    SlowLoopForAlphaT(n1,jets,MinDeltaHT,-1,DummyJet);
  }

  // Final
  return 0.5*(1.-MinDeltaHT/THT)/sqrt(1.-MHT/THT*MHT/THT);
}

MAfloat64 TransverseVariables::AlphaT(const RecEventFormat* event)
{
  // jets
  const std::vector<RecJetFormat>& jets = event->jets();

  // safety
  if (jets.size()<2) return 0;

  // dijet event: the easiest case
  if (jets.size()==2) return std::min(jets[0].et(),jets[1].et()) / 
  ((jets[0].momentum())+(jets[1].momentum())).Mt();

  // more than 2 jets: loops
  MAfloat64 MinDeltaHT = 1e6;

  // compute vectum sum of jet momenta
  MALorentzVector q(0.,0.,0.,0.);
  for (MAuint32 i=0;i<jets.size();i++) q+=jets[i].momentum();
  MAfloat64 MHT = q.Pt();

  // compute HT
  MAfloat64 THT = 0;
  for (MAuint32 i=0;i<jets.size();i++) THT+=jets[i].et();

  // Safety
  if (THT==0) return -1.;
  else if (MHT/THT>=1) return -1.;

  // split into 2 sets
  // n1 = number of jets in the first set
  // n2 = number of jets in the second set
  std::vector<bool> DummyJet(jets.size(),false);
  for (MAuint32 n1=1; n1<=(jets.size()/2); n1++)
  {
    std::fill(DummyJet.begin(),DummyJet.end(),false);
    LoopForAlphaT(n1,jets,MinDeltaHT,-1,DummyJet,0);
  }

  // Final
  return 0.5*(1.-MinDeltaHT/THT)/sqrt(1.-MHT/THT*MHT/THT);
}




MAfloat64 TransverseVariables::SlowAlphaT(const RecEventFormat* event)
{
  // jets
  std::vector<RecJetFormat> jets = event->jets();

  // safety
  if (jets.size()<2) return 0;

  // dijet event
  if (jets.size()==2) return std::min(jets[0].et(),jets[1].et()) / 
  ((jets[0].momentum())+(jets[1].momentum())).Mt();

  MAfloat64 MinDeltaHT = 1e6;

  // compute vectum sum of jet momenta
  MALorentzVector q(0.,0.,0.,0.);
  for (MAuint32 i=0;i<jets.size();i++) q+=jets[i].momentum();
  MAfloat64 MHT = q.Pt();

  // compute HT
  MAfloat64 THT = 0;
  for (MAuint32 i=0;i<jets.size();i++) THT+=jets[i].et();

  // Safety
  if (THT==0) return -1.;
  else if (MHT/THT>=1) return -1.;

  // more than 3 jets : split into 2 sets
  // n1 = number of jets in the first set
  // n2 = number of jets in the second set
  for (MAuint32 n1=1; n1<=(jets.size()/2); n1++)
          {
            std::vector<MAuint32> DummyJet;
    SlowLoopForAlphaT(n1,jets,MinDeltaHT,-1,DummyJet);
  }

  // Final
  return 0.5*(1.-MinDeltaHT/THT)/sqrt(1.-MHT/THT*MHT/THT);
}

