/*
An example demonstrating the use of the binned Pdf class
A 2D Pdf in radius and energy is created, followed by a 1D
psd pdf. The final step is combining the two to produce a 3D
pdf where psd is independent of both energy and radius
P(E,r, PSD) = P'(R, r) * P''(PSD)

Note: you need at least three observables in your data set to run this script
*/

#include <BinnedPdf.h>
#include <CompositePdf.h>
#include <Gaussian.h>
#include <ROOTNtuple.h>
#include <PdfConverter.h>
#include <iostream>
#include <string>

const std::string dataPath = "";
const std::string treeName = "";

int main(){
    // Set Up binning
    AxisCollection axes;
    axes.AddAxis(PdfAxis("energy", 2, 3, 10, "Energy")); 
    // PdfAxis(name, min, max, nbins, latex_name)
    
    // custom radial bins as vec
    std::vector<double> radialBinLows(10);
    std::vector<double> radialBinHighs(10);
    for(size_t i = 0; i < radialBinLows.size(); i++){
        radialBinLows[i] = 0.1 * i;
        radialBinHighs[i] = 0.1 * i + 0.05;
    }
    
    axes.AddAxis(PdfAxis("radius", radialBinLows, radialBinHighs, "$\rho$"));
    // PdfAxis(name, binLowEdges, binHighEdges, latex_name)
    
    BinnedPdf energyRadiusPdf(axes);
    
    // Tell the pdf where it looks in the data - the first two indicies in the ntuples
    std::vector<size_t> indicies;
    indicies.push_back(0);
    indicies.push_back(1);
    energyRadiusPdf.SetDataRep(DataRepresentation(indicies));
    
    // fill from mc data
    ROOTNtuple twoNuMC(dataPath, treeName);

    for(size_t i = 0; i < twoNuMC.GetNEntries(); i++){
        EventData evData = twoNuMC.GetEntry(i);
        energyRadiusPdf.Fill(evData);                // pdf knows where to look
    }
    energyRadiusPdf.Normalise();

    // Make analytic approximation in a third variable, say PSD
    Gaussian psdGaus(0.5, 1); // mean,sigma
    
    // Turn it into a binned pdf looking at index 2
    AxisCollection psdAxes;
    psdAxes.AddAxis(PdfAxis("psd", -2, 2, 10));
    
    DataRepresentation psdRep(2);

    // this method integrates the gaussian over the binning defined in pdfAxes
    BinnedPdf psdPDF = PdfConverter::ToBinnedPdf(psdGaus, psdAxes); 
    psdPDF.SetDataRep(psdRep);

    // Combine the two into a new pdf
    CompositePdf finalPdf = energyRadiusPdf * psdPDF;

    // index manipulations done automatically
    std::cout << finalPdf.Probability(twoNuMC.GetEntry(0)) << std::endl;

    return 0;
}
