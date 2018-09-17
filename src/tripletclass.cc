// (Generated, Reconstructed, Corrected) triplet ROOT histograms
// ------------------------------------------------------------------------
//
// mikael.mieskolainen@cern.ch, 23/07/2018


// C++
#include <string>

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TGaxis.h"
#include "TLegend.h"

// Own
#include "tripletclass.h"


h1Triplet::h1Triplet(const std::string& name, const std::string& labeltext,
                     int N, double minval, double maxval, const std::string& legendposition) {
    name_ = name;
    N_ = N;
    minval_ = minval;
    maxval_ = maxval;
    legendposition_ = legendposition;
    
    hTrue = new TH1D((name + "True").c_str(), labeltext.c_str(), N, minval, maxval);
        hTrue->Sumw2();
    hReco = new TH1D((name + "Reco").c_str(), labeltext.c_str(), N, minval, maxval);
        hReco->Sumw2();
    hCorr = new TH1D((name + "Corr").c_str(), labeltext.c_str(), N, minval, maxval);
        hCorr->Sumw2();
    
    h2ObsWeight = new TH2D(("h2" + name).c_str(), labeltext.c_str(), N, minval, maxval, N, 0, 1.0);
}

double h1Triplet::SaveFig() {
    
    // ----------------------------------------------------
    // Apply the chi2 test and retrieve the residuals
    printf("***********************************************************\n");
    double res[N_] = {0.0};
    printf("%s:: \n", name_.c_str());
    double chi2ndf = hTrue->Chi2Test(hCorr,"WW P CHI2/NDF", res);
    
    printf("chi2/ndf = %0.3f \n\n", chi2ndf);
    printf("***********************************************************\n");
    // ---------------------------------------------------

    TCanvas c0("c", "c", 750, 800);
    
    // Upper plot will be in pad1
    TPad* pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.015); // Upper and lower plot are joined
    //pad1->SetGridx();           // Vertical grid
    pad1->Draw();                 // Draw the upper pad: pad1
    pad1->cd();                   // pad1 becomes the current pad
    hTrue->SetStats(0);           // No statistics on upper plot

    hTrue->SetLineColor(1);
    hTrue->SetMarkerColor(1);
    hTrue->SetMarkerStyle(20);
    hTrue->SetMarkerSize(0.5);
    hTrue->Draw("L");

    hReco->SetLineColor(99);
    hReco->SetMarkerColor(99);
    hReco->SetMarkerStyle(24);
    hReco->SetMarkerSize(0.5);
    hReco->Draw("same");

    hCorr->SetLineColor(59);
    hCorr->SetMarkerColor(59);
    hCorr->SetMarkerStyle(21);
    hCorr->SetMarkerSize(0.5);
    hCorr->Draw("same");
    
    // Remove x-axis
    hTrue->GetXaxis()->SetLabelOffset(999);
    hTrue->GetXaxis()->SetLabelSize(0);

    // Give y-axis title some offset to avoid overlapping with numbers
    hTrue->GetYaxis()->SetTitleOffset(1.45);

    // Do not draw the Y axis label on the upper plot and redraw a small
    // axis instead, in order to avoid the first label (0) to be clipped.
    //hTrue->GetYaxis()->SetLabelSize(0.);
  /*  
    TGaxis* axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
    axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    axis->SetLabelSize(15);
    axis->Draw();
*/

