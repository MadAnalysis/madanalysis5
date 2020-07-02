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


// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/DataFormat/MCEventFormat.h"
#include <set>


using namespace MA5;

struct VertexInfo
{
  std::vector<const MCParticleFormat*> in;
  std::vector<const MCParticleFormat*> out;
};

MAbool CompareMothers(const std::vector<const MCParticleFormat*>& a, const std::vector<const MCParticleFormat*>& b)
{
  std::set<const MCParticleFormat*> aa;
  std::set<const MCParticleFormat*> bb;
  for (MAuint32 i=0;i<a.size();i++) aa.insert(a[i]);
  for (MAuint32 i=0;i<b.size();i++) bb.insert(b[i]);
  if (aa==bb) return true; else return false;
}

MAbool CompareMothers(const std::vector<const MCParticleFormat*>& a, const std::vector<MCParticleFormat*>& b)
{
  std::set<const MCParticleFormat*> aa;
  std::set<const MCParticleFormat*> bb;
  for (MAuint32 i=0;i<a.size();i++) aa.insert(a[i]);
  for (MAuint32 i=0;i<b.size();i++) bb.insert(b[i]);
  if (aa==bb) return true; else return false;
}

void MCEventFormat::PrintVertices() const
{
  std::vector<VertexInfo> vertices;
  for (MAuint32 i=0;i<particles_.size();i++)
  {
    const MCParticleFormat* part = &(particles_[i]);
    if (part->mothers().empty())   continue;

    VertexInfo myVertex;
    for (MAuint32 j=0;j<part->mothers().size();j++)
    {
      myVertex.in.push_back(part->mothers()[j]);
    }
    for (MAuint32 j=0;j<particles_.size();j++)
    {
      if (CompareMothers(myVertex.in,particles_[j].mothers()))
      {
        myVertex.out.push_back(&(particles_[j]));
      }
    }
    MAbool ok=true;
    for (MAuint32 j=0;j<vertices.size();j++)
    {
      if (CompareMothers(myVertex.in,vertices[j].in) && CompareMothers(myVertex.out,vertices[j].out))
      {
        ok=false; break;
      }
    }
    if (ok) vertices.push_back(myVertex);
  }
  std::cout << "# vertices = " << vertices.size() << std::endl;
  for (MAuint32 i=0;i<vertices.size();i++)
  {
    std::cout << " - ( ";
    for (MAuint32 j=0;j<vertices[i].in.size();j++)
    {
      if (j!=0) std::cout << " ++ ";
      std::cout << vertices[i].in[j]->pdgid();
    }
    std::cout << " ) --> ( ";
    for (MAuint32 j=0;j<vertices[i].out.size();j++)
    {
      if (j!=0) std::cout << " ++ ";
      std::cout << vertices[i].out[j]->pdgid();
    }
    std::cout << " )" << std::endl;
  }
}

/// Displaying mothers
void MCEventFormat::PrintMothers() const
{
  std::cout << "**********************************************" << std::endl;
  for (MAuint32 i=0;i<particles_.size();i++)
  {
    std::cout << "- ";
    std::cout << std::setw(6) << particles_[i].pdgid() << "]  <-  ";
    for (MAuint32 j=0;j<particles_[i].mothers().size();j++)
    {
      std::cout << std::setw(6) << particles_[i].mothers()[j]->pdgid() << "  ";
    }
    std::cout << std::endl;
  }
  std::cout << "**********************************************" << std::endl;
}


/// Displaying daughters
void MCEventFormat::PrintDaughters() const
{
  std::cout << "**********************************************" << std::endl;
  for (MAuint32 i=0;i<particles_.size();i++)
  {
    std::cout << "- ";
    std::cout << std::setw(6) << particles_[i].pdgid() << "]  <-  ";
    for (MAuint32 j=0;j<particles_[i].daughters().size();j++)
    {
      std::cout << std::setw(6) << particles_[i].daughters()[j]->pdgid() << "  ";
    }
    std::cout << std::endl;
  }
  std::cout << "**********************************************" << std::endl;
}
