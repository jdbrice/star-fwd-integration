//usr/bin/env root4star -l -b -q  $0; exit $?
// that is a valid shebang to run script as executable

TFile *output = 0;

void fast_track(   int n = 100,
                  const char *inFile = "tests/sim.fzd",
                  std::string configFile = "tests/fast_track.xml",
                  const char *geom = "dev2021") {
    TString _geom = geom;

    bool SiIneff = false;

    TString _chain;

    // NOTE "event" does not work in CMAKE StRoot wo network, it includes detDb - root problem. Swap to StEvent instead
    _chain = Form("fzin %s StEvent evout geantout ReverseField agml usexgeom bigbig", _geom.Data());

    gROOT->LoadMacro("bfc.C");
    bfc(-1, _chain, inFile);

    // StarMagField::setConstBz(true);

    gSystem->Load("libMathMore.so");
    gSystem->Load("libStFstSimMaker.so");
    gSystem->Load("libStFttSimMaker.so");

    gSystem->Load("libgenfit2.so");
    gSystem->Load("libKiTrack.so");
    gSystem->Load("libStEventUtilities.so");
    gSystem->Load("libStFwdTrackMaker.so");

    StFttFastSimMaker *fttFastSim = new StFttFastSimMaker();
    cout << "Adding StFttFastSimMaker to chain" << endl;
    chain->AddMaker(fttFastSim);

    TString qaoutname(gSystem->BaseName(inFile));
    qaoutname.ReplaceAll(".fzd", ".FastSimu.QA.root");

    // Create fast simulator and add after event maker
    StFstFastSimMaker *fstFastSim = new StFstFastSimMaker();

    if (SiIneff)
        fstFastSim->setInEfficiency(0.1); // inefficiency of Si 

    fstFastSim->setQAFileName(qaoutname);

    cout << "Adding StFstFastSimMaker to chain" << endl;
    chain->AddMaker(fstFastSim);

    StFwdTrackMaker *gmk = new StFwdTrackMaker();
    // config file set here overides chain opt
    gmk->SetConfigFile( configFile );
    gmk->GenerateTree( false );
    // chain->AddAfter( "fsiSim", gmk );
    chain->AddMaker(gmk);

    chain->Init();

    //_____________________________________________________________________________
    //
    // MAIN EVENT LOOP
    //_____________________________________________________________________________
    for (int i = 0; i < n; i++) {

        chain->Clear();
        if (kStOK != chain->Make())
            break;
    }

}
