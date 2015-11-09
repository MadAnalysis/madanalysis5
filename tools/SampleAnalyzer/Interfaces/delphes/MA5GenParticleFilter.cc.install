/*
 *  Delphes: a framework for fast simulation of a generic collider experiment
 *  Copyright (C) 2012-2014  Universite catholique de Louvain (UCL), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \class MA5GenParticleFilter
 *
 *  Removes particles with specific PDG codes
 *
 *  \author M. Selvaggi
 *
 */

#include "modules/MA5GenParticleFilter.h"

#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesFormula.h"

#include "ExRootAnalysis/ExRootResult.h"
#include "ExRootAnalysis/ExRootFilter.h"
#include "ExRootAnalysis/ExRootClassifier.h"

#include "TMath.h"
#include "TString.h"
#include "TFormula.h"
#include "TRandom3.h"
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace std;


void MA5GenParticleFilter::saveDescendants(Candidate* candidate, const std::vector<Candidate*>& table, 
                                           std::vector<bool>& good_table, unsigned int level, unsigned int loop)
{
  //candidate->M1

}

//------------------------------------------------------------------------------

MA5GenParticleFilter::MA5GenParticleFilter() : fItInputArray(0)
{
}

//------------------------------------------------------------------------------

MA5GenParticleFilter::~MA5GenParticleFilter()
{
}

//------------------------------------------------------------------------------

void MA5GenParticleFilter::Init()
{
  ExRootConfParam param;
  Size_t i, size;

  // import input array
  fInputArray = ImportArray(GetString("InputArray", "Delphes/allParticles"));
  fItInputArray = fInputArray->MakeIterator();

  param = GetParam("PdgCode");
  size = param.GetSize();

  // read PdgCodes to be filtered out from the data card
  fPdgCodes.clear();
  for(i = 0; i < size; ++i)
  {
    fPdgCodes.push_back(param[i].GetInt());
  }

  // create output array
  fOutputArray = ExportArray(GetString("OutputArray", "filteredParticles"));
}

//------------------------------------------------------------------------------

void MA5GenParticleFilter::Finish()
{
  if(fItInputArray) delete fItInputArray;
}

//------------------------------------------------------------------------------

void MA5GenParticleFilter::Process()
{
  Candidate *cand;
  Int_t pdgCode;
  Int_t status;
  fItInputArray->Reset();

  unsigned int i=0;
  while((cand = static_cast<Candidate*>(fItInputArray->Next())))
  {

    pdgCode = std::abs(cand->PID);
    status  = std::abs(cand->Status);

    bool keep=false;

    if      (status==-1)               keep=true; // Pythia6: beam particle
    else if (status==3)                keep=true; // Pythia6: hard process
    else if (status>=11 && status<=19) keep=true; // Pythia8: beam particle
    else if (status>=21 && status<=39) keep=true; // Pythia8: hard proces

    if ((pdgCode%100)==5 || (pdgCode%1000)==5) keep=true; //b hadrons
    if ((pdgCode%100)==4 || (pdgCode%1000)==4) keep=true; //c hadrons 
    if (pdgCode==4 || pdgCode==5) keep=true; // b/c quarks
    if( find(fPdgCodes.begin(), fPdgCodes.end(), pdgCode) == fPdgCodes.end()) 
        keep=true;

    if (pdgCode==15) //tau hadrons
    {
      keep=true;
    }

    if (status==1)
    {
      if (pdgCode==11 || pdgCode==13) keep=true; //lepton
      if (pdgCode>=1000000)           keep=true; //susy
      //  saveDescendants(cand,table,good_table,0);
    }

    if (keep) fOutputArray->Add(cand);
    i++;
  }

}

