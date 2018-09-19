// Differential distributions with DeepEfficiency inversion
// ------------------------------------------------------------------------
//
//
// Compile with makefile: make && ./deeplot
//
// mikael.mieskolainen@cern.ch, 23/07/2018


#include <iostream>
#include <fstream>
#include <map>
#include <memory>

// ROOT
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TColor.h"

// Own classes
#include "tripletclass.h"


const double PI = 3.14159265359;

// Maximum event count cut (for quick testing)
const int MAXEVENTS = 100000000;


// Particle mass
const double mPI = 0.139570;
const double mK  = 0.493677;


// ****************** FIDUCIAL DEFINITION ******************
// We cut events here for the DeepEfficiency training phase
// Note that these should be kept the same through whole
// algorithmic chain!
const double FID_PT  = 0.1; // track pt
const double FID_ETA = 0.9; // track eta
// *********************************************************


bool Processor(const std::string& PREDICTFILE);
void SetROOTStyle();
void SetPlotStyle();
void PlotFilled(TH1D* h1, std::string& name, bool logscale, bool normalize);
bool ReadKinematics(TLorentzVector& p1_gen, TLorentzVector& p2_gen,
                    TLorentzVector& p1_rec, TLorentzVector& p2_rec, int& reco);


// Kinematic input
FILE* fp;


// Main function
int main(void) {

    SetPlotStyle();

    std::vector<std::string> filenames;
    filenames.push_back("tree2track_kPipmExp");
    filenames.push_back("tree2track_kPipmOrexp");
    filenames.push_back("tree2track_kPipmPower");
    filenames.push_back("tree2track_kCohRhoToPi");

    filenames.push_back("tree2track_kKpkmExp");
    filenames.push_back("tree2track_kKpkmOrexp");
    filenames.push_back("tree2track_kKpkmPower");

    for (uint i = 0; i < filenames.size(); ++i) {
        Processor(filenames.at(i));
    }

    return EXIT_SUCCESS;
}

