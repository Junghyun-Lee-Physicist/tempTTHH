#ifndef BINCONFIG_H
#define BINCONFIG_H

#include <vector>

class BinConfig {
    public:
        // HT bins
	static const std::vector<double>& getHTBins() {
	    static std::vector<double> HT_bins = {500., 600., 700., 800., 1000., 1300., 2500.};
	    return HT_bins;
	}

        // pT bins
        static const std::vector<double>& getPTBins() {
            static std::vector<double> pT_bins = {40., 45., 50., 60., 70., 90., 150.};
            return pT_bins;
        }

        // nBjets bins
        static const std::vector<int>& getNBJetsBins() {
            //static std::vector<int> nBjets_bins = {3, 4, 5, 9};
            static std::vector<int> nBjets_bins = {3, 4, 12};
            return nBjets_bins;
        }

        // Eta bins
        static const std::vector<double>& getEtaBins() {
            static std::vector<double> eta_bins = {-2.5, -2.1, -1.5, -0.8, 0.0, 0.8, 1.5, 2.1, 2.5};
            return eta_bins;
        }


        inline static const int HTBinCount     = BinConfig::getHTBins().size() - 1;
        inline static const int PTBinCount     = BinConfig::getPTBins().size() - 1;
        inline static const int NBJetsBinCount = BinConfig::getNBJetsBins().size() - 1;
        inline static const int EtaBinCount    = BinConfig::getEtaBins().size() - 1;

};


#endif // BINCONFIG_H
