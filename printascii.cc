// Monte Carlo .ROOT tree file to ascii (.csv)
// ------------------------------------------------------------------
//
// Run with: root printascii.cc+ -b -q
//
// mikael.mieskolainen@cern.ch, 26/07/2018


#include <vector>
#include "TFile.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TTree.h"

const double M = 0; // Particle mass (not needed here)

bool ProcessData(const TString& filename);


// ****************** FIDUCIAL DEFINITION ******************
// We cut events here for the DeepEfficiency training phase
// Note that these should be kept the same through whole
// algorithmic chain!
const double FID_PT  = 0.1; // track pt
const double FID_ETA = 0.9; // track eta
// *********************************************************


// Main function
int printascii(void){

  std::vector<TString> filenames;

  // Simulation sets

  // pi+pi-
  filenames.push_back("tree2track_kPipmExp");
  filenames.push_back("tree2track_kPipmOrexp");
  filenames.push_back("tree2track_kPipmPower");
  filenames.push_back("tree2track_kCohRhoToPi");

  // K+K-
  filenames.push_back("tree2track_kKpkmExp");
  filenames.push_back("tree2track_kKpkmOrexp");
  filenames.push_back("tree2track_kKpkmPower");
  
  // Process data
  for (uint i = 0; i < filenames.size(); ++i) {
    ProcessData(filenames.at(i));
  }
  
  return EXIT_SUCCESS;
}


// Processor
bool ProcessData(const TString& filename) {

  printf("Processing file: %s \n", filename.Data());

  TFile* f = new TFile("./rootdata/" + filename + ".root");

  // Tree definition
  TTree* tree2track = (TTree*) f->Get("tree2track");

  Int_t run    = 0;
  Float_t zVtx = 0;
  Float_t px1 = 0;
  Float_t py1 = 0;
  Float_t pz1 = 0;
  Float_t px2 = 0;
  Float_t py2 = 0;
  Float_t pz2 = 0;
  Float_t sigmaPiTPC1 = 0;
  Float_t sigmaKaTPC1 = 0;
  Float_t sigmaPrTPC1 = 0;
  Float_t sigmaPiTPC2 = 0;
  Float_t sigmaKaTPC2 = 0;
  Float_t sigmaPrTPC2 = 0;
  Float_t sigmaPiTOF1 = 0;
  Float_t sigmaKaTOF1 = 0;
  Float_t sigmaPrTOF1 = 0;
  Float_t sigmaPiTOF2 = 0;
  Float_t sigmaKaTOF2 = 0;
  Float_t sigmaPrTOF2 = 0;
  Float_t pxMc1 = 0;
  Float_t pyMc1 = 0;
  Float_t pzMc1 = 0;
  Float_t pxMc2 = 0;
  Float_t pyMc2 = 0;
  Float_t pzMc2 = 0;
  Int_t pidCode1 = 0;
  Int_t pidCode2 = 0;

  // Connect tree branches
  tree2track->SetBranchAddress("run", &run);
  tree2track->SetBranchAddress("pxMc1", &pxMc1);
  tree2track->SetBranchAddress("pyMc1", &pyMc1);
  tree2track->SetBranchAddress("pzMc1", &pzMc1);
  tree2track->SetBranchAddress("pxMc2", &pxMc2);
  tree2track->SetBranchAddress("pyMc2", &pyMc2);
  tree2track->SetBranchAddress("pzMc2", &pzMc2);
  tree2track->SetBranchAddress("pidCode1", &pidCode1);
  tree2track->SetBranchAddress("pidCode2", &pidCode2);
  tree2track->SetBranchAddress("zVtx", &zVtx);
  tree2track->SetBranchAddress("px1", &px1);
  tree2track->SetBranchAddress("py1", &py1);
  tree2track->SetBranchAddress("pz1", &pz1);
  tree2track->SetBranchAddress("px2", &px2);
  tree2track->SetBranchAddress("py2", &py2);
  tree2track->SetBranchAddress("pz2", &pz2);
  tree2track->SetBranchAddress("sigmaPiTPC1", &sigmaPiTPC1);
  tree2track->SetBranchAddress("sigmaKaTPC1", &sigmaKaTPC1);
  tree2track->SetBranchAddress("sigmaPrTPC1", &sigmaPrTPC1);
  tree2track->SetBranchAddress("sigmaPiTPC2", &sigmaPiTPC2);
  tree2track->SetBranchAddress("sigmaKaTPC2", &sigmaKaTPC2);
  tree2track->SetBranchAddress("sigmaPrTPC2", &sigmaPrTPC2);
  tree2track->SetBranchAddress("sigmaPiTOF1", &sigmaPiTOF1);
  tree2track->SetBranchAddress("sigmaKaTOF1", &sigmaKaTOF1);
  tree2track->SetBranchAddress("sigmaPrTOF1", &sigmaPrTOF1);
  tree2track->SetBranchAddress("sigmaPiTOF2", &sigmaPiTOF2);
  tree2track->SetBranchAddress("sigmaKaTOF2", &sigmaKaTOF2);
  tree2track->SetBranchAddress("sigmaPrTOF2", &sigmaPrTOF2);

  // Output file
  TString outputfile;
  outputfile = "./data/" + filename +  ".csv";
  FILE* asciif = fopen(outputfile.Data(), "w");
  if (asciif == NULL) {
    printf("Error opening output file!\n");
    return false;
  }
  
  // Loop over events
  for (uint k = 0; k < tree2track->GetEntries(); ++k) {

    tree2track->GetEntry(k);

    //printf("Event %d / %u \n", k, tree2track->GetEntries() );

    TLorentzVector p1_gen;
    TLorentzVector p2_gen;

    TLorentzVector p1_rec;
    TLorentzVector p2_rec;

    // Generated (only MC)
    p1_gen.SetXYZM(pxMc1,pyMc1,pzMc1,M);
    p2_gen.SetXYZM(pxMc2,pyMc2,pzMc2,M);

    // Reconstructed
    p1_rec.SetXYZM(px1,py1,pz1,M);
    p2_rec.SetXYZM(px2,py2,pz2,M);

    // -------------------------------------------------------------
    // *********** FIDUCIAL PHASE-SPACE DEFINITION CUTS ************
    if (p1_gen.Perp() > FID_PT && p2_gen.Perp() > FID_PT &&
        std::abs(p1_gen.Eta()) < FID_ETA && std::abs(p2_gen.Eta()) < FID_ETA ) {
      // Event within fiducial
    } else {
      continue; // Do not accept
    }
    // -------------------------------------------------------------

    // Did we reconstruct both?
    int reco = 0;
    if (p1_rec.Px() > -999 && p2_rec.Px() > - 999) {
       reco = 1;
    }

    fprintf(asciif, "%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%d,%d,%d\n",
      p1_gen.Px(),p1_gen.Py(),p1_gen.Pz(),
      p2_gen.Px(),p2_gen.Py(),p2_gen.Pz(),
      p1_rec.Px(),p1_rec.Py(),p1_rec.Pz(),
      p2_rec.Px(),p2_rec.Py(),p2_rec.Pz(),
      pidCode1, pidCode2,
      reco);
  }

  fclose(asciif);
  delete f;

  return true;
}
