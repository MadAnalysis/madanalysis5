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

// STL headers

// SampleAnalyzer headers
#include "SampleAnalyzer/Service/TransverseVariables.h"
#include "SampleAnalyzer/Service/LogService.h"


using namespace MA5;


inline int signchange_n(long double t1,long double t2,long double t3,long double t4,long double t5)
{
  int nsc=0;
  if(t1*t2>0) nsc++;
  if(t2*t3>0) nsc++;
  if(t3*t4>0) nsc++;
  if(t4*t5>0) nsc++;
  return nsc;
}

inline int signchange_p(long double t1,long double t2,long double t3,long double t4,long double t5)
{
   int nsc=0;
   if(t1*t2<0) nsc++;
   if(t2*t3<0) nsc++;
   if(t3*t4<0) nsc++;
   if(t4*t5<0) nsc++;
   return nsc;
}

int TransverseVariables::Nsolutions()
{
  //obtain the coefficients for the 4th order equation
  //divided by Ea^n to make the variable dimensionless
  long double A4 = -4.*C2_[0]*C1_[1]*C2_[1]*C1_[2] + 4.*C1_[0]*C2_[1]*C2_[1]*C1_[2] +
    C2_[0]*C2_[0]*C1_[2]*C1_[2] + 4.*C2_[0]*C1_[1]*C1_[1]*C2_[2] -
    4.*C1_[0]*C1_[1]*C2_[1]*C2_[2] - 2.*C1_[0]*C2_[0]*C1_[2]*C2_[2] + C1_[0]*C1_[0]*C2_[2]*C2_[2];
  long double A3 = (-4.*C2_[0]*C2_[1]*C1_[2]*C1_[3] + 8.*C2_[0]*C1_[1]*C2_[2]*C1_[3] -
    4.*C1_[0]*C2_[1]*C2_[2]*C1_[3] - 4.*C2_[0]*C1_[1]*C1_[2]*C2_[3] +
    8.*C1_[0]*C2_[1]*C1_[2]*C2_[3] - 4.*C1_[0]*C1_[1]*C2_[2]*C2_[3] -
    8.*C2_[0]*C1_[1]*C2_[1]*C1_[4] + 8.*C1_[0]*C2_[1]*C2_[1]*C1_[4] +
    4.*C2_[0]*C2_[0]*C1_[2]*C1_[4] - 4.*C1_[0]*C2_[0]*C2_[2]*C1_[4] +
    8.*C2_[0]*C1_[1]*C1_[1]*C2_[4] - 8.*C1_[0]*C1_[1]*C2_[1]*C2_[4] -
    4.*C1_[0]*C2_[0]*C1_[2]*C2_[4] + 4.*C1_[0]*C1_[0]*C2_[2]*C2_[4])/p2_.Mt();
  long double A2 = (4.*C2_[0]*C2_[2]*C1_[3]*C1_[3] - 4.*C2_[0]*C1_[2]*C1_[3]*C2_[3] -
    4.*C1_[0]*C2_[2]*C1_[3]*C2_[3] + 4.*C1_[0]*C1_[2]*C2_[3]*C2_[3] -
    8.*C2_[0]*C2_[1]*C1_[3]*C1_[4] - 8.*C2_[0]*C1_[1]*C2_[3]*C1_[4] +
    16.*C1_[0]*C2_[1]*C2_[3]*C1_[4] + 4.*C2_[0]*C2_[0]*C1_[4]*C1_[4] +
    16.*C2_[0]*C1_[1]*C1_[3]*C2_[4] - 8.*C1_[0]*C2_[1]*C1_[3]*C2_[4] -
    8.*C1_[0]*C1_[1]*C2_[3]*C2_[4] - 8.*C1_[0]*C2_[0]*C1_[4]*C2_[4] +
    4.*C1_[0]*C1_[0]*C2_[4]*C2_[4] - 4.*C2_[0]*C1_[1]*C2_[1]*C1_[5] +
    4.*C1_[0]*C2_[1]*C2_[1]*C1_[5] + 2.*C2_[0]*C2_[0]*C1_[2]*C1_[5] -
    2.*C1_[0]*C2_[0]*C2_[2]*C1_[5] + 4.*C2_[0]*C1_[1]*C1_[1]*C2_[5] -
    4.*C1_[0]*C1_[1]*C2_[1]*C2_[5] - 2.*C1_[0]*C2_[0]*C1_[2]*C2_[5] +
    2.*C1_[0]*C1_[0]*C2_[2]*C2_[5])/p2_.Mt2();
  long double A1 = (-8.*C2_[0]*C1_[3]*C2_[3]*C1_[4] + 8.*C1_[0]*C2_[3]*C2_[3]*C1_[4] +
    8.*C2_[0]*C1_[3]*C1_[3]*C2_[4] - 8.*C1_[0]*C1_[3]*C2_[3]*C2_[4] -
    4.*C2_[0]*C2_[1]*C1_[3]*C1_[5] - 4.*C2_[0]*C1_[1]*C2_[3]*C1_[5] +
    8.*C1_[0]*C2_[1]*C2_[3]*C1_[5] + 4.*C2_[0]*C2_[0]*C1_[4]*C1_[5] -
    4.*C1_[0]*C2_[0]*C2_[4]*C1_[5] + 8.*C2_[0]*C1_[1]*C1_[3]*C2_[5] -
    4.*C1_[0]*C2_[1]*C1_[3]*C2_[5] - 4.*C1_[0]*C1_[1]*C2_[3]*C2_[5] -
    4.*C1_[0]*C2_[0]*C1_[4]*C2_[5] + 4.*C1_[0]*C1_[0]*C2_[4]*C2_[5])/pow(p2_.Mt(),3);
  long double A0 = (-4.*C2_[0]*C1_[3]*C2_[3]*C1_[5] + 4.*C1_[0]*C2_[3]*C2_[3]*C1_[5] +
    C2_[0]*C2_[0]*C1_[5]*C1_[5] + 4.*C2_[0]*C1_[3]*C1_[3]*C2_[5] -
    4.*C1_[0]*C1_[3]*C2_[3]*C2_[5] - 2.*C1_[0]*C2_[0]*C1_[5]*C2_[5] +
    C1_[0]*C1_[0]*C2_[5]*C2_[5])/pow(p2_.Mt2(),2);

  long double C2 = -(A2/2. - 3.*pow(A3,2)/(16.*A4));
  long double C1 = -(3.*A1/4. -A2*A3/(8.*A4));
  long double C0 = -A0 + A1*A3/(16.*A4);
  long double D1 = -2.*A2 - (4.*A4*C1*C1/C2 - 4.*A4*C0 -3.*A3*C1)/C2;
  long double D0 = -A1 - 4.*A4*C0*C1/pow(C2,2) + 3.*A3*C0/C2;
  long double E0 = -C0 - C2*D0*D0/(D1*D1) + C1*D0/D1;

  // Find the coefficients for the leading term in the Sturm sequence
  // The number of solutions depends on diffence of number of sign changes
  // for x->Inf and x->-Inf
  int nsol = signchange_n(A4,A4,C2,D1,E0) - signchange_p(A4,A4,C2,D1,E0);
  if (nsol < 0) nsol = 0; //rounding effects
  return nsol;
}


