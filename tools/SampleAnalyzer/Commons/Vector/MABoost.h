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

#ifndef MABoost_h
#define MABoost_h

// STL headers
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"
#include "SampleAnalyzer/Commons/Vector/MALorentzVector.h"
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

namespace MA5
{

    class MABoost
    {

    public:
        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /// @brief value of the velocity on x-axis (px / E)
        MAdouble64 bx_;

        /// @brief value of the velocity on y-axis (py / E)
        MAdouble64 by_;

        /// @brief value of the velocity on z-axis (pz / E)
        MAdouble64 bz_;

        MAdouble64 b2_;
        MAdouble64 gamma_;
        MAdouble64 gamma2_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        // Constructors
        MABoost()
        {
            bx_ = 0.;
            by_ = 0.;
            bz_ = 0;
            b2_ = 0;
            gamma_ = 0;
            gamma2_ = 0;
        }

        /// @brief Initialise the boost vector
        /// @param bx value of the velocity on x-axis (px / E)
        /// @param by value of the velocity on y-axis (py / E)
        /// @param bz value of the velocity on z-axis (pz / E)
        MABoost(MAdouble64 bx, MAdouble64 by, MAdouble64 bz)
        {
            setBoostVector(bx, by, bz);
        }

        /// @brief Initialise boost vector with LorentzVector
        /// @param q lorentz vector
        MABoost(const MALorentzVector &q)
        {
            setBoostVector(q);
        }

        // Destructor
        ~MABoost() {}

        /// @brief Setting the boost vector
        /// @param bx value of the velocity on x-axis
        /// @param by value of the velocity on y-axis
        /// @param bz value of the velocity on z-axis
        void setBoostVector(MAdouble64 bx, MAdouble64 by, MAdouble64 bz)
        {
            // boost component
            bx_ = bx;
            by_ = by;
            bz_ = bz;

            // intermediate results
            b2_ = bx_ * bx_ + by_ * by_ + bz_ * bz_;
            gamma_ = 1.0 / std::sqrt(1.0 - b2_);
            gamma2_ = b2_ > 0 ? (gamma_ - 1.0) / b2_ : 0.0;
        }

        /// @brief Setting the boost vector
        /// @param q Lorentz vector input
        void setBoostVector(const MALorentzVector &q)
        {
            try
            {
                if (q.T() == 0)
                    throw EXCEPTION_WARNING("Energy equal to zero. Impossible to compute the boost.", "", 0);
                setBoostVector(q.X() / q.T(), q.Y() / q.T(), q.Z() / q.T());
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
                MABoost();
            }
        }

        /// @brief Accessor to Beta
        /// @return beta value
        const MAdouble64 beta() const { return b2_; }

        /// @brief Accessor to gamma
        /// @return gamma value
        const MAdouble64 gamma() const { return gamma_; }

        /// @brief Accessor to beta vector
        /// @return beta vector as MAVector3
        const MAVector3 velocity() const { return MAVector3(bx_, by_, bz_); }

        /// @brief Boost a given lorentz vector
        /// @param p lorentz vector to be boosted
        void boost(MALorentzVector &p) const
        {
            MAdouble64 bp = bx_ * p.X() + by_ * p.Y() + bz_ * p.Z();
            p.SetX(p.X() + gamma2_ * bp * bx_ + gamma_ * bx_ * p.T());
            p.SetY(p.Y() + gamma2_ * bp * by_ + gamma_ * by_ * p.T());
            p.SetZ(p.Z() + gamma2_ * bp * bz_ + gamma_ * bz_ * p.T());
            p.SetT(gamma_ * (p.T() + bp));
        }

        /// @brief Boost the given lorentz vector
        /// @param q lorentz vector
        /// @return boosted lorentz vector
        MALorentzVector operator*(const MALorentzVector &q) const
        {
            MALorentzVector q2 = q;
            boost(q2);
            return q2;
        }
    };

}

#endif
