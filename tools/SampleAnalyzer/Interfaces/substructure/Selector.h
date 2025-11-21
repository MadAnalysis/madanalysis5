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

#ifndef MADANALYSIS5_SELECTOR_H
#define MADANALYSIS5_SELECTOR_H

// FastJet headers
#include "fastjet/Selector.hh"

// SampleAnalyser headers
#include "SampleAnalyzer/Commons/Base/PortableDatatypes.h"

using namespace std;

namespace MA5 {
    namespace Substructure {

        class Filter;
        // These classes act as placeholder for Fastjet selectors

        class Selector {
            friend class SelectorNHardest;
            friend class SelectorPtFractionMin;
            friend class Filter;

            protected:
                fastjet::Selector selector_;

            public:
                Selector() {}
                virtual ~Selector( ) {}
                Selector operator * (Selector & s2)
                {
                    Selector new_selector;
                    fastjet::Selector selector = this->__get() * s2.__get();
                    new_selector.__set(selector);
                    return new_selector;
                }

            private:
                fastjet::Selector __get() {return selector_;}
                void __set(fastjet::Selector selector) { selector_ = selector;}
        };

        class SelectorNHardest: public Selector {
            public:
                SelectorNHardest(){}
                virtual ~SelectorNHardest() {}
                SelectorNHardest(MAint32 n)
                { selector_ = fastjet::SelectorNHardest(n); }
        };

        class SelectorPtFractionMin: public Selector {
            public:
                SelectorPtFractionMin(){}
                virtual ~SelectorPtFractionMin() {}
                SelectorPtFractionMin(MAfloat32 frac)
                { selector_ = fastjet::SelectorPtFractionMin(frac); }
        };

    }
}

#endif //MADANALYSIS5_SELECTOR_H
