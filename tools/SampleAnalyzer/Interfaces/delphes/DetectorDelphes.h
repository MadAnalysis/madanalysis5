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


#ifndef DETECTOR_DELPHES_H
#define DETECTOR_DELPHES_H


// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/DetectorBase.h"
#include "SampleAnalyzer/Interfaces/delphes/DelphesMemoryInterface.h"


class ExRootConfReader;
class ExRootTreeWriter;
class ExRootTreeBranch;
class Delphes;
class DelphesFactory;
class TObjArray;
class TFolder;
class TDatabasePDG;
class TParticlePDG;
class TFile;

namespace MA5
{

class DetectorDelphes: public DetectorBase
{

//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  private :
 
    // Delphes objects
    ExRootConfReader* confReader_;
    ExRootTreeWriter* treeWriter_;
    ExRootTreeBranch* branchEvent_;
    ExRootTreeBranch* branchWeight_;
    Delphes*          modularDelphes_;
    DelphesFactory*   factory_;

    // ROOT objects
    TObjArray*        allParticleOutputArray_;
    TObjArray*        stableParticleOutputArray_;
    TObjArray*        partonOutputArray_;
    TFile*            outputFile_;
    TDatabasePDG*     PDG_;
    TFolder*          delphesFolder_;

    // parameters
    MAbool output_;
    std::string outputdir_;
    std::string rootfile_;
    MAbool MA5card_;
    std::map<std::string,std::string> table_;
    DelphesMemoryInterface interface_;
    MAuint64 nprocesses_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor without argument
    DetectorDelphes() 
    { outputdir_="."; output_=false; MA5card_=false; nprocesses_=0;}

    /// Destructor
    virtual ~DetectorDelphes()
    {}

    /// Initialization
    virtual MAbool Initialize(const std::string& configFile, const std::map<std::string,std::string>& options);

    /// Finalization
    virtual void Finalize();

    /// Print parameters
    virtual void PrintParam();

    /// Accessor to the jet clusterer name
    virtual std::string GetName()
    { return "delphes"; }

    /// Accessor to the jet clusterer parameters
    virtual std::string GetParameters();

    /// Jet clustering
    virtual MAbool Execute(SampleFormat& mySample, EventFormat& myEvent);

    /// Translation functions
    void TranslateMA5toDELPHES(SampleFormat& mySample, EventFormat& myEvent);
    void TranslateDELPHEStoMA5(SampleFormat& mySample, EventFormat& myEvent);

    /// Create Event block for Delphes output
    void StoreEventHeader(SampleFormat& mySample, EventFormat& myEvent);

    /// Config File
    virtual const std::string PrintConfigFile() const
    {
      if (MA5card_)
      {
        return "        with the MA5-tuned card: "+GetConfigFile();
      }
      else
      {
        return "        with the Delphes card: "+GetConfigFile();
      }
    }
};

}

#endif
