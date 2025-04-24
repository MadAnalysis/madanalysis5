/////////////////////////////////////////////////////////////////////////
//   RestFrames: particle physics event analysis library
//   --------------------------------------------------------------------
//   Copyright (c) 2014-2016, Christopher Rogan
/////////////////////////////////////////////////////////////////////////
///
///  \file   ppLabGenFrame.h
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2015 Jan
///
//   This file is part of RestFrames.
//
//   RestFrames is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   RestFrames is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with RestFrames. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////

#ifndef ppLabGenFrame_H
#define ppLabGenFrame_H

#include "SampleAnalyzer/Commons/RestFrames/LabGenFrame.h"

namespace RestFrames {

  ///////////////////////////////////////////////
  // ppLabGenFrame class
  ///////////////////////////////////////////////
  class ppLabGenFrame : public LabGenFrame {
  public:
    ppLabGenFrame(const std::string& sname, 
		  const std::string& stitle);
    virtual ~ppLabGenFrame();
  
    virtual void Clear();

    void SetEnergyP1(double E);
    void SetEnergyP2(double E);
    
    double GetEnergyP1() const;
    double GetEnergyP2() const;

    void SetPDFqqbar();
    void SetPDFgg();
    void SetPDFgq();
    void SetPDFqq();

    virtual double GetProbMCMC(double mass = -1.) const;

  protected:
    virtual bool InitializeGenAnalysis();
    virtual bool IterateMCMC();

  private:
    double m_Ep1;
    double m_Ep2;

    double m_deltaLogX;

    bool m_PDFqqbar;
    bool m_PDFgg;
    bool m_PDFgq;
    bool m_PDFqq;
    virtual double pPDF_q(double x) const;
    virtual double pPDF_qbar(double x) const;
    virtual double pPDF_g(double x) const;

    // proton PDF parameters
    static double m_PDF_eta_1;
    static double m_PDF_eta_2;
    static double m_PDF_eps_u;
    static double m_PDF_g_u;
    static double m_PDF_del_S;
    static double m_PDF_eta_S;
    static double m_PDF_eps_S;
    static double m_PDF_g_S;
    static double m_PDF_A_g;
    static double m_PDF_del_g;
    static double m_PDF_eta_g;
    static double m_PDF_eps_g;
    static double m_PDF_g_g;
    static double m_PDF_A_g1;
    static double m_PDF_del_g1;
    static double m_PDF_eta_g1;
    
  };

}

#endif
