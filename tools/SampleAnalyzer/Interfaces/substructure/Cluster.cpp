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
 * @file Cluster.cpp
 * @brief Supposed to implement MA5::Substructure::Cluster.
 */

// FIXME: this file does not implement Substructure::Cluster (Cluster::Initialize() is missing). It
//   contains a copy of a NullSmearer class (include guard NULLSMEARER_H) instead.
#ifndef NULLSMEARER_H
#define NULLSMEARER_H


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Base/SmearerBase.h"


namespace MA5
{
    /** @brief Smearer that does nothing (stale code, see the FIXME above). */
    class NullSmearer: public SmearerBase {
        public:
            /** @brief Constructor. */
            NullSmearer() { }

            /** @brief Destructor. */
            ~NullSmearer() {}

    };
}

#endif
