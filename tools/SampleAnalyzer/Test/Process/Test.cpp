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

#include <cstdlib>
#include <map>
#include <string>
#include <iostream>

// SampleHeader header
#include "SampleAnalyzer/Process/Core/SampleAnalyzer.h"
#include "SampleAnalyzer/Process/JetClustering/JetClusterer.h"

using namespace MA5;

// simple test helper
static int g_failures = 0;
static void CHECK(const char *name, bool cond)
{
    if (cond)
    {
        std::cout << "[  OK  ] " << name << std::endl;
    }
    else
    {
        std::cout << "[FAILED] " << name << std::endl;
        ++g_failures;
    }
}

// -----------------------------------------------------------------------
// main program
// -----------------------------------------------------------------------
MAint32 main(MAint32 argc, MAchar *argv[])
{
    std::cout << "BEGIN-SAMPLEANALYZER-TEST" << std::endl;
    std::cout << std::endl;

    // Creating a manager
    SampleAnalyzer manager;
    if (!manager.Initialize(argc, argv, "pdg.ma5"))
        return 1;
    std::cout << std::endl;

    // List of available analyzers
    INFO << "List of available analyzers:" << endmsg;
    manager.AnalyzerList().Print();
    INFO << endmsg;

    // List of available readers
    INFO << "List of available readers:" << endmsg;
    manager.ReaderList().Print();
    INFO << endmsg;

    // List of available writers
    INFO << "List of available writers:" << endmsg;
    manager.WriterList().Print();
    INFO << endmsg;

    // List of available JetClusterer
    INFO << "List of available JetClusterer:" << endmsg;
    manager.JetClustererList().Print();
    INFO << endmsg;

    // List of available detector simulation
    INFO << "List of available DetectorSimList:" << endmsg;
    manager.DetectorSimList().Print();
    INFO << endmsg;

    std::cout << std::endl
              << "BEGIN-PROCESS-SMOKE-TESTS" << std::endl;

    // JetClusterer smoke tests (safe, do not require full runtime)
    {
        JetClusterer jc(nullptr);
        // GetName should handle null algo pointer safely
        std::string name = jc.GetName();
        CHECK("JetClusterer::GetName() with null algo returns non-empty", !name.empty());
        // Initialize should detect missing algo and return false
        std::map<std::string, std::string> opts;
        bool init_ok = jc.Initialize(opts);
        CHECK("JetClusterer::Initialize() with null algo returns false", init_ok == false);
    }

    // Basic SampleAnalyzer lists existence checks
    {
        bool ok = true;
        // Ensure lists can be queried and have Print() callable (no exception)
        try
        {
            manager.AnalyzerList().Print();
            manager.ReaderList().Print();
            manager.WriterList().Print();
            manager.JetClustererList().Print();
            manager.DetectorSimList().Print();
        }
        catch (...)
        {
            ok = false;
        }
        CHECK("SampleAnalyzer lists printable without throwing", ok);
    }

    std::cout << "END-PROCESS-SMOKE-TESTS" << std::endl
              << std::endl;

    std::cout << "END-SAMPLEANALYZER-TEST" << std::endl;

    return (g_failures == 0) ? 0 : 1;
}