#include "SampleAnalyzer/User/Analyzer/user.h"
using namespace MA5;

MAbool user::Initialize(const MA5::Configuration& cfg,
                      const std::map<std::string,std::string>& parameters)
{
  // Initializing PhysicsService for MC
  PHYSICS->mcConfig().Reset();

  // definition of the multiparticle "hadronic"
  PHYSICS->mcConfig().AddHadronicId(-5);
  PHYSICS->mcConfig().AddHadronicId(-4);
  PHYSICS->mcConfig().AddHadronicId(-3);
  PHYSICS->mcConfig().AddHadronicId(-2);
  PHYSICS->mcConfig().AddHadronicId(-1);
  PHYSICS->mcConfig().AddHadronicId(1);
  PHYSICS->mcConfig().AddHadronicId(2);
  PHYSICS->mcConfig().AddHadronicId(3);
  PHYSICS->mcConfig().AddHadronicId(4);
  PHYSICS->mcConfig().AddHadronicId(5);
  PHYSICS->mcConfig().AddHadronicId(21);

  // definition of the multiparticle "invisible"
  PHYSICS->mcConfig().AddInvisibleId(-16);
  PHYSICS->mcConfig().AddInvisibleId(-14);
  PHYSICS->mcConfig().AddInvisibleId(-12);
  PHYSICS->mcConfig().AddInvisibleId(12);
  PHYSICS->mcConfig().AddInvisibleId(14);
  PHYSICS->mcConfig().AddInvisibleId(16);
  PHYSICS->mcConfig().AddInvisibleId(1000022);

  // ===== Signal region ===== //
  Manager()->AddRegionSelection("myregion");

  // ===== Selections ===== //

  // ===== Histograms ===== //
  Manager()->AddHisto("1_SQRTS", 50,0.0,5000.0);
  Manager()->AddHisto("2_SCALE", 50,0.0,2000.0);
  Manager()->AddHisto("3_ALPHA_QCD", 50,0.0,0.3);
  Manager()->AddHisto("4_ALPHA_QED", 50,0.0,0.02);
  Manager()->AddHisto("5_TET", 50,0.0,5000.0);
  Manager()->AddHisto("6_THT", 50,0.0,5000.0);
  Manager()->AddHisto("7_MET", 50,0.0,1500.0);
  Manager()->AddHisto("8_MHT", 50,0.0,1500.0);
  Manager()->AddHistoFrequency("9_NPID");
  Manager()->AddHistoFrequency("10_NAPID");
  Manager()->AddHisto("11_WEIGHTS", 100,-1.0,1.0);
  Manager()->AddHisto("12_PT", 50,0.0,1000.0);
  Manager()->AddHisto("13_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("14_ABSETA", 50,0.0,8.0);
  Manager()->AddHisto("15_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("16_E", 50,0.0,2000.0);
  Manager()->AddHisto("17_ET", 50,0.0,2000.0);
  Manager()->AddHisto("18_M", 50,0.0,20.0);
  Manager()->AddHisto("19_MT", 50,0.0,1000.0);
  Manager()->AddHisto("20_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("21_P", 50,0.0,2000.0);
  Manager()->AddHisto("22_PX", 50,-1000.0,1000.0);
  Manager()->AddHisto("23_PY", 50,-1000.0,1000.0);
  Manager()->AddHisto("24_PZ", 50,-2000.0,2000.0);
  Manager()->AddHisto("25_R", 50,0.0,10.0);
  Manager()->AddHisto("26_Y", 50,-8.0,8.0);
  Manager()->AddHisto("27_BETA", 50,0.0,1.2);
  Manager()->AddHisto("28_GAMMA", 50,1.0,100.0);
  Manager()->AddHisto("29_PT", 50,0.0,1000.0);
  Manager()->AddHisto("30_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("31_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("32_E", 50,0.0,2000.0);
  Manager()->AddHisto("33_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("34_PT", 50,0.0,1000.0);
  Manager()->AddHisto("35_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("36_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("37_E", 50,0.0,2000.0);
  Manager()->AddHisto("38_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("39_M", 50,0.0,1000.0);
  Manager()->AddHisto("40_PT", 50,0.0,1500.0);
  Manager()->AddHisto("41_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("42_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("43_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("44_PT", 50,0.0,1000.0);
  Manager()->AddHisto("45_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("46_ABSETA", 50,0.0,8.0);
  Manager()->AddHisto("47_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("48_E", 50,0.0,2000.0);
  Manager()->AddHisto("49_ET", 50,0.0,2000.0);
  Manager()->AddHisto("50_M", 50,0.0,20.0);
  Manager()->AddHisto("51_MT", 50,0.0,1000.0);
  Manager()->AddHisto("52_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("53_P", 50,0.0,2000.0);
  Manager()->AddHisto("54_PX", 50,-1000.0,1000.0);
  Manager()->AddHisto("55_PY", 50,-1000.0,1000.0);
  Manager()->AddHisto("56_PZ", 50,-2000.0,2000.0);
  Manager()->AddHisto("57_R", 50,0.0,10.0);
  Manager()->AddHisto("58_Y", 50,-8.0,8.0);
  Manager()->AddHisto("59_BETA", 50,0.0,1.2);
  Manager()->AddHisto("60_GAMMA", 50,1.0,100.0);
  Manager()->AddHisto("61_PT", 50,0.0,1000.0);
  Manager()->AddHisto("62_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("63_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("64_E", 50,0.0,2000.0);
  Manager()->AddHisto("65_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("66_PT", 50,0.0,1000.0);
  Manager()->AddHisto("67_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("68_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("69_E", 50,0.0,2000.0);
  Manager()->AddHisto("70_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("71_M", 50,0.0,1000.0);
  Manager()->AddHisto("72_PT", 50,0.0,1500.0);
  Manager()->AddHisto("73_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("74_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("75_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("76_PT", 50,0.0,1000.0);
  Manager()->AddHisto("77_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("78_ABSETA", 50,0.0,8.0);
  Manager()->AddHisto("79_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("80_E", 50,0.0,2000.0);
  Manager()->AddHisto("81_ET", 50,0.0,2000.0);
  Manager()->AddHisto("82_M", 50,0.0,50.0);
  Manager()->AddHisto("83_MT", 50,0.0,1000.0);
  Manager()->AddHisto("84_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("85_P", 50,0.0,2000.0);
  Manager()->AddHisto("86_PX", 50,-1000.0,1000.0);
  Manager()->AddHisto("87_PY", 50,-1000.0,1000.0);
  Manager()->AddHisto("88_PZ", 50,-2000.0,2000.0);
  Manager()->AddHisto("89_R", 50,0.0,10.0);
  Manager()->AddHisto("90_Y", 50,-8.0,8.0);
  Manager()->AddHisto("91_BETA", 50,0.0,1.2);
  Manager()->AddHisto("92_GAMMA", 50,1.0,100.0);
  Manager()->AddHisto("93_PT", 50,0.0,1000.0);
  Manager()->AddHisto("94_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("95_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("96_E", 50,0.0,2000.0);
  Manager()->AddHisto("97_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("98_PT", 50,0.0,1000.0);
  Manager()->AddHisto("99_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("100_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("101_E", 50,0.0,2000.0);
  Manager()->AddHisto("102_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("103_M", 50,0.0,2000.0);
  Manager()->AddHisto("104_PT", 50,0.0,2000.0);
  Manager()->AddHisto("105_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("106_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("107_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("108_PT", 50,0.0,1000.0);
  Manager()->AddHisto("109_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("110_ABSETA", 50,0.0,8.0);
  Manager()->AddHisto("111_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("112_E", 50,0.0,2000.0);
  Manager()->AddHisto("113_ET", 50,0.0,2000.0);
  Manager()->AddHisto("114_M", 50,0.0,200.0);
  Manager()->AddHisto("115_MT", 50,0.0,1000.0);
  Manager()->AddHisto("116_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("117_P", 50,0.0,2000.0);
  Manager()->AddHisto("118_PX", 50,-1000.0,1000.0);
  Manager()->AddHisto("119_PY", 50,-1000.0,1000.0);
  Manager()->AddHisto("120_PZ", 50,-2000.0,2000.0);
  Manager()->AddHisto("121_R", 50,0.0,10.0);
  Manager()->AddHisto("122_Y", 50,-8.0,8.0);
  Manager()->AddHisto("123_BETA", 50,0.0,1.2);
  Manager()->AddHisto("124_GAMMA", 50,1.0,100.0);
  Manager()->AddHisto("125_PT", 50,0.0,1000.0);
  Manager()->AddHisto("126_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("127_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("128_E", 50,0.0,2000.0);
  Manager()->AddHisto("129_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("130_PT", 50,0.0,1000.0);
  Manager()->AddHisto("131_ETA", 50,-8.0,8.0);
  Manager()->AddHisto("132_PHI", 32,-3.2,3.2);
  Manager()->AddHisto("133_E", 50,0.0,2000.0);
  Manager()->AddHisto("134_MT_MET", 50,0.0,1000.0);
  Manager()->AddHisto("135_M", 50,0.0,3000.0);
  Manager()->AddHisto("136_PT", 50,0.0,3000.0);
  Manager()->AddHisto("137_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("138_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("139_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("140_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("141_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("142_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("143_M", 50,0.0,1000.0);
  Manager()->AddHisto("144_PT", 50,0.0,1500.0);
  Manager()->AddHisto("145_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("146_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("147_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("148_M", 50,0.0,1000.0);
  Manager()->AddHisto("149_PT", 50,0.0,1500.0);
  Manager()->AddHisto("150_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("151_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("152_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("153_M", 50,0.0,2000.0);
  Manager()->AddHisto("154_PT", 50,0.0,2000.0);
  Manager()->AddHisto("155_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("156_DPHI_0_PI", 32,0.0,3.2);
  Manager()->AddHisto("157_DPHI_0_2PI", 32,0.0,6.4);
  Manager()->AddHisto("158_M", 50,0.0,3000.0);
  Manager()->AddHisto("159_PT", 50,0.0,3000.0);
  Manager()->AddHisto("160_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("161_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("162_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("163_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("164_M", 50,0.0,1000.0);
  Manager()->AddHisto("165_M", 50,0.0,2000.0);
  Manager()->AddHisto("166_M", 50,0.0,2000.0);
  Manager()->AddHisto("167_M", 50,0.0,3000.0);
  Manager()->AddHisto("168_PT", 50,0.0,1500.0);
  Manager()->AddHisto("169_PT", 50,0.0,3000.0);
  Manager()->AddHisto("170_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("171_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("172_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("173_DELTAR", 50,0.0,10.0);
  Manager()->AddHisto("174_M", 50,0.0,1000.0);
  Manager()->AddHisto("175_M", 50,0.0,2000.0);
  Manager()->AddHisto("176_M", 50,0.0,2000.0);
  Manager()->AddHisto("177_M", 50,0.0,3000.0);
  Manager()->AddHisto("178_PT", 50,0.0,1500.0);
  Manager()->AddHisto("179_PT", 50,0.0,3000.0);

  // No problem during initialization
  return true;
}

MAbool user::Execute(SampleFormat& sample, const EventFormat& event)
{
  MAfloat32 __event_weight__ = 1.0;
  if (weighted_events_ && event.mc()!=0) __event_weight__ = event.mc()->weight();

  if (sample.mc()!=0) sample.mc()->addWeightedEvents(__event_weight__);
  Manager()->InitializeForNewEvent(__event_weight__);

  // Clearing particle containers
  {
      _P_e_I2I_PTorderingfinalstate_REG_.clear();
      _P_mu_I2I_PTorderingfinalstate_REG_.clear();
      _P_b_I2I_PTorderingfinalstate_REG_.clear();
      _P_j_I2I_PTorderingfinalstate_REG_.clear();
      _P_e_I1I_PTorderingfinalstate_REG_.clear();
      _P_mu_I1I_PTorderingfinalstate_REG_.clear();
      _P_b_I1I_PTorderingfinalstate_REG_.clear();
      _P_j_I1I_PTorderingfinalstate_REG_.clear();
      _P_ePTorderingfinalstate_REG_.clear();
      _P_muPTorderingfinalstate_REG_.clear();
      _P_bPTorderingfinalstate_REG_.clear();
      _P_jPTorderingfinalstate_REG_.clear();
  }

  // Filling particle containers
  {
    for (MAuint32 i=0;i<event.mc()->particles().size();i++)
    {
      if (isP__ePTorderingfinalstate((&(event.mc()->particles()[i])))) _P_ePTorderingfinalstate_REG_.push_back(&(event.mc()->particles()[i]));
      if (isP__muPTorderingfinalstate((&(event.mc()->particles()[i])))) _P_muPTorderingfinalstate_REG_.push_back(&(event.mc()->particles()[i]));
      if (isP__bPTorderingfinalstate((&(event.mc()->particles()[i])))) _P_bPTorderingfinalstate_REG_.push_back(&(event.mc()->particles()[i]));
      if (isP__jPTorderingfinalstate((&(event.mc()->particles()[i])))) _P_jPTorderingfinalstate_REG_.push_back(&(event.mc()->particles()[i]));
    }
  }

  // Sorting particles
  // Sorting particle collection according to PTordering
  // for getting 2th particle
  _P_e_I2I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_ePTorderingfinalstate_REG_,2,PTordering);

  // Sorting particle collection according to PTordering
  // for getting 2th particle
  _P_mu_I2I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_muPTorderingfinalstate_REG_,2,PTordering);

  // Sorting particle collection according to PTordering
  // for getting 2th particle
  _P_b_I2I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_bPTorderingfinalstate_REG_,2,PTordering);

  // Sorting particle collection according to PTordering
  // for getting 2th particle
  _P_j_I2I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_jPTorderingfinalstate_REG_,2,PTordering);

  // Sorting particle collection according to PTordering
  // for getting 1th particle
  _P_e_I1I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_ePTorderingfinalstate_REG_,1,PTordering);

  // Sorting particle collection according to PTordering
  // for getting 1th particle
  _P_mu_I1I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_muPTorderingfinalstate_REG_,1,PTordering);

  // Sorting particle collection according to PTordering
  // for getting 1th particle
  _P_b_I1I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_bPTorderingfinalstate_REG_,1,PTordering);

  // Sorting particle collection according to PTordering
  // for getting 1th particle
  _P_j_I1I_PTorderingfinalstate_REG_=SORTER->rankFilter(_P_jPTorderingfinalstate_REG_,1,PTordering);

  // Histogram number 1
  //   * Plot: SQRTS
  {
    Manager()->FillHisto("1_SQRTS", PHYSICS->SqrtS(event.mc()));
  }

  // Histogram number 2
  //   * Plot: SCALE
  {
    Manager()->FillHisto("2_SCALE", event.mc()->scale());
  }

  // Histogram number 3
  //   * Plot: ALPHA_QCD
  {
    Manager()->FillHisto("3_ALPHA_QCD", event.mc()->alphaQCD());
  }

  // Histogram number 4
  //   * Plot: ALPHA_QED
  {
    Manager()->FillHisto("4_ALPHA_QED", event.mc()->alphaQED());
  }

  // Histogram number 5
  //   * Plot: TET
  {
    Manager()->FillHisto("5_TET", PHYSICS->Transverse->EventTET(event.mc()));
  }

  // Histogram number 6
  //   * Plot: THT
  {
    Manager()->FillHisto("6_THT", PHYSICS->Transverse->EventTHT(event.mc()));
  }

  // Histogram number 7
  //   * Plot: MET
  {
    Manager()->FillHisto("7_MET", PHYSICS->Transverse->EventMET(event.mc()));
  }

  // Histogram number 8
  //   * Plot: MHT
  {
    Manager()->FillHisto("8_MHT", PHYSICS->Transverse->EventMHT(event.mc()));
  }

  // Histogram number 9
  //   * Plot: NPID
  {
  for (unsigned int i=0;i<event.mc()->particles().size();i++)
  {
    if (!PHYSICS->Id->IsFinalState(event.mc()->particles()[i])) continue;
    Manager()->FillHisto("9_NPID", event.mc()->particles()[i].pdgid());
  }
  }

  // Histogram number 10
  //   * Plot: NAPID
  {
  for (unsigned int i=0;i<event.mc()->particles().size();i++)
  {
    if (!PHYSICS->Id->IsFinalState(event.mc()->particles()[i])) continue;
    Manager()->FillHisto("10_NAPID", std::abs(event.mc()->particles()[i].pdgid()));
  }
  }

  // Histogram number 11
  //   * Plot: WEIGHTS
  {
    Manager()->FillHisto("11_WEIGHTS", PHYSICS->weights(event.mc()));
  }

  // Histogram number 12
  //   * Plot: PT ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("12_PT", _P_ePTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 13
  //   * Plot: ETA ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("13_ETA", _P_ePTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 14
  //   * Plot: ABSETA ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("14_ABSETA", _P_ePTorderingfinalstate_REG_[ind[0]]->abseta());
    }
  }
  }

  // Histogram number 15
  //   * Plot: PHI ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("15_PHI", _P_ePTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 16
  //   * Plot: E ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("16_E", _P_ePTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 17
  //   * Plot: ET ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("17_ET", _P_ePTorderingfinalstate_REG_[ind[0]]->et());
    }
  }
  }

  // Histogram number 18
  //   * Plot: M ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("18_M", _P_ePTorderingfinalstate_REG_[ind[0]]->m());
    }
  }
  }

  // Histogram number 19
  //   * Plot: MT ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("19_MT", _P_ePTorderingfinalstate_REG_[ind[0]]->mt());
    }
  }
  }

  // Histogram number 20
  //   * Plot: MT_MET ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("20_MT_MET", _P_ePTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 21
  //   * Plot: P ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("21_P", _P_ePTorderingfinalstate_REG_[ind[0]]->p());
    }
  }
  }

  // Histogram number 22
  //   * Plot: PX ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("22_PX", _P_ePTorderingfinalstate_REG_[ind[0]]->px());
    }
  }
  }

  // Histogram number 23
  //   * Plot: PY ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("23_PY", _P_ePTorderingfinalstate_REG_[ind[0]]->py());
    }
  }
  }

  // Histogram number 24
  //   * Plot: PZ ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("24_PZ", _P_ePTorderingfinalstate_REG_[ind[0]]->pz());
    }
  }
  }

  // Histogram number 25
  //   * Plot: R ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("25_R", _P_ePTorderingfinalstate_REG_[ind[0]]->r());
    }
  }
  }

  // Histogram number 26
  //   * Plot: Y ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("26_Y", _P_ePTorderingfinalstate_REG_[ind[0]]->y());
    }
  }
  }

  // Histogram number 27
  //   * Plot: BETA ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("27_BETA", _P_ePTorderingfinalstate_REG_[ind[0]]->beta());
    }
  }
  }

  // Histogram number 28
  //   * Plot: GAMMA ( e ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("28_GAMMA", _P_ePTorderingfinalstate_REG_[ind[0]]->gamma());
    }
  }
  }

  // Histogram number 29
  //   * Plot: PT ( e[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("29_PT", _P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 30
  //   * Plot: ETA ( e[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("30_ETA", _P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 31
  //   * Plot: PHI ( e[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("31_PHI", _P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 32
  //   * Plot: E ( e[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("32_E", _P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 33
  //   * Plot: MT_MET ( e[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("33_MT_MET", _P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 34
  //   * Plot: PT ( e[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("34_PT", _P_e_I2I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 35
  //   * Plot: ETA ( e[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("35_ETA", _P_e_I2I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 36
  //   * Plot: PHI ( e[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("36_PHI", _P_e_I2I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 37
  //   * Plot: E ( e[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("37_E", _P_e_I2I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 38
  //   * Plot: MT_MET ( e[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("38_MT_MET", _P_e_I2I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 39
  //   * Plot: M ( e[1] e[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_e_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_e_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_e_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_e_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_e_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_e_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("39_M", q.m());
    }
    }
  }
  }

  // Histogram number 40
  //   * Plot: PT ( e[1] e[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_e_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_e_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_e_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_e_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_e_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_e_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("40_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 41
  //   * Plot: DELTAR ( e[1] , e[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_e_I1I_PTorderingfinalstate_REG_[a[0]] == _P_e_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("41_DELTAR", _P_e_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_e_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 42
  //   * Plot: DPHI_0_PI ( e[1] , e[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_e_I1I_PTorderingfinalstate_REG_[a[0]] == _P_e_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("42_DPHI_0_PI", _P_e_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_e_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 43
  //   * Plot: DPHI_0_2PI ( e[1] , e[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_e_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_e_I1I_PTorderingfinalstate_REG_[a[0]] == _P_e_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("43_DPHI_0_2PI", _P_e_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_e_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 44
  //   * Plot: PT ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("44_PT", _P_muPTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 45
  //   * Plot: ETA ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("45_ETA", _P_muPTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 46
  //   * Plot: ABSETA ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("46_ABSETA", _P_muPTorderingfinalstate_REG_[ind[0]]->abseta());
    }
  }
  }

  // Histogram number 47
  //   * Plot: PHI ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("47_PHI", _P_muPTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 48
  //   * Plot: E ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("48_E", _P_muPTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 49
  //   * Plot: ET ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("49_ET", _P_muPTorderingfinalstate_REG_[ind[0]]->et());
    }
  }
  }

  // Histogram number 50
  //   * Plot: M ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("50_M", _P_muPTorderingfinalstate_REG_[ind[0]]->m());
    }
  }
  }

  // Histogram number 51
  //   * Plot: MT ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("51_MT", _P_muPTorderingfinalstate_REG_[ind[0]]->mt());
    }
  }
  }

  // Histogram number 52
  //   * Plot: MT_MET ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("52_MT_MET", _P_muPTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 53
  //   * Plot: P ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("53_P", _P_muPTorderingfinalstate_REG_[ind[0]]->p());
    }
  }
  }

  // Histogram number 54
  //   * Plot: PX ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("54_PX", _P_muPTorderingfinalstate_REG_[ind[0]]->px());
    }
  }
  }

  // Histogram number 55
  //   * Plot: PY ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("55_PY", _P_muPTorderingfinalstate_REG_[ind[0]]->py());
    }
  }
  }

  // Histogram number 56
  //   * Plot: PZ ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("56_PZ", _P_muPTorderingfinalstate_REG_[ind[0]]->pz());
    }
  }
  }

  // Histogram number 57
  //   * Plot: R ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("57_R", _P_muPTorderingfinalstate_REG_[ind[0]]->r());
    }
  }
  }

  // Histogram number 58
  //   * Plot: Y ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("58_Y", _P_muPTorderingfinalstate_REG_[ind[0]]->y());
    }
  }
  }

  // Histogram number 59
  //   * Plot: BETA ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("59_BETA", _P_muPTorderingfinalstate_REG_[ind[0]]->beta());
    }
  }
  }

  // Histogram number 60
  //   * Plot: GAMMA ( mu ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("60_GAMMA", _P_muPTorderingfinalstate_REG_[ind[0]]->gamma());
    }
  }
  }

  // Histogram number 61
  //   * Plot: PT ( mu[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("61_PT", _P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 62
  //   * Plot: ETA ( mu[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("62_ETA", _P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 63
  //   * Plot: PHI ( mu[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("63_PHI", _P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 64
  //   * Plot: E ( mu[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("64_E", _P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 65
  //   * Plot: MT_MET ( mu[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("65_MT_MET", _P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 66
  //   * Plot: PT ( mu[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("66_PT", _P_mu_I2I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 67
  //   * Plot: ETA ( mu[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("67_ETA", _P_mu_I2I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 68
  //   * Plot: PHI ( mu[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("68_PHI", _P_mu_I2I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 69
  //   * Plot: E ( mu[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("69_E", _P_mu_I2I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 70
  //   * Plot: MT_MET ( mu[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("70_MT_MET", _P_mu_I2I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 71
  //   * Plot: M ( mu[1] mu[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_mu_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_mu_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_mu_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_mu_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_mu_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("71_M", q.m());
    }
    }
  }
  }

  // Histogram number 72
  //   * Plot: PT ( mu[1] mu[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_mu_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_mu_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_mu_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_mu_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_mu_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_mu_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("72_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 73
  //   * Plot: DELTAR ( mu[1] , mu[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_mu_I1I_PTorderingfinalstate_REG_[a[0]] == _P_mu_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("73_DELTAR", _P_mu_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_mu_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 74
  //   * Plot: DPHI_0_PI ( mu[1] , mu[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_mu_I1I_PTorderingfinalstate_REG_[a[0]] == _P_mu_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("74_DPHI_0_PI", _P_mu_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_mu_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 75
  //   * Plot: DPHI_0_2PI ( mu[1] , mu[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_mu_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_mu_I1I_PTorderingfinalstate_REG_[a[0]] == _P_mu_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("75_DPHI_0_2PI", _P_mu_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_mu_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 76
  //   * Plot: PT ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("76_PT", _P_bPTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 77
  //   * Plot: ETA ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("77_ETA", _P_bPTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 78
  //   * Plot: ABSETA ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("78_ABSETA", _P_bPTorderingfinalstate_REG_[ind[0]]->abseta());
    }
  }
  }

  // Histogram number 79
  //   * Plot: PHI ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("79_PHI", _P_bPTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 80
  //   * Plot: E ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("80_E", _P_bPTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 81
  //   * Plot: ET ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("81_ET", _P_bPTorderingfinalstate_REG_[ind[0]]->et());
    }
  }
  }

  // Histogram number 82
  //   * Plot: M ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("82_M", _P_bPTorderingfinalstate_REG_[ind[0]]->m());
    }
  }
  }

  // Histogram number 83
  //   * Plot: MT ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("83_MT", _P_bPTorderingfinalstate_REG_[ind[0]]->mt());
    }
  }
  }

  // Histogram number 84
  //   * Plot: MT_MET ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("84_MT_MET", _P_bPTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 85
  //   * Plot: P ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("85_P", _P_bPTorderingfinalstate_REG_[ind[0]]->p());
    }
  }
  }

  // Histogram number 86
  //   * Plot: PX ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("86_PX", _P_bPTorderingfinalstate_REG_[ind[0]]->px());
    }
  }
  }

  // Histogram number 87
  //   * Plot: PY ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("87_PY", _P_bPTorderingfinalstate_REG_[ind[0]]->py());
    }
  }
  }

  // Histogram number 88
  //   * Plot: PZ ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("88_PZ", _P_bPTorderingfinalstate_REG_[ind[0]]->pz());
    }
  }
  }

  // Histogram number 89
  //   * Plot: R ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("89_R", _P_bPTorderingfinalstate_REG_[ind[0]]->r());
    }
  }
  }

  // Histogram number 90
  //   * Plot: Y ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("90_Y", _P_bPTorderingfinalstate_REG_[ind[0]]->y());
    }
  }
  }

  // Histogram number 91
  //   * Plot: BETA ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("91_BETA", _P_bPTorderingfinalstate_REG_[ind[0]]->beta());
    }
  }
  }

  // Histogram number 92
  //   * Plot: GAMMA ( b ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("92_GAMMA", _P_bPTorderingfinalstate_REG_[ind[0]]->gamma());
    }
  }
  }

  // Histogram number 93
  //   * Plot: PT ( b[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("93_PT", _P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 94
  //   * Plot: ETA ( b[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("94_ETA", _P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 95
  //   * Plot: PHI ( b[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("95_PHI", _P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 96
  //   * Plot: E ( b[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("96_E", _P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 97
  //   * Plot: MT_MET ( b[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("97_MT_MET", _P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 98
  //   * Plot: PT ( b[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("98_PT", _P_b_I2I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 99
  //   * Plot: ETA ( b[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("99_ETA", _P_b_I2I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 100
  //   * Plot: PHI ( b[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("100_PHI", _P_b_I2I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 101
  //   * Plot: E ( b[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("101_E", _P_b_I2I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 102
  //   * Plot: MT_MET ( b[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("102_MT_MET", _P_b_I2I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 103
  //   * Plot: M ( b[1] b[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_b_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_b_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_b_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_b_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_b_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_b_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("103_M", q.m());
    }
    }
  }
  }

  // Histogram number 104
  //   * Plot: PT ( b[1] b[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_b_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_b_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_b_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_b_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_b_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_b_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("104_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 105
  //   * Plot: DELTAR ( b[1] , b[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_b_I1I_PTorderingfinalstate_REG_[a[0]] == _P_b_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("105_DELTAR", _P_b_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_b_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 106
  //   * Plot: DPHI_0_PI ( b[1] , b[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_b_I1I_PTorderingfinalstate_REG_[a[0]] == _P_b_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("106_DPHI_0_PI", _P_b_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_b_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 107
  //   * Plot: DPHI_0_2PI ( b[1] , b[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_b_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_b_I1I_PTorderingfinalstate_REG_[a[0]] == _P_b_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("107_DPHI_0_2PI", _P_b_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_b_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 108
  //   * Plot: PT ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("108_PT", _P_jPTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 109
  //   * Plot: ETA ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("109_ETA", _P_jPTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 110
  //   * Plot: ABSETA ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("110_ABSETA", _P_jPTorderingfinalstate_REG_[ind[0]]->abseta());
    }
  }
  }

  // Histogram number 111
  //   * Plot: PHI ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("111_PHI", _P_jPTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 112
  //   * Plot: E ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("112_E", _P_jPTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 113
  //   * Plot: ET ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("113_ET", _P_jPTorderingfinalstate_REG_[ind[0]]->et());
    }
  }
  }

  // Histogram number 114
  //   * Plot: M ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("114_M", _P_jPTorderingfinalstate_REG_[ind[0]]->m());
    }
  }
  }

  // Histogram number 115
  //   * Plot: MT ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("115_MT", _P_jPTorderingfinalstate_REG_[ind[0]]->mt());
    }
  }
  }

  // Histogram number 116
  //   * Plot: MT_MET ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("116_MT_MET", _P_jPTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 117
  //   * Plot: P ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("117_P", _P_jPTorderingfinalstate_REG_[ind[0]]->p());
    }
  }
  }

  // Histogram number 118
  //   * Plot: PX ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("118_PX", _P_jPTorderingfinalstate_REG_[ind[0]]->px());
    }
  }
  }

  // Histogram number 119
  //   * Plot: PY ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("119_PY", _P_jPTorderingfinalstate_REG_[ind[0]]->py());
    }
  }
  }

  // Histogram number 120
  //   * Plot: PZ ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("120_PZ", _P_jPTorderingfinalstate_REG_[ind[0]]->pz());
    }
  }
  }

  // Histogram number 121
  //   * Plot: R ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("121_R", _P_jPTorderingfinalstate_REG_[ind[0]]->r());
    }
  }
  }

  // Histogram number 122
  //   * Plot: Y ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("122_Y", _P_jPTorderingfinalstate_REG_[ind[0]]->y());
    }
  }
  }

  // Histogram number 123
  //   * Plot: BETA ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("123_BETA", _P_jPTorderingfinalstate_REG_[ind[0]]->beta());
    }
  }
  }

  // Histogram number 124
  //   * Plot: GAMMA ( j ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("124_GAMMA", _P_jPTorderingfinalstate_REG_[ind[0]]->gamma());
    }
  }
  }

  // Histogram number 125
  //   * Plot: PT ( j[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("125_PT", _P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 126
  //   * Plot: ETA ( j[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("126_ETA", _P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 127
  //   * Plot: PHI ( j[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("127_PHI", _P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 128
  //   * Plot: E ( j[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("128_E", _P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 129
  //   * Plot: MT_MET ( j[1] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("129_MT_MET", _P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 130
  //   * Plot: PT ( j[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("130_PT", _P_j_I2I_PTorderingfinalstate_REG_[ind[0]]->pt());
    }
  }
  }

  // Histogram number 131
  //   * Plot: ETA ( j[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("131_ETA", _P_j_I2I_PTorderingfinalstate_REG_[ind[0]]->eta());
    }
  }
  }

  // Histogram number 132
  //   * Plot: PHI ( j[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("132_PHI", _P_j_I2I_PTorderingfinalstate_REG_[ind[0]]->phi());
    }
  }
  }

  // Histogram number 133
  //   * Plot: E ( j[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("133_E", _P_j_I2I_PTorderingfinalstate_REG_[ind[0]]->e());
    }
  }
  }

  // Histogram number 134
  //   * Plot: MT_MET ( j[2] ) 
  {
  {
    MAuint32 ind[1];
    for (ind[0]=0;ind[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
      Manager()->FillHisto("134_MT_MET", _P_j_I2I_PTorderingfinalstate_REG_[ind[0]]->mt_met(event.mc()->MET().momentum()));
    }
  }
  }

  // Histogram number 135
  //   * Plot: M ( j[1] j[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_j_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_j_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_j_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_j_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_j_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_j_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("135_M", q.m());
    }
    }
  }
  }

  // Histogram number 136
  //   * Plot: PT ( j[1] j[2] ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_j_I2I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_j_I2I_PTorderingfinalstate_REG_[ind[1]]==_P_j_I1I_PTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_j_I1I_PTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_j_I2I_PTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_j_I2I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("136_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 137
  //   * Plot: DELTAR ( j[1] , j[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_j_I1I_PTorderingfinalstate_REG_[a[0]] == _P_j_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("137_DELTAR", _P_j_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_j_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 138
  //   * Plot: DPHI_0_PI ( j[1] , j[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_j_I1I_PTorderingfinalstate_REG_[a[0]] == _P_j_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("138_DPHI_0_PI", _P_j_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_j_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 139
  //   * Plot: DPHI_0_2PI ( j[1] , j[2] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_j_I2I_PTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_j_I1I_PTorderingfinalstate_REG_[a[0]] == _P_j_I2I_PTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("139_DPHI_0_2PI", _P_j_I1I_PTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_j_I2I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 140
  //   * Plot: DELTAR ( e , e ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_ePTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_ePTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_ePTorderingfinalstate_REG_[a[0]] == _P_ePTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("140_DELTAR", _P_ePTorderingfinalstate_REG_[a[0]]->dr(_P_ePTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 141
  //   * Plot: DPHI_0_PI ( e , e ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_ePTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_ePTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_ePTorderingfinalstate_REG_[a[0]] == _P_ePTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("141_DPHI_0_PI", _P_ePTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_ePTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 142
  //   * Plot: DPHI_0_2PI ( e , e ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_ePTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_ePTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_ePTorderingfinalstate_REG_[a[0]] == _P_ePTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("142_DPHI_0_2PI", _P_ePTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_ePTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 143
  //   * Plot: M ( e e ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_ePTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_ePTorderingfinalstate_REG_[ind[1]]==_P_ePTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_ePTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_ePTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_ePTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_ePTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("143_M", q.m());
    }
    }
  }
  }

  // Histogram number 144
  //   * Plot: PT ( e e ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_ePTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_ePTorderingfinalstate_REG_[ind[1]]==_P_ePTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_ePTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_ePTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_ePTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_ePTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("144_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 145
  //   * Plot: DELTAR ( mu , mu ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_muPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_muPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_muPTorderingfinalstate_REG_[a[0]] == _P_muPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("145_DELTAR", _P_muPTorderingfinalstate_REG_[a[0]]->dr(_P_muPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 146
  //   * Plot: DPHI_0_PI ( mu , mu ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_muPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_muPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_muPTorderingfinalstate_REG_[a[0]] == _P_muPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("146_DPHI_0_PI", _P_muPTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_muPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 147
  //   * Plot: DPHI_0_2PI ( mu , mu ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_muPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_muPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_muPTorderingfinalstate_REG_[a[0]] == _P_muPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("147_DPHI_0_2PI", _P_muPTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_muPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 148
  //   * Plot: M ( mu mu ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_muPTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_muPTorderingfinalstate_REG_[ind[1]]==_P_muPTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_muPTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_muPTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_muPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_muPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("148_M", q.m());
    }
    }
  }
  }

  // Histogram number 149
  //   * Plot: PT ( mu mu ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_muPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_muPTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_muPTorderingfinalstate_REG_[ind[1]]==_P_muPTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_muPTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_muPTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_muPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_muPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("149_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 150
  //   * Plot: DELTAR ( b , b ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_bPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_bPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_bPTorderingfinalstate_REG_[a[0]] == _P_bPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("150_DELTAR", _P_bPTorderingfinalstate_REG_[a[0]]->dr(_P_bPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 151
  //   * Plot: DPHI_0_PI ( b , b ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_bPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_bPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_bPTorderingfinalstate_REG_[a[0]] == _P_bPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("151_DPHI_0_PI", _P_bPTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_bPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 152
  //   * Plot: DPHI_0_2PI ( b , b ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_bPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_bPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_bPTorderingfinalstate_REG_[a[0]] == _P_bPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("152_DPHI_0_2PI", _P_bPTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_bPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 153
  //   * Plot: M ( b b ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_bPTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_bPTorderingfinalstate_REG_[ind[1]]==_P_bPTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_bPTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_bPTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_bPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_bPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("153_M", q.m());
    }
    }
  }
  }

  // Histogram number 154
  //   * Plot: PT ( b b ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_bPTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_bPTorderingfinalstate_REG_[ind[1]]==_P_bPTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_bPTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_bPTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_bPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_bPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("154_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 155
  //   * Plot: DELTAR ( j , j ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_jPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_jPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_jPTorderingfinalstate_REG_[a[0]] == _P_jPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("155_DELTAR", _P_jPTorderingfinalstate_REG_[a[0]]->dr(_P_jPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 156
  //   * Plot: DPHI_0_PI ( j , j ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_jPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_jPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_jPTorderingfinalstate_REG_[a[0]] == _P_jPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("156_DPHI_0_PI", _P_jPTorderingfinalstate_REG_[a[0]]->dphi_0_pi(_P_jPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 157
  //   * Plot: DPHI_0_2PI ( j , j ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_jPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_jPTorderingfinalstate_REG_.size();b[0]++)
    {
     if ( _P_jPTorderingfinalstate_REG_[a[0]] == _P_jPTorderingfinalstate_REG_[b[0]] ) continue;
      Manager()->FillHisto("157_DPHI_0_2PI", _P_jPTorderingfinalstate_REG_[a[0]]->dphi_0_2pi(_P_jPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 158
  //   * Plot: M ( j j ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_jPTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_jPTorderingfinalstate_REG_[ind[1]]==_P_jPTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_jPTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_jPTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_jPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_jPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("158_M", q.m());
    }
    }
  }
  }

  // Histogram number 159
  //   * Plot: PT ( j j ) 
  {
  {
    MAuint32 ind[2];
    std::vector<std::set<const MCParticleFormat*> > combis;
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_jPTorderingfinalstate_REG_.size();ind[1]++)
    {
    if (_P_jPTorderingfinalstate_REG_[ind[1]]==_P_jPTorderingfinalstate_REG_[ind[0]]) continue;

    // Checking if consistent combination
    std::set<const MCParticleFormat*> mycombi;
    for (MAuint32 i=0;i<2;i++)
    {
      mycombi.insert(_P_jPTorderingfinalstate_REG_[ind[i]]);
      mycombi.insert(_P_jPTorderingfinalstate_REG_[ind[i]]);
    }
    MAbool matched=false;
    for (MAuint32 i=0;i<combis.size();i++)
      if (combis[i]==mycombi) {matched=true; break;}
    if (matched) continue;
    else combis.push_back(mycombi);

    ParticleBaseFormat q;
    q+=_P_jPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_jPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("159_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 160
  //   * Plot: DELTAR ( e , mu ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_ePTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_muPTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("160_DELTAR", _P_ePTorderingfinalstate_REG_[a[0]]->dr(_P_muPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 161
  //   * Plot: DELTAR ( e , j ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_ePTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_jPTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("161_DELTAR", _P_ePTorderingfinalstate_REG_[a[0]]->dr(_P_jPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 162
  //   * Plot: DELTAR ( mu , j ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_muPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_jPTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("162_DELTAR", _P_muPTorderingfinalstate_REG_[a[0]]->dr(_P_jPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 163
  //   * Plot: DELTAR ( b , j ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_bPTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_jPTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("163_DELTAR", _P_bPTorderingfinalstate_REG_[a[0]]->dr(_P_jPTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 164
  //   * Plot: M ( e mu ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_muPTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_ePTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_muPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("164_M", q.m());
    }
    }
  }
  }

  // Histogram number 165
  //   * Plot: M ( e j ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_jPTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_ePTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_jPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("165_M", q.m());
    }
    }
  }
  }

  // Histogram number 166
  //   * Plot: M ( j mu ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_jPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_muPTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_jPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_muPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("166_M", q.m());
    }
    }
  }
  }

  // Histogram number 167
  //   * Plot: M ( b j ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_jPTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_bPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_jPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("167_M", q.m());
    }
    }
  }
  }

  // Histogram number 168
  //   * Plot: PT ( e mu ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_ePTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_muPTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_ePTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_muPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("168_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 169
  //   * Plot: PT ( b j ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_bPTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_jPTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_bPTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_jPTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("169_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 170
  //   * Plot: DELTAR ( e[1] , mu[1] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("170_DELTAR", _P_e_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_mu_I1I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 171
  //   * Plot: DELTAR ( e[1] , j[1] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("171_DELTAR", _P_e_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_j_I1I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 172
  //   * Plot: DELTAR ( mu[1] , j[1] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_mu_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("172_DELTAR", _P_mu_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_j_I1I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 173
  //   * Plot: DELTAR ( b[1] , j[1] ) 
  {
  {
    MAuint32 a[1];
    for (a[0]=0;a[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();a[0]++)
    {
    MAuint32 b[1];
    for (b[0]=0;b[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();b[0]++)
    {
      Manager()->FillHisto("173_DELTAR", _P_b_I1I_PTorderingfinalstate_REG_[a[0]]->dr(_P_j_I1I_PTorderingfinalstate_REG_[b[0]]));
    }
    }
  }
  }

  // Histogram number 174
  //   * Plot: M ( e[1] mu[1] ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_mu_I1I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("174_M", q.m());
    }
    }
  }
  }

  // Histogram number 175
  //   * Plot: M ( e[1] j[1] ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_j_I1I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("175_M", q.m());
    }
    }
  }
  }

  // Histogram number 176
  //   * Plot: M ( j[1] mu[1] ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_j_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_mu_I1I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("176_M", q.m());
    }
    }
  }
  }

  // Histogram number 177
  //   * Plot: M ( b[1] j[1] ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_j_I1I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("177_M", q.m());
    }
    }
  }
  }

  // Histogram number 178
  //   * Plot: PT ( e[1] mu[1] ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_e_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_mu_I1I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_e_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_mu_I1I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("178_PT", q.pt());
    }
    }
  }
  }

  // Histogram number 179
  //   * Plot: PT ( b[1] j[1] ) 
  {
  {
    MAuint32 ind[2];
    for (ind[0]=0;ind[0]<_P_b_I1I_PTorderingfinalstate_REG_.size();ind[0]++)
    {
    for (ind[1]=0;ind[1]<_P_j_I1I_PTorderingfinalstate_REG_.size();ind[1]++)
    {
    ParticleBaseFormat q;
    q+=_P_b_I1I_PTorderingfinalstate_REG_[ind[0]]->momentum();
    q+=_P_j_I1I_PTorderingfinalstate_REG_[ind[1]]->momentum();
      Manager()->FillHisto("179_PT", q.pt());
    }
    }
  }
  }

  return true;
}

void user::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
}