template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

int TransverseVariables::Nsolutions_massless(const double &dsq)
{
  //obtain the coefficients for the 4th order equation
  //divided by Ea^n to make the variable dimensionless
  long double a = sgn(p2_.Px())*p2_.Mt()/dsq;
  long double b = sgn(p2_.Px())*(msq_*p2_.Mt()/dsq - dsq/(4.*p2_.Mt()));
  long double A4 = a*a*C2_[0];
  long double A3 = 2.*a*C2_[1]/p2_.Mt();
  long double A2 = (2.*a*C2_[0]*b+C2_[2]+2.*a*C2_[3])/p2_.Mt2();
  long double A1 = (2.*b*C2_[1]+2.*C2_[4])/pow(p2_.Mt(),3);
  long double A0 = (C2_[0]*b*b+2.*b*C2_[3]+C2_[5])/pow(p2_.Mt2(),2);

  long double C2 = -(A2/2.-3.*pow(A3,2)/(16.*A4));
  long double C1 = -(3.*A1/4.-A2*A3/(8.*A4));
  long double C0 = -A0+A1*A3/(16.*A4);
  long double D1 = -2.*A2-(4.*A4*C1*C1/C2 -4.*A4*C0-3.*A3*C1)/C2;
  long double D0 = -A1-4.*A4*C0*C1/(C2*C2)+3.*A3*C0/C2;
  long double E0 = -C0 - C2*D0*D0/(D1*D1) + C1*D0/D1;

  // Find the coefficients for the leading term in the Sturm sequence
  // The number of solutions depends on diffence of number of sign changes
  // for x->Inf and x->-Inf
  int nsol = signchange_n(A4,A4,C2,D1,E0)-signchange_p(A4,A4,C2,D1,E0);
  if( nsol < 0 ) nsol=0;  // possible rounding effects
  return nsol;
}