// Processor
bool Processor(const std::string& PREDICTFILE) {

    // Create output directory in a case
    system("mkdir figs");
    std::string cmd = "mkdir ./figs/" + PREDICTFILE + "/";
    system(cmd.c_str());


    // 1. Open kinematics
    std::string filename = "./data/" + PREDICTFILE + ".csv";

    if ((fp = fopen(filename.c_str(), "r")) == NULL) {
        printf("Cannot open kinematics inputfile: %s \n", filename.c_str());
        return false;
    }

    // 2. Open DeepEfficiency weights
    std::string deepfilename = "./output/" + PREDICTFILE + ".out";

    std::ifstream deepnetfile;
    deepnetfile.open(deepfilename);

    if (!deepnetfile) {
        printf("Cannot open DeepEfficiency outputfile: %s \n", deepfilename.c_str());
        return false;
    }

    // -------------------------------------------------------------------------
    // Create histograms

    // Number of bins
    const int Nbin1D   = 100;
    const int Nbin2D   = 80;

    // (Generated, Reconstructed, Corrected) 1D-histogram triplets
    std::vector<h1Triplet*> h1;
    h1Triplet h1M(PREDICTFILE + "/h1M", ";System M (GeV); events", Nbin1D, 0.0, 4.0, "northeast");
        h1.push_back(&h1M);
    h1Triplet h1Y(PREDICTFILE + "/h1Y", ";System Y; events", Nbin1D, -1.0, 1.0, "northeast");
        h1.push_back(&h1Y);
    h1Triplet h1Pt(PREDICTFILE + "/h1Pt", ";System P_{T} (GeV); events", Nbin1D, 0.0, 2.0, "northeast");
        h1.push_back(&h1Pt);
    h1Triplet h1pt1(PREDICTFILE + "/h1pt1", ";Track p_{T} (GeV); events", Nbin1D, 0.0, 2.0, "northeast");
        h1.push_back(&h1pt1);
    h1Triplet h1eta1(PREDICTFILE + "/h1eta1", ";Track #eta; events", Nbin1D, -1.0, 1.0, "southeast");
        h1.push_back(&h1eta1);
    h1Triplet h1dY(PREDICTFILE + "/h1dY", ";#Deltay #equiv y_{1}-y_{2}; events", Nbin1D, -2.0, 2.0, "northeast");
        h1.push_back(&h1dY);

    // (Generated, Reconstructed, Corrected) 2D-histogram triplets
    std::vector<h2Triplet*> h2;
    h2Triplet h2etaphi(PREDICTFILE + "/h2etaphi", ";Track #eta; Track #phi (rad)", Nbin2D, -1.0, 1.0, Nbin2D,  -PI, PI);
        h2.push_back(&h2etaphi);
    h2Triplet h2etaeta(PREDICTFILE + "/h2etaeta", ";Track #eta^{(1)}; Track #eta^{(2)}", Nbin2D, -1.0, 1.0, Nbin2D,  -1.0, 1.0);
        h2.push_back(&h2etaeta);
    h2Triplet h2pt1pt2(PREDICTFILE + "/h2pt1pt2", ";Track p_{T}^{(1)}; Track p_{T}^{(2)}", Nbin2D, 0.0, 2.0, Nbin2D,  0.0, 2.0);
        h2.push_back(&h2pt1pt2);
    h2Triplet h2Mdeltaphi(PREDICTFILE + "/h2Mdeltaphi", ";System M (GeV); Pair #Delta#phi (rad)", Nbin2D, 0.0, 4.0, Nbin2D,  0, PI);
        h2.push_back(&h2Mdeltaphi);
    h2Triplet h2MPt(PREDICTFILE + "/h2MPt", ";System M (GeV); System P_{T} (GeV)", Nbin2D, 0.0, 4.0, Nbin2D,  0, 2.5);
        h2.push_back(&h2MPt);
    h2Triplet h2Mpt1(PREDICTFILE + "/h2Mpt1", ";System M (GeV); Track p_{T} (GeV)", Nbin2D, 0.0, 4.0, Nbin2D,  0, 2.5);
        h2.push_back(&h2Mpt1);

    // Aux variables
    int reco = 0;
    int k = 0;    
    double weight = 0.0;
    TH1D* h1W = new TH1D("h1W", ";DeepEfficiency-6D output w; events", 200, 0, 1.0);

    // Generated 4-momentum
    TLorentzVector p1_gen;
    TLorentzVector p2_gen;
    TLorentzVector system_gen;    

    // Reconstructed 4-momentum
    TLorentzVector p1_rec;
    TLorentzVector p2_rec;
    TLorentzVector system_rec;

    // Event loop
    while (true) {

        if (k >= MAXEVENTS - 1) {
            printf("Maximum event count = %d reached \n", MAXEVENTS);
            break;
        }
        // Read kinematic input
        if (!ReadKinematics(p1_gen, p2_gen, p1_rec, p2_rec, reco)) {
            break;
        }

        // System 4-momentum, generated and reconstructed
        system_gen = p1_gen + p2_gen;
        system_rec = p1_rec + p2_rec;

        // Read in DeepEfficiency efficiency estimate
        if (!(deepnetfile >> weight)) {
            printf("Weight not found (k = %d)!\n", k);
            break;
        }
        // Inverse weight
        weight = 1.0 / std::min(std::max(weight, 1e-6), 1.0); // max operator regularizator for safety	
	
	
        // ----------------------------------------------------------------
        //        ***** FIDUCIAL CUTS *****
        // Note that DeepEfficiency network should not be trained with more restrictive cuts than what
	// one applied here.
	
	// Use generator level variables here, in order to be able to make "ground truth comparison".
        // When working with data, this option is not possible. 
        if (p1_gen.Perp() > FID_PT && p2_gen.Perp() > FID_PT &&
            std::abs(p1_gen.Eta()) < FID_ETA && std::abs(p2_gen.Eta()) < FID_ETA ) {
            // Event within fiducial
        } else {
            //printf("Event outside fiducial phase space!! Check numerics:: (pt,eta) = (%0.3f,%0.3f), (%0.3f, %0.3f) \n", p1_gen.Perp(), p1_gen.Eta(), p2_gen.Perp(), p2_gen.Eta());
            continue;
        }

        // ----------------------------------------------------------------
        // Construct observables of interest

        TLorentzVector p1;
        TLorentzVector p2;
        TLorentzVector system;

        // Generator level
        const double M_gen        = system_gen.M();
        const double Y_gen        = system_gen.Y();
        const double Pt_gen       = system_gen.Perp();
        const double dY_gen       = p1_gen.Rapidity() - p2_gen.Rapidity();
        const double eta1_gen     = p1_gen.Eta();
        const double pt1_gen      = p1_gen.Perp();
        const double pt2_gen      = p2_gen.Perp();
        const double deltaphi_gen = p1_gen.DeltaPhi(p2_gen);

        // Reconstruction level
        const double M_rec        = system_rec.M();
        const double Y_rec        = system_rec.Y();
        const double Pt_rec       = system_rec.Perp();
        const double dY_rec       = p1_rec.Rapidity() - p2_rec.Rapidity();
        const double eta1_rec     = p1_rec.Eta();
        const double pt1_rec      = p1_rec.Perp();
        const double pt2_rec      = p2_rec.Perp();
        const double deltaphi_rec = p1_rec.DeltaPhi(p2_rec);

        // ----------------------------------------------------------------
        // *** Efficiency correction and plotting ***
        
        // 1D
        h1M.Fill(reco, M_gen, M_rec, weight);
        h1Y.Fill(reco, Y_gen, Y_rec, weight);
        h1Pt.Fill(reco, Pt_gen, Pt_rec, weight);
        h1pt1.Fill(reco, pt1_gen, pt1_rec, weight);
        h1eta1.Fill(reco, eta1_gen, eta1_rec, weight);
        h1dY.Fill(reco, dY_gen, dY_rec, weight);
        

        // 2D
        h2etaphi.Fill(reco, p1_gen.Eta(), p1_gen.Phi(), p1_rec.Eta(), p1_rec.Phi(), weight);
        h2etaeta.Fill(reco, p1_gen.Eta(), p2_gen.Eta(), p1_rec.Eta(), p2_rec.Eta(), weight);
        h2Mdeltaphi.Fill(reco, M_gen, deltaphi_gen, M_rec, deltaphi_rec, weight);
        h2MPt.Fill(reco, M_gen, Pt_gen, M_rec, Pt_rec, weight);
        h2Mpt1.Fill(reco, M_gen, pt1_gen, M_rec, pt1_rec, weight);
        h2pt1pt2.Fill(reco, pt1_gen, pt2_gen, pt1_rec, pt2_rec, weight);
	     

        // DEBUG fills
        h1W->Fill(1.0/weight);
        ++k; // event count
    }

    // Plot
    std::string name = PREDICTFILE + "/hx_weights";
    PlotFilled(h1W, name, false, false);

    // Save 1D-histograms
    double chi2sum = 0.0;
    for (uint i = 0; i < h1.size(); ++i) {
        chi2sum += h1.at(i)->SaveFig();
    }
    printf("=======================================================\n");
    printf("AVERAGE: <Chi2 / ndf> = %0.2f \n", chi2sum / (double)h1.size());
    printf("=======================================================\n");

    // Save 2D-histograms
    for (uint i = 0; i < h2.size(); ++i) {
        h2.at(i)->SaveFig();
    }

    fclose(fp);
    deepnetfile.close();
    delete h1W;

    return true;
}