/*
    c0.cd(2);
    hTrue->Draw("same");
    hReco->Draw("same");
    hCorr->Draw("same");
    c0.cd(2)->SetLogy();
*/

    // LEGEND entries
    double x1,x2,y1,y2 = 0.0;

    // North-East
    x1 = 0.60; x2 = 0.87;
    y1 = 0.70; y2 = 0.85;

    // South-East
    if (legendposition_.compare("southeast") == 0) {
        x1 = 0.60; x2 = 0.87;
        y1 = 0.10; y2 = 0.25;
    }
    TLegend* legend = new TLegend(x1,y1, x2,y2); // x1,y1,x2,y2


    legend->SetFillColor(0);  // White background
    //legend->SetBorderSize(0); // No box

    legend->AddEntry(hTrue, "Generated");
    legend->AddEntry(hReco, "Reconstructed");
    legend->AddEntry(hCorr, Form("DeepEfficiency-6D [#chi^{2}_{/ bin}= %0.1f] ", chi2ndf));

    legend->Draw();

    // ==============================================================
    // Ratio plots
    c0.cd();
    TPad* pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0.025);
    pad2->SetBottomMargin(0.25);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();       // pad2 becomes the current pad

    // *** Reconstructed histogram ***
    TH1D* h3 = (TH1D*)hReco->Clone("h3");
    h3->Divide(hTrue);

    h3->SetMinimum(0.0);  // Define Y ..
    h3->SetMaximum(2.0); // .. range
    h3->SetStats(0);     // no stat box
    h3->Draw("same");       // Draw the ratio plot

    // Ratio plot (h3) settings
    h3->SetTitle(""); // Remove the ratio title

    // Y axis ratio plot settings
    h3->GetYaxis()->SetTitle("Ratio");
    h3->GetYaxis()->CenterTitle();
    h3->GetYaxis()->SetNdivisions(505);
    h3->GetYaxis()->SetTitleSize(20);
    h3->GetYaxis()->SetTitleFont(43);
    h3->GetYaxis()->SetTitleOffset(1.55);
    h3->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    h3->GetYaxis()->SetLabelSize(15);

    // X axis ratio plot settings
    h3->GetXaxis()->SetTitleSize(20);
    h3->GetXaxis()->SetTitleFont(43);
    h3->GetXaxis()->SetTitleOffset(4.);
    h3->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    h3->GetXaxis()->SetLabelSize(15);

    // Draw horizontal line
    const double ymax = 1.0;
    TLine* line = new TLine(minval_, ymax, maxval_, ymax);
    line->SetLineColor(15);
    line->SetLineWidth(2.0);
    line->Draw();

    // *** Corrected histogram ***
    TH1D* h4 = (TH1D*)hCorr->Clone("h4");
    h4->Divide(hTrue);
    h4->Draw("same");
    
    // Save pdf
    std::string fullfile = "./figs/" + name_ + ".pdf";
    c0.SaveAs(fullfile.c_str());

    // Save logscale pdf
    pad1->cd()->SetLogy();       // pad2 becomes the current pad
    fullfile = "./figs/" + name_ + "_logy"+ ".pdf";
    c0.SaveAs(fullfile.c_str());

    delete pad1;
    delete pad2;
    delete line;
    delete legend;

    // -------------------------------------------------------------------
    // Print out 2D-control plot
    TCanvas c2D("c2D", "c2D", 800, 800);
    c2D.cd();
    c2D.SetRightMargin(0.13); // Give space for colorbar
    h2ObsWeight->Draw("COLZ");
    h2ObsWeight->SetStats(0);
    h2ObsWeight->GetYaxis()->SetTitleOffset(1.3);
    h2ObsWeight->GetYaxis()->SetTitle("DeepEfficiency-6D output w");
    fullfile = "./figs/" + name_ + "_vs_weight"+ ".pdf";
    c2D.SaveAs(fullfile.c_str()); 

    return chi2ndf;   
}


h2Triplet::h2Triplet(const std::string& name, const std::string& labeltext,
            int N1, double minval1, double maxval1, int N2, double minval2, double maxval2) {
    name_ = name;
    N1_ = N1;
    N2_ = N2;
    
    hTrue = new TH2D((name + "True").c_str(), ("Generated" + labeltext).c_str(), N1, minval1, maxval1, N2, minval2, maxval2);
        hTrue->Sumw2();
    hReco = new TH2D((name + "Reco").c_str(), ("Reconstructed" + labeltext).c_str(), N1, minval1, maxval1, N2, minval2, maxval2);
        hReco->Sumw2();
    hCorr = new TH2D((name + "Corr").c_str(), ("DeepEfficiency-6D" + labeltext).c_str(), N1, minval1, maxval1, N2, minval2, maxval2);
        hCorr->Sumw2();   
}

double h2Triplet::SaveFig() {

    TCanvas c0("c", "c", 800, 525);
    c0.Divide(3, 2, 0.01, 0.02);

    // Scale for normalization
    /*
    double norm = 1.0/hTrue->Integral();
    hTrue->Scale(norm);

    norm = 1.0/hReco->Integral();
    hReco->Scale(norm);

    norm = 1.0/hCorr->Integral();
    hCorr->Scale(norm);
    */

    c0.cd(1);
        hTrue->SetStats(0);          // No statistics on upper plot
        hTrue->Draw("COLZ");
        hTrue->GetYaxis()->SetTitleOffset(1.3);
        hTrue->GetZaxis()->SetRangeUser(0.0, hTrue->GetMaximum());
    c0.cd(2);
        hReco->SetStats(0);          // No statistics on upper plot
        hReco->Draw("COLZ");
        hReco->GetYaxis()->SetTitleOffset(1.3);
        hReco->GetZaxis()->SetRangeUser(0.0, hTrue->GetMaximum());
    c0.cd(3);
        hCorr->SetStats(0);          // No statistics on upper plot
        hCorr->Draw("COLZ");
        hCorr->GetYaxis()->SetTitleOffset(1.3);
        hCorr->GetZaxis()->SetRangeUser(0.0, hTrue->GetMaximum());

    c0.cd(5);
        TH2D* h5 = (TH2D*)hReco->Clone("h5");
        h5->Divide(hTrue);
        h5->GetYaxis()->SetTitleOffset(1.3);
        h5->SetStats(0);          // No statistics on upper plot
        h5->Draw("COLZ");
        h5->GetZaxis()->SetRangeUser(0.0, 2.0);
        h5->SetTitle("Ratio: Reconstructed / Generated");
    c0.cd(6);
        TH2D* h6 = (TH2D*)hCorr->Clone("h6");
        h6->Divide(hTrue);
        h6->GetYaxis()->SetTitleOffset(1.3);
        h6->SetStats(0);          // No statistics on upper plot
        h6->Draw("COLZ");
        h6->GetZaxis()->SetRangeUser(0.0, 2.0);
        h6->SetTitle(Form("Ratio: DeepEfficiency-6D / Generated"));

    // Save pdf
    std::string fullfile = "./figs/" + name_ + ".pdf";
    c0.SaveAs(fullfile.c_str());

    delete h5;
    delete h6;

    return 0.0;
}