bool TransverseVariables::FindHigh(double &dsqH)
{
   double x0 = (C1_[2]*C1_[3]-C1_[1]*C1_[4])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
   double y0 = (C1_[0]*C1_[4]-C1_[1]*C1_[3])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
   double dsqL = m_*(m_+2.*p2_.M());
   do
   {
      double dsqM = (dsqH + dsqL)/2.;
      UpdateC1((dsqM-p2_.M2())/(2.*p2_.Mt2()));
      UpdateC2(((dsqM-p1_.M2())/2.+p1met_)/p1_.Mt2());
      int nsolM = Nsolutions();
      if     (nsolM==2) { dsqH = dsqM; return true; }
      else if(nsolM==4) { dsqH = dsqM; continue; }
      else if(nsolM==0)
      {
        UpdateC1((dsqM-p2_.M2())/(2.*p2_.Mt2()));
        UpdateC2(((dsqM-p1_.M2())/2.+p1met_)/p1_.Mt2());
        // Does the larger ellipse contain the smaller one? 
        double dis = C2_[0]*x0*x0+2.*C2_[1]*x0*y0+C2_[2]*y0*y0+2.*C2_[3]*x0+2.*C2_[4]*y0+C2_[5];
        if(dis<0) dsqH=dsqM;
        else      dsqL=dsqM;
      }
   } while ((dsqH-dsqL)>0.001);
   return false;
}

