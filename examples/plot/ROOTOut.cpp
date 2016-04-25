/*
  Demonstrates how to render 1 or 2 D histograms/BinnedPdfs to ROOT 
  histograms
*/

#include <BinnedPdf.h>
#include <PdfConverter.h>
#include <Histogram.h>
#include <TH1D.h>
#include <TH2D.h>

int main(){
    // 1D example
    // Make a dummy histogram
    AxisCollection axes;
    axes.AddAxis(PdfAxis("name", 0, 10, 100));

    Histogram histo(axes);
    for(int i = 0; i < 100; i++)
        histo.Fill(i, i * i ); // second argument is a weight

    // Use it to make a dummy BinnedPdf
    BinnedPdf pdf(histo);

    // Convert to root objects
    // the second argument toggles whether 
    // probability = height (false) or area (true)
    TH1D th1fromHisto = PdfConverter::ToTH1D(histo, false);
    TH1D th1fromPdf   = PdfConverter::ToTH1D(histo, false);

    

    // 2D example

    // set up 
    axes.AddAxis(PdfAxis("name2", 1, 2, 10));
    Histogram histo2D(axes);
    
    for(size_t i = 0; i < histo2D.GetNBins(); i++)
        histo2D.AddBinContent(i, i * i);
    
    BinnedPdf pdf2D(histo2D);
        
    // convert
    TH2D th2dfromHisto = PdfConverter::ToTH2D(histo2D, false);
        

    return 0;
}
