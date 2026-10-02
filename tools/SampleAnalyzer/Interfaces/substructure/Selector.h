////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

/**
 * @file Selector.h
 * @brief Wrappers of the FastJet selectors used by Filter.
 */

#ifndef MADANALYSIS5_SELECTOR_H
#define MADANALYSIS5_SELECTOR_H

// FastJet headers
#include "fastjet/Selector.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

using namespace std;

namespace MA5 {
    namespace Substructure {

        /** @brief Forward declaration (friend class). */
        class Filter;
        // These classes act as placeholder for Fastjet selectors

        /** @brief Wrapper of a FastJet selector. */
        class Selector {
            friend class SelectorNHardest;
            friend class SelectorPtFractionMin;
            friend class Filter;

            protected:
                /** @brief Wrapped FastJet selector. */
                fastjet::Selector selector_;

            public:
                /** @brief Constructor (empty selector). */
                Selector() {}
                /** @brief Destructor. */
                virtual ~Selector( ) {}
                /**
                 * @brief Combine two selectors (logical AND).
                 *
                 * Takes a non-const reference, so a temporary cannot be used as right operand.
                 *
                 * @param s2 second selector.
                 * @return the combined selector.
                 */
                Selector operator * (Selector & s2)
                {
                    Selector new_selector;
                    fastjet::Selector selector = this->__get() * s2.__get();
                    new_selector.__set(selector);
                    return new_selector;
                }

            private:
                /**
                 * @brief Accessor to the wrapped selector.
                 *
                 * @return a copy of the FastJet selector.
                 */
                fastjet::Selector __get() {return selector_;}
                /**
                 * @brief Set the wrapped selector.
                 *
                 * @param selector FastJet selector.
                 */
                void __set(fastjet::Selector selector) { selector_ = selector;}
        };

        /** @brief Selector keeping the n hardest jets. */
        class SelectorNHardest: public Selector {
            public:
                /** @brief Constructor (empty selector). */
                SelectorNHardest(){}
                /** @brief Destructor. */
                virtual ~SelectorNHardest() {}
                /**
                 * @brief Constructor.
                 *
                 * @param n number of jets to keep.
                 */
                SelectorNHardest(MAint32 n)
                { selector_ = fastjet::SelectorNHardest(n); }
        };

        /** @brief Selector keeping the jets carrying at least a given fraction of the total pT. */
        class SelectorPtFractionMin: public Selector {
            public:
                /** @brief Constructor (empty selector). */
                SelectorPtFractionMin(){}
                /** @brief Destructor. */
                virtual ~SelectorPtFractionMin() {}
                /**
                 * @brief Constructor.
                 *
                 * @param frac minimum pT fraction.
                 */
                SelectorPtFractionMin(MAfloat32 frac)
                { selector_ = fastjet::SelectorPtFractionMin(frac); }
        };

    }
}

#endif //MADANALYSIS5_SELECTOR_H
