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
 * @file MABoost.h
 * @brief Lorentz boost.
 */

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

    /** @brief Lorentz boost defined by a velocity vector (bx, by, bz). */
    class MABoost
    {

    public:
        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    protected:
        /** @brief Velocity along x (px/E). */
        MAdouble64 bx_;

        /** @brief Velocity along y (py/E). */
        MAdouble64 by_;

        /** @brief Velocity along z (pz/E). */
        MAdouble64 bz_;

        /** @brief Squared velocity, Lorentz factor and (gamma-1)/beta^2. */
        MAdouble64 b2_;
        MAdouble64 gamma_;
        MAdouble64 gamma2_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor (all members set to zero, i.e. gamma = 0: not the identity). */
        MABoost()
        {
            bx_ = 0.;
            by_ = 0.;
            bz_ = 0;
            b2_ = 0;
            gamma_ = 0;
            gamma2_ = 0;
        }

        /**
         * @brief Constructor from a velocity.
         *
         * @param bx velocity along x (px/E).
         * @param by velocity along y (py/E).
         * @param bz velocity along z (pz/E).
         */
        MABoost(MAdouble64 bx, MAdouble64 by, MAdouble64 bz)
        {
            setBoostVector(bx, by, bz);
        }

        /**
         * @brief Constructor from a four-vector (velocity = p/E).
         *
         * @param q four-vector.
         */
        MABoost(const MALorentzVector &q)
        {
            setBoostVector(q);
        }

        /** @brief Destructor. */
        ~MABoost() {}

        /**
         * @brief Set the velocity.
         *
         * @param bx velocity along x.
         * @param by velocity along y.
         * @param bz velocity along z.
         */
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

        /**
         * @brief Set the velocity from a four-vector (p/E).
         *
         * @param q four-vector (warning if E = 0).
         */
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
                // FIXME: 'MABoost();' creates a temporary object: the members of this boost are not reset.
                MABoost();
            }
        }

  /**
   * @brief Accessor to the velocity vector.
   *
   * @return the velocity.
   */
  MAVector3 BoostVector() const { return MAVector3(bx_, by_, bz_); }


        /**
         * @brief Accessor to beta (see the FIXME).
         *
         * @return beta squared (b2_), not beta.
         */
        // FIXME: returns the squared velocity instead of beta.
        const MAdouble64 beta() const { return b2_; }

        /**
         * @brief Accessor to the Lorentz factor.
         *
         * @return gamma.
         */
        const MAdouble64 gamma() const { return gamma_; }

        /**
         * @brief Accessor to the velocity vector.
         *
         * @return the velocity.
         */
        const MAVector3 velocity() const { return MAVector3(bx_, by_, bz_); }

        /**
         * @brief Boost a four-vector in place.
         *
         * @param p four-vector.
         */
        void boost(MALorentzVector &p) const
        {
            MAdouble64 bp = bx_ * p.X() + by_ * p.Y() + bz_ * p.Z();
            p.SetX(p.X() + gamma2_ * bp * bx_ + gamma_ * bx_ * p.T());
            p.SetY(p.Y() + gamma2_ * bp * by_ + gamma_ * by_ * p.T());
            p.SetZ(p.Z() + gamma2_ * bp * bz_ + gamma_ * bz_ * p.T());
            p.SetT(gamma_ * (p.T() + bp));
        }

        /**
         * @brief Boost a four-vector.
         *
         * @param q four-vector.
         * @return the boosted four-vector.
         */
        MALorentzVector operator*(const MALorentzVector &q) const
        {
            MALorentzVector q2 = q;
            boost(q2);
            return q2;
        }
    };

}

#endif