double TransverseVariables::GetMT2()
{
  // massless case
  if(p1_.M()<=0.1 && p2_.M()<=0.1)  { return GetMT2_massless(); }

  // Solving the two quadratic equations: initialization of the coefficients
  double dsq0 = p2_.M()*(p2_.M() + 2.*m_);
  InitCoefs();
  UpdateC1( (dsq0-p2_.M2())/(2.*p2_.Mt2()) );
  UpdateC2( ((dsq0-p1_.M2())/2.+p1met_)/p1_.Mt2() );

  // Get the center of the ellipses amd check if the larger ellipse contains
  // the smaller one
  double x0 = (C1_[2]*C1_[3]-C1_[1]*C1_[4])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
  double y0 = (C1_[0]*C1_[4]-C1_[1]*C1_[3])/(C1_[1]*C1_[1]-C1_[0]*C1_[2]);
  double dis= C2_[0]*x0*x0+2.*C2_[1]*x0*y0+C2_[2]*y0*y0+2.*C2_[3]*x0+2.*C2_[4]*y0+C2_[5];
  if(dis<=0.01) { return sqrt(msq_+dsq0); }

  // If not, check if the larger ellipse contains the center of the smaller one
  // and get two estimates for an upper bound on MT2 (dsqH)
  double p2x0 = pmx_-x0, p2y0 = pmy_-y0;
  double dsqH = 2.*(p1_.Mt()*sqrt(pow(p2x0,2)+pow(p2y0,2)+msq_)-p1_.Px()*p2x0-p1_.Py()*p2y0)
    +p1_.M2();
  double dsqH2 = 2.*(p1_.Mt()*sqrt(pmtm_)-p1met_)+p1_.M2();
  double dsqH3 = 2.*p2_.Mt()*m_ + p2_.M2();
  if(dsqH3 > dsqH2) dsqH2 = dsqH3;
  if(dsqH  > dsqH2) dsqH  = dsqH2;

  // Calculating the number of solutions: coefficients for the two quadratic equations
  // bissection method
  int nsolL = Nsolutions();
  if(nsolL>0) { return sqrt(msq_+dsq0); }

  UpdateC1( (dsqH-p2_.M2())/(2.*p2_.Mt2()) );
  UpdateC2( ((dsqH-p1_.M2())/2.+p1met_)/p1_.Mt2() );
  int nsolH = Nsolutions();
  if(nsolH==nsolL || nsolH==4) { if(!FindHigh(dsqH)) { return sqrt(dsq0+msq_); } }

  while(sqrt(dsqH+msq_) - sqrt(dsq0+msq_) > 0.001)
  {
    double dsqM = (dsqH+dsq0)/2.;
    UpdateC1( (dsqM-p2_.M2())/(2.*p2_.Mt2()) );
    UpdateC2( ((dsqM-p1_.M2())/2.+p1met_)/p1_.Mt2() );
    int nsolM = Nsolutions();
    if(nsolM==4) { dsqH=dsqM; FindHigh(dsqH); continue; }
    if(nsolM!=nsolL) dsqH=dsqM;
    if(nsolM==nsolL) dsq0=dsqM;
  }
  return sqrt(msq_+dsqH);
}

double TransverseVariables::GetMT2_massless()
{
  // Rotation of all four-momenta so that p2_.Py() = 0
  double th=-atan(p2_.Py()/p2_.Px());
  p2_.RotateZ(th);
  p1_.RotateZ(th);
  double pxtmp = pmx_*cos(th)-pmy_*sin(th);
  double pytmp = sin(th)*pmx_+cos(th)*pmy_;
  pmx_ = pxtmp;
  pmy_ = pytmp;

  // Initialization of the C2 coefficients + dsq0 + proceed with the calculation
  // of the number of solutions for the lower bourd
  double dsq0 = 0.0005/p2_.Mt2();
  InitC(p1_,C2_);
  UpdateC2( (dsq0+p1met_)/p1_.Mt2() );

  // Calculating the number of solutions: coefficients for the two quadratic equations
  // bissection method
  int nsolL = Nsolutions_massless(dsq0);
  if(nsolL>0) { return sqrt(msq_+dsq0); }

  // When both parabolas contain origin: two estimates for an upper bound on MT2 (dsqH)
  double dsqH  = 2.*(p1_.Mt()*sqrt(pmtm_) - p1met_);
  double dsqH2 = 2.*m_*p2_.Mt();
  if(dsqH  < dsqH2) dsqH = dsqH2;

  UpdateC2( (dsqH/2.+p1met_)/p1_.Mt2() );
  int nsolH = Nsolutions_massless(dsqH);

  // Scanning to get a new lower bound (bissection method)
  bool found=false;
  if (nsolH==nsolL)
  {
    for(double mass = m_+0.1; mass < sqrt(msq_+dsqH); mass+=0.1)
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
    double dsqM = (dsqH+dsq0)/2.;
    UpdateC2( (dsqM/2.+p1met_)/p1_.Mt2() );
    int nsolM = Nsolutions_massless(dsqM);
    if(nsolM!=nsolL) dsqH=dsqM;
    if(nsolM==nsolL) dsq0=dsqM;
  }
  return sqrt(dsqH+msq_);
}
