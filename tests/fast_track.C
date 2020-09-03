//usr/bin/env root4star -l -b -q  $0; exit $?
// that is a valid shebang to run script as executable

TFile *output = 0;

void fast_track(   int n = 100,
                  const char *inFile = "test/sim.fzd",
                  std::string configFile = "test/fast_track.xml",
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

    fstFastSim->setPointHits(); // X&Y combined to points 
    fstFastSim->setPixels(8, 12, 128); // set the digitization options, note these cannot actually be changed without code change.
    fstFastSim->setRaster(0.0); // raster between layers

    if (SiIneff)
        fstFastSim->setInEfficiency(0.1); // inefficiency of Si 

    fstFastSim->setQAFileName(qaoutname);

    cout << "Adding StFstFastSimMaker to chain" << endl;
    chain->AddMaker(fstFastSim);

    StFwdTrackMaker *gmk = new StFwdTrackMaker();
    gmk->SetConfigFile( configFile );
    gmk->GenerateTree( false );
    // chain->AddAfter( "fsiSim", gmk );
    chain->AddMaker(gmk);

    chain->Init();

    // Output filename setup
    // TString outname(gSystem->BaseName(inFile));
    // outname.ReplaceAll(".fzd", "");
    // output = TFile::Open(Form("%s_output.root", outname.Data()), "recreate");

    //_____________________________________________________________________________
    //
    // MAIN EVENT LOOP
    //_____________________________________________________________________________
    for (int i = 0; i < n; i++) {

        chain->Clear();
        if (kStOK != chain->Make())
            break;
    }

    // output->cd();
    // output->Close();
}