// Global Style Setup
void SetROOTStyle() {

  gStyle->SetOptStat(0); // Statistics BOX OFF with argument 0
  gStyle->SetTitleSize(0.0475,"t"); // Title with "t" (or anything else than xyz)
  gStyle->SetStatY(1.0);
  gStyle->SetStatX(1.0);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.09);
}


// Set "nice" 2D-plot style
void SetPlotStyle() {

  // Set Smooth color gradients
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

  // Black-Red palette
  gStyle->SetPalette(53); // 53,56 for inverted

  // Number of decimals in text in TH2 plots
  //gStyle->SetPaintTextFormat("4.2f");

  //gStyle->SetTitleOffset(1.4,"x");  //X-axis title offset from axis
  //gStyle->SetTitleOffset(1.4,"y");  //X-axis title offset from axis
  //gStyle->SetTitleSize(0.04,"x");   //X-axis title size
  //gStyle->SetTitleSize(0.04,"y");
  //gStyle->SetTitleSize(0.04,"z");
  //gStyle->SetLabelOffset(0.025);

}

// 
void PlotFilled(TH1D* h1, std::string& name, bool logscale, bool normalize) {

    TCanvas* c = new TCanvas("c","c", 800, 650);
    if (logscale) {
        c->cd()->SetLogy();
    }
    if (normalize) { // Make it discrete probability distribution
        double norm = 1.0/h1->GetEntries();
        //double norm = 1.0/h1->Integral();
        h1->Scale(norm);
    }

    h1->GetYaxis()->SetTitleOffset(1.45);
    h1->SetFillColor(19);
    h1->SetLineWidth(1.5);
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.5);
    h1->SetStats(0);
    h1->Draw(); // "ep"
    c->SaveAs( ("./figs/" + name + ".pdf").c_str() );

    delete c;
}

// Read event kinematics
bool ReadKinematics(TLorentzVector& p1_gen, TLorentzVector& p2_gen,
                    TLorentzVector& p1_rec, TLorentzVector& p2_rec, int& reco) {

    double px1_gen, py1_gen, pz1_gen, px2_gen, py2_gen, pz2_gen,
           px1_rec, py1_rec, pz1_rec, px2_rec, py2_rec, pz2_rec = 0.0;

    int pidCode[2] = {0};

    // Read event kinematics
    int ret = fscanf(fp, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d,%d\n", 
        &px1_gen, &py1_gen, &pz1_gen,
        &px2_gen, &py2_gen, &pz2_gen, 
        &px1_rec, &py1_rec, &pz1_rec,
        &px2_rec, &py2_rec, &pz2_rec,
        &pidCode[0], &pidCode[1], 
        &reco);

    if (ret == 15) { // check we got the right number of variables!
        // ok
    } else if (errno != 0) {
        printf("Kinematics inputfile:: Error in parsing! \n");
        return false;
    } else if (ret == EOF) {
        printf("Kinematics inputfile:: EOF! \n");
        return false;
    } else {
        printf("Kinematics inputfile:: General error! \n");
        return false;
    }

    // Assign masses
    double mass[2] = {0,0};
    for (int i = 0; i < 2; ++i) {
        if (std::abs(pidCode[i]) == 211) { // Charged pion
            mass[i] = mPI;
        }
        if (std::abs(pidCode[i]) == 321) { // Charged kaon
            mass[i] = mK;
        }
    }

    // Construct 4-momenta
    p1_gen.SetXYZM(px1_gen, py1_gen, pz1_gen, mass[0]);
    p2_gen.SetXYZM(px2_gen, py2_gen, pz2_gen, mass[1]);

    p1_rec.SetXYZM(px1_rec, py1_rec, pz1_rec, mass[0]);
    p2_rec.SetXYZM(px2_rec, py2_rec, pz2_rec, mass[1]);

    return true;
}
