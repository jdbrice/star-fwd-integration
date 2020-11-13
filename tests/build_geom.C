//usr/bin/env root4star -l -b -q  $0; exit $?
// that is a valid shebang to run script as executable


void build_geom( TString geomtag = "dev2021", TString output="fGeom.root" ) {
   gROOT->LoadMacro("bfc.C");
   bfc(0, "fzin agml sdt20181215", "" );

   gSystem->Load("libStarClassLibrary.so");
   gSystem->Load("libStEvent.so" );

   // Force build of the geometry
   TFile *geom = TFile::Open( output.Data() );

   if ( 0 == geom ) {
      AgModule::SetStacker( new StarTGeoStacker() );
      AgPosition::SetDebug(2);
      StarGeometry::Construct("dev2021");

      // Genfit requires the geometry is cached in a ROOT file
      gGeoManager->Export( output.Data() );
      cout << "Writing output to geometry file [" << output.Data() << "]" << endl;
   }
   else {
      cout << "WARNING:  Geometry file [" << output.Data() << "] already exists." << endl;
      cout << "Existting without doing anything!" << endl;
      delete geom;
   }

}