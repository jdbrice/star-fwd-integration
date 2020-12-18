// macro to instantiate the Geant3 from within
// STAR  C++  framework and get the starsim prompt
// To use it do
//  root4star starsim.C

class St_geant_Maker;
St_geant_Maker *geant_maker = 0;

class StarGenEvent;
StarGenEvent   *event       = 0;

class StarPrimaryMaker;
StarPrimaryMaker *_primary = 0;


class StarParticleFilter;
StarParticleFilter *_filter = 0;

// ----------------------------------------------------------------------------
void geometry( TString tag, Bool_t agml=true )
{
  TString cmd = "DETP GEOM "; cmd += tag;
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> LoadGeometry(cmd);
  //  if ( agml ) command("gexec $STAR_LIB/libxgeometry.so");
}
// ----------------------------------------------------------------------------
void command( TString cmd )
{
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> Do( cmd );
}
// ----------------------------------------------------------------------------
// trig()  -- generates one event
// trig(n) -- generates n+1 events.
//
// NOTE:  last event generated will be corrupt in the FZD file
//
void trig( int n=1 )
{
  chain->Clear();
  chain->Make();

  // vertex_table = dynamic_cast<TTable*>( chain->GetDataSet("g2t_vertex")  ) ;
  // track_table  = dynamic_cast<TTable*>( chain->GetDataSet("g2t_track")   ) ;
  // hit_table    = dynamic_cast<TTable*>( chain->GetDataSet("g2t_stg_hit") ) ;  

  // if ( track_table ) {
  //   int nrows = track_table->GetNRows();
  //   track_table->Print(0,nrows);
  // }

  // if ( hit_table ) {
  //   int nrows = hit_table->GetNRows();
  //   hit_table->Print(0, nrows );
  // }

  
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Pythia8( TString config="pp:W", double energy=510.0 )
{

  //
  // Create the pythia 8 event generator and add it to 
  // the primary generator
  //
  StarPythia8 *pythia8 = new StarPythia8();    

  // See the following page for configuring electroweak processes (eg W production, Drell Yan, etc...)
  // http://home.thep.lu.se/~torbjorn/pythia82html/ElectroweakProcesses.html

  if ( config=="pp:W" )    {
      pythia8->SetFrame("CMS", energy);
      pythia8->SetBlue("proton");
      pythia8->SetYell("proton");
      
      pythia8->Set("WeakSingleBoson:all=off");
      pythia8->Set("WeakSingleBoson:ffbar2W=on");
      pythia8->Set("24:onMode=0");              // switch off all W+/- decays
      pythia8->Set("24:onIfAny 11 -11");        // switch on for decays to e+/-
      
    }
  if ( config=="pp:minbias" )    {
      pythia8->SetFrame("CMS", 510.0);
      pythia8->SetBlue("proton");
      pythia8->SetYell("proton");            

      pythia8->Set("SoftQCD:minBias = on");
    }
  if ( config=="pp:DrellYan" )     {
      pythia8->SetFrame("CMS", energy);
      pythia8->SetBlue("proton");
      pythia8->SetYell("proton");
      
      pythia8->Set("WeakSingleBoson:all=off");
      pythia8->Set("WeakSingleBoson:ffbar2gmZ=on");
      pythia8->Set("23:onMode=0");              // switch off all Z/gamma decays
      pythia8->Set("23:onIfAny 11 -11");        // switch on for decays to e+/-
      pythia8->Set("WeakZ0:gmZmode = 0");       // full gamma+Z structure w/ interference
           
    }
  if ( config=="pp:DrellYan:mode1" )     {
      pythia8->SetFrame("CMS", energy);
      pythia8->SetBlue("proton");
      pythia8->SetYell("proton");
      
      pythia8->Set("WeakSingleBoson:all=off");
      pythia8->Set("WeakSingleBoson:ffbar2gmZ=on");
      pythia8->Set("23:onMode=0");              // switch off all Z/gamma decays
      pythia8->Set("23:onIfAny 11 -11");        // switch on for decays to e+/-
      pythia8->Set("WeakZ0:gmZmode = 1");       // pure gamma*
           
    }
  if ( config=="pp:DrellYan:mode2" )     {
      pythia8->SetFrame("CMS", energy);
      pythia8->SetBlue("proton");
      pythia8->SetYell("proton");
      
      pythia8->Set("WeakSingleBoson:all=off");
      pythia8->Set("WeakSingleBoson:ffbar2gmZ=on");
      pythia8->Set("23:onMode=0");              // switch off all Z/gamma decays
      pythia8->Set("23:onIfAny 11 -11");        // switch on for decays to e+/-
      pythia8->Set("WeakZ0:gmZmode = 2");       // pure Z0
           
    }

  // Phase space cuts
  // http://home.thep.lu.se/~torbjorn/pythia82html/PhaseSpaceCuts.html
  pythia8->Set("PhaseSpace:mHatMin=4.0"); // minimum invariant mass [pythia8 default = 4 GeV]
  

  _primary -> AddGenerator( pythia8 );
  
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void runStarsimPythia8( int nevents=10, const char* mode="pp:DrellYan", int rngSeed=1234 )
{ 

  gROOT->ProcessLine(".L bfc.C");
  {
    TString simple = "dev2021 geant gstar usexgeom agml ";
    bfc(0, simple );
  }

  gSystem->Load( "libVMC.so");

  gSystem->Load( "StarGeneratorUtil.so");
  gSystem->Load( "StarGeneratorEvent.so");
  gSystem->Load( "StarGeneratorBase.so" );
  gSystem->Load( "StarGeneratorFilt.so" );

  gSystem->Load( "Pythia8_1_62.so");

  gSystem->Load( "libMathMore.so"   );  

  // Force loading of xgeometry
  gSystem->Load( "xgeometry.so"     );
  gSystem->Load( "Pythia8_1_62.so"  );

  // Setup RNG seed and map all ROOT TRandom here
  StarRandom::seed( rngSeed );
  StarRandom::capture();
  
  //
  // Create the primary event generator and insert it
  // before the geant maker
  //
  //  StarPrimaryMaker *
  _primary = new StarPrimaryMaker();
  {
    _primary -> SetVertex( 0.1, -0.1, 0.0 );
    _primary -> SetSigma ( 0.1,  0.1, 30.0 );
    chain -> AddBefore( "geant", _primary );
  }

  //
  // Setup an event generator
  //
  Pythia8("pp:DrellYan");


  //
  // Setup the event generator filter (to increase number of
  // DY events in the acceptance of the detector, for instance...)
  //  ... specify pdgid of each required particle, along with min
  //      and max pT, min and max eta.  (ptmx<ptmn indicactes no
  //      upper limit).
  _filter = new StarParticleFilter();  _primary->AddFilter( _filter );
  //                    pdg  ptmn ptmx  etamn etamx parent pdgid
  _filter -> AddTrigger( +11, 1.4, -1.0, 2.25, 4.25,  23 );
  _filter -> AddTrigger( -11, 1.4, -1.0, 2.25, 4.25,  23 );

  _primary ->SetAttr( "FilterKeepAll", int(0) ); // Saves event header information in MC tree only (set =1 to keep all gen tracks)

  //
  // Setup cuts on which particles get passed to geant for
  //   simulation.  (To run generator in standalone mode,
  //   set ptmin=1.0E9.)
  //                    ptmin  ptmax
  _primary->SetPtRange  (0.0,  -1.0);         // GeV
  //                    etamin etamax
  _primary->SetEtaRange ( -2.0, +5.0 );
  //                    phimin phimax
  _primary->SetPhiRange ( 0., TMath::TwoPi() );
  
  
  // 
  // Setup a realistic z-vertex distribution:
  //   x = 0 gauss width = 1mm
  //   y = 0 gauss width = 1mm
  //   z = 0 gauss width = 30cm
  // 
  _primary->SetVertex( 0., 0., 0. );
  _primary->SetSigma( 0.1, 0.1, 30.0 );

  
  //
  // Initialize primary event generator and all sub makers
  //
  _primary -> Init();

  //
  // Setup geometry and set starsim to use agusread for input
  //
  //geometry("dev2021");
  //* AGUSER/GKINE NTRACK ID [ PTLOW PTHIGH YLOW YHIGH PHILOW PHIHIGH ZLOW ZHIGH option ]

  command("gkine -4 0");

  TString fzdname = Form("gfile o pythia8_%s_%i_%ievts.fzd",mode,rngSeed,nevents);
  fzdname.ReplaceAll("pp:","pp_");
  TString rooname = Form("pythia8_%s_%i_%ievts.genevents.root",mode,rngSeed,nevents);
  command( fzdname );
  _primary -> SetFileName( rooname );  

  //
  // Trigger on nevents
  //
  for ( int ev=0;ev<nevents;ev++ ) trig(1);

  //
  // Finish the chain
  //
  chain->Finish();

  //
  // EXIT starsim
  //
  command("call agexit");  // Make sure that STARSIM exits properly

}
// ----------------------------------------------------------------------------

