#include "SampleAnalyzer/User/Analyzer/datastructure.h"
using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------
bool datastructure::Initialize(const MA5::Configuration& cfg,
                               const std::map<std::string,std::string>& parameters)
{
  // Initializing PhysicsService for MC
  PHYSICS->mcConfig().Reset();
  AddDefaultHadronic();
  AddDefaultInvisible();

  MAint32 test = getOption<MAint32>("test", 0);

  if (test == 0) {ERROR << "Commandline input has not been registered." << endmsg;}

  return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void datastructure::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files) {}

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool datastructure::Execute(SampleFormat& sample, const EventFormat& event)
{
    // Event weight
    MAdouble64 EvWeight;
    if(Configuration().IsNoEventWeight()) EvWeight=1.;
    else if(event.mc()->weight()!=0.) EvWeight=event.mc()->weight();
    else { return false;}
    Manager()->InitializeForNewEvent(EvWeight);
    // Empty event
    if (event.rec()==0) {return true;}

    std::vector<const RecLeptonFormat*> SignalElectrons = filter(event.rec()->electrons(), 10, 2.5);
    std::vector<const RecLeptonFormat*> SignalMuons     = filter(event.rec()->muons(),     10, 2.5);
    std::vector<const RecPhotonFormat*> SignalPhotons   = filter(event.rec()->photons(),   10, 2.5);
    std::vector<const RecTauFormat*> SignalTaus         = filter(event.rec()->taus(),      10, 2.5);

    std::vector<const RecJetFormat*> SignalJets = filter_select(event.rec()->jets(),
        [] (const RecJetFormat* jet) { return (jet->pt()>20. && jet->eta()<4.); });

    std::vector<const RecTrackFormat*> SignalTracks = filter_select(event.rec()->tracks(), \
        [] (const RecTrackFormat* trk) { return (trk->eta() > 2. && trk->eta()<4.); });

    // Object overlap removal
    SignalJets        = OverlapRemoval(SignalJets,      SignalElectrons, 0.2);
    SignalElectrons   = OverlapRemoval(SignalElectrons, SignalJets,      0.5);

    // Test attributes of each data structure
    if (SignalJets.size() > 0)
    {
        MAuint16 ntracks = SignalJets[0]->ntracks();
        MAbool btag = SignalJets[0]->btag();
        MAbool ctag = SignalJets[0]->ctag();
        MAbool true_ctag = SignalJets[0]->true_ctag();
        MAbool true_btag = SignalJets[0]->true_btag();
        MAint32 nconstit = SignalJets[0]->constituents().size();
//        SignalJets[0]->Print();
    }
    if (SignalElectrons.size() > 0)
    {
        MAint32 charge = SignalElectrons[0]->charge();
        MAfloat32 sumET_isol = SignalElectrons[0]->sumET_isol();
        MAfloat32 sumPT_isol = SignalElectrons[0]->sumPT_isol();
        MAfloat32 ET_PT_isol = SignalElectrons[0]->ET_PT_isol();
        MAuint64 refmc = SignalElectrons[0]->refmc();
        MAbool isElectron = SignalElectrons[0]->isElectron();
        MAbool isMuon = SignalElectrons[0]->isMuon();
        MAfloat32 d0error = SignalElectrons[0]->d0error();
        MAfloat32 dzerror = SignalElectrons[0]->dzerror();
//        SignalElectrons[0]->Print();
    }
    if (SignalTaus.size() > 0)
    {
        MAint32 charge = SignalTaus[0]->charge();
        MAuint16 ntracks = SignalTaus[0]->ntracks();
        MAint32 DecayMode = SignalTaus[0]->DecayMode();
    }
    if (SignalTracks.size() > 0)
    {
        MAint32 charge = SignalTracks[0]->charge();
        MAint32 pdgid = SignalTracks[0]->pdgid();
//        SignalTracks[0]->Print();
    }

    return true;
}
