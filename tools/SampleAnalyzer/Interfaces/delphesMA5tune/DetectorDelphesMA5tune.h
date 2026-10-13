////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
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


/**
 * @file DetectorDelphesMA5tune.h
 * @brief DelphesMA5tune detector simulation run inside SampleAnalyzer.
 */

#ifndef DETECTOR_DELPHESMA5TUNE_H
#define DETECTOR_DELPHESMA5TUNE_H


// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/DetectorBase.h"


class ExRootConfReader;
class ExRootTreeWriter;
class ExRootTreeBranch;
class Delphes;
class DelphesFactory;
class TObjArray;
class TDatabasePDG;
class TParticlePDG;
class TFile;

namespace MA5
{

/** @brief Detector simulation with DelphesMA5tune, run event by event on the MC particles. */
class DetectorDelphesMA5tune: public DetectorBase
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
    TObjArray*        jets_;
    TFile*            outputFile_;
    TDatabasePDG*     PDG_;

    // parameters
    MAbool output_;
    std::string outputdir_;
    std::string rootfile_;
    std::map<std::string,std::string> table_;
    MAbool first_;
    MAuint64 nprocesses_;


//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /** @brief Constructor without argument. */
    DetectorDelphesMA5tune() 
    { outputdir_="."; output_=false; first_=false; nprocesses_=0; }

    /** @brief Destructor. */
    virtual ~DetectorDelphesMA5tune()
    {}

    /**
     * @brief Read the card and the options, create the output file and the Delphes modules.
     *
     * @param configFile DelphesMA5tune card.
     * @param options options `output` (0/1), `rootfile` and `outputdir`.
     * @return false in case of error.
     */
    virtual MAbool Initialize(const std::string& configFile, const std::map<std::string,std::string>& options);

    /** @brief Finish the Delphes tasks, write the output tree and delete the Delphes objects. */
    virtual void Finalize();

    /** @brief Print the parameters (nothing). */
    virtual void PrintParam();

    /**
     * @brief Accessor to the name of the detector simulation.
     *
     * @return "delphes".
     */
    // NOTE: the same name as DetectorDelphes is returned.
    virtual std::string GetName()
    { return "delphes"; }

    /**
     * @brief Accessor to the parameters.
     *
     * @return an empty string.
     */
    virtual std::string GetParameters();

    /**
     * @brief Run DelphesMA5tune on an event.
     *
     * @param mySample current sample.
     * @param myEvent current event.
     * @return true.
     */
    virtual MAbool Execute(SampleFormat& mySample, EventFormat& myEvent);

    /**
     * @brief Fill the Event branch of the output tree.
     *
     * @param mySample current sample.
     * @param myEvent current event.
     */
    void StoreEventHeader(SampleFormat& mySample, EventFormat& myEvent);

    /**
     * @brief Give the MC particles of the event to Delphes.
     *
     * @param mySample current sample.
     * @param myEvent current event.
     */
    void TranslateMA5toDELPHES(SampleFormat& mySample, EventFormat& myEvent);
    /**
     * @brief Fill the reconstructed event from the Delphes output.
     *
     * @param mySample current sample.
     * @param myEvent current event.
     */
    void TranslateDELPHEStoMA5(SampleFormat& mySample, EventFormat& myEvent);

};

}

#endif
