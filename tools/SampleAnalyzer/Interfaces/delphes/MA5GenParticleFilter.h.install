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

//------------------------------------------------------------------------------

#ifndef MA5GenParticleFilter_h
#define MA5GenParticleFilter_h

/** \class Efficiency
 *
 *  Removes particles with specific pdg codes
  *
 *  \author M. Selvaggi
 *
 */
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesFormula.h"

#include "classes/DelphesModule.h"
#include <vector>

class TIterator;
class TObjArray;

class MA5GenParticleFilter: public DelphesModule
{
public:

  MA5GenParticleFilter();
  ~MA5GenParticleFilter();

  void Init();
  void Process();
  void Finish();

private:

  std::vector<Int_t> fPdgCodes;

  TIterator *fItInputArray; //!

  const TObjArray *fInputArray; //!

  TObjArray *fOutputArray; //!

  void saveDescendants(Candidate* candidate, const std::vector<Candidate*>& table, 
                       std::vector<bool>& good_table, unsigned int level, unsigned int loop=0);

  ClassDef(MA5GenParticleFilter, 1)
};

#endif
