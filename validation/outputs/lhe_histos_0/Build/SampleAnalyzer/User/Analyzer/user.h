#ifndef analysis_user_h
#define analysis_user_h

#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"
#include "SampleAnalyzer/Interfaces/root/RootMainHeaders.h"

namespace MA5
{
class user : public AnalyzerBase
{
    INIT_ANALYSIS(user, "MadAnalysis5job")

    public : 
        MAbool Initialize(const MA5::Configuration& cfg,
                          const std::map<std::string,std::string>& parameters);
        void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);
        MAbool Execute(SampleFormat& sample, const EventFormat& event);

    private : 
  // Declaring particle containers
  std::vector<const MCParticleFormat*> _P_e_I2I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_mu_I2I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_b_I2I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_j_I2I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_e_I1I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_mu_I1I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_b_I1I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_j_I1I_PTorderingfinalstate_REG_;
  std::vector<const MCParticleFormat*> _P_ePTorderingfinalstate_REG_;
  MAbool isP__ePTorderingfinalstate(const MCParticleFormat* part) const {
     if ( part==0 ) return false;
     if ( !PHYSICS->Id->IsFinalState(part) ) return false;
     if ( (part->pdgid()!=-11)&&(part->pdgid()!=11) ) return false;
     return true; }
  std::vector<const MCParticleFormat*> _P_muPTorderingfinalstate_REG_;
  MAbool isP__muPTorderingfinalstate(const MCParticleFormat* part) const {
     if ( part==0 ) return false;
     if ( !PHYSICS->Id->IsFinalState(part) ) return false;
     if ( (part->pdgid()!=-13)&&(part->pdgid()!=13) ) return false;
     return true; }
  std::vector<const MCParticleFormat*> _P_bPTorderingfinalstate_REG_;
  MAbool isP__bPTorderingfinalstate(const MCParticleFormat* part) const {
     if ( part==0 ) return false;
     if ( !PHYSICS->Id->IsFinalState(part) ) return false;
     if ( (part->pdgid()!=5) ) return false;
     return true; }
  std::vector<const MCParticleFormat*> _P_jPTorderingfinalstate_REG_;
  MAbool isP__jPTorderingfinalstate(const MCParticleFormat* part) const {
     if ( part==0 ) return false;
     if ( !PHYSICS->Id->IsFinalState(part) ) return false;
     if ( (part->pdgid()!=-4)&&(part->pdgid()!=-3)&&(part->pdgid()!=-2)&&(part->pdgid()!=-1)&&(part->pdgid()!=1)&&(part->pdgid()!=2)&&(part->pdgid()!=3)&&(part->pdgid()!=4)&&(part->pdgid()!=21) ) return false;
     return true; }
};
}

#endif