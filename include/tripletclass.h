// (Generated, Reconstructed, Corrected) triplet ROOT histograms
// 
// mikael.mieskolainen@cern.ch, 23/07/2018


#ifndef TRIPLETCLASS_H
#define TRIPLETCLASS_H

// C++
#include <string>

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"


class h1Triplet {

public:
    h1Triplet(const std::string& name, const std::string& labeltext, 
            int N, double minval, double maxval, const std::string& legendposition);
    ~h1Triplet() {
        delete hTrue; delete hReco; delete hCorr; delete h2ObsWeight;
    }
    
    void Fill(bool reco, double x_gen, double x_rec, double weight) {
        
        hTrue->Fill(x_gen, 1.0);        // Generated

        if (reco == true) {
            hReco->Fill(x_rec, 1.0);    // Reconstructed
            hCorr->Fill(x_rec, weight); // Corrected (inverted)

            // Control plot
            h2ObsWeight->Fill(x_rec, 1.0/weight); // Note 1/weight
        }
    }

    // Plot and save 1D-histogram triplet (left linear, right logarithmic)
    double SaveFig();

    std::string name_;
    int N_;
    double minval_;
    double maxval_;
    std::string legendposition_;

    TH1D* hTrue;
    TH1D* hReco;
    TH1D* hCorr;

    TH2D* h2ObsWeight; // Control plot

    //ClassDef(h1Triplet,1);         // ROOT system integration
};

class h2Triplet {

public:
    h2Triplet(const std::string& name, const std::string& labeltext,
            int N1, double minval1, double maxval1, int N2, double minval2, double maxval2);
    ~h2Triplet() {
        delete hTrue; delete hReco; delete hCorr;
    }

    void Fill(bool reco, double x_gen, double y_gen, double x_rec, double y_rec, double weight) {
        
        hTrue->Fill(x_gen, y_gen, 1.0);        // Generated

        if (reco == true) {
            hReco->Fill(x_rec, y_rec, 1.0);    // Reconstructed
            hCorr->Fill(x_rec, y_rec, weight); // Corrected (inverted)
        }
    }

    // Plot and save 2D-histogram triplet
    double SaveFig();

    std::string name_;
    int N1_;
    int N2_;

    TH2D* hTrue;
    TH2D* hReco;
    TH2D* hCorr;

    //ClassDef(h2Triplet,1);         // ROOT system integration
};


#endif

/*

// Write here just as above

class hpTriplet {

public:

    TProfile* hTrue;
    TProfile* hReco;
    TProfile* hCorr;
};
*/

