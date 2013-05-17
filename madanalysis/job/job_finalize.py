################################################################################
#  
#  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
#  
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#  
################################################################################


import logging
def WriteJobFinalize(file,main):
 
    # Function header
    file.write('void user::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)\n{\n')

    # Opening ROOT file
    #file.write('  // Creating output root file\n')
    #file.write('  TFile* output = new TFile((outputName_+".root").c_str(),"RECREATE");\n')

    # Creating subdirectories
    #file.write('  // Creating directories in the ROOT file\n')
    #file.write('  output->mkdir("plots");\n')

    # Saving histos in a ROOT file
    file.write('  // Saving histogram\n')
    #file.write('  output->cd("plots");\n')
    #file.write('  plots_.Write_RootFormat(output);\n')
    file.write('  *out().GetStream() << "<Selection>\\n";\n\n')
    file.write('  plots_.Write_TextFormat(out());\n')

    # Saving cuts in a ROOT file
    file.write('  // Saving cut cuts\n')
    file.write('  cuts_.Write_TextFormat(out());\n\n')
    file.write('  *out().GetStream() << "</Selection>\\n";\n\n')

    # Finalizing cuts and histos
    file.write('  // Finalizing cuts and histos\n')
    file.write('  plots_.Finalize();\n')
    file.write('  cuts_.Finalize();\n\n')

    # End
    #file.write('  // Closing the output file\n')
    #file.write('  delete output;\n')
    file.write('}\n')


