#include <PdfConverter.h>
#include <BinnedPdf.h>
#include <IntegrableFunction.h>
#include <AxisCollection.h>
#include <vector>
#include <Exceptions.h>
#include <iostream>
#include <algorithm>
#include <Histogram.h>
#include <TH1D.h>
#include <TH2D.h>

BinnedPdf
PdfConverter::ToBinnedPdf(const IntegrableFunction& analytic_, const AxisCollection& axes_){
    if (analytic_.GetNDims() != axes_.GetNDimensions())
        throw DimensionError("PdfConverter::Dimensionality of Pdf doesn't match requested axes!");

    BinnedPdf binnedPdf(axes_);
    // Loop over bins
    std::vector<double> lows(axes_.GetNDimensions());
    std::vector<double> highs(axes_.GetNDimensions());
    for(size_t bin = 0; bin < axes_.GetNBins(); bin++){
        axes_.GetBinLowEdges(bin, lows);
        axes_.GetBinHighEdges(bin, highs);

        binnedPdf.AddBinContent(bin, analytic_.Integral(lows, highs));
    }
    
    return binnedPdf;
}

TH1D
PdfConverter::ToTH1D(const BinnedPdf& pdf_, const bool widthCorrect_){
    if(pdf_.GetNDims() != 1)
        throw DimensionError("Only a 1D pdf can be represented by a TH1D");
 
    const PdfAxis& axis  = pdf_.GetAxes().GetAxis(0);
    const unsigned nBins = axis.GetNBins(); 
	std::vector<double> lowEdges  = axis.GetBinLowEdges();
	std::vector<double> highEdges = axis.GetBinHighEdges();
	lowEdges.push_back(highEdges.back());

    TH1D rtHist("", "", nBins, &lowEdges.at(0));
    rtHist.SetDirectory(0);

    for(unsigned bin = 0; bin < nBins; bin++)
	  if (widthCorrect_)
		rtHist.SetBinContent(bin+1, pdf_.GetBinContent(bin)/axis.GetBinWidth(bin));
	  else
        rtHist.SetBinContent(bin+1, pdf_.GetBinContent(bin));
    
    return rtHist;                         
}

TH1D
PdfConverter::ToTH1D(const Histogram& histo_, const bool widthCorrect_){
    if(histo_.GetNDims() != 1)
        throw DimensionError("Only a 1D pdf can be represented by a TH1D");
 
    const PdfAxis& axis  = histo_.GetAxes().GetAxis(0);
    const unsigned nBins = axis.GetNBins(); 
	std::vector<double> lowEdges = axis.GetBinLowEdges();
	std::vector<double> highEdges = axis.GetBinHighEdges();
	lowEdges.push_back(highEdges.back());
    TH1D rtHist("", "", nBins, &lowEdges.at(0));
    rtHist.SetDirectory(0);

    for(unsigned bin = 0; bin < nBins; bin++)
	  if (widthCorrect_)
		rtHist.SetBinContent(bin+1, histo_.GetBinContent(bin)/axis.GetBinWidth(bin));
	  else
        rtHist.SetBinContent(bin+1, histo_.GetBinContent(bin));    
    return rtHist;                         
}

TH2D
PdfConverter::ToTH2D(const Histogram& histo_, const bool widthCorrect_){
    if(histo_.GetNDims() != 2)
	  throw DimensionError("Only a 2D pdf can be represented by a TH2D");
	
	const PdfAxis& axis0 = histo_.GetAxes().GetAxis(0);
	const PdfAxis& axis1 = histo_.GetAxes().GetAxis(1);
	const unsigned nBinsX = axis0.GetNBins();
	const unsigned nBinsY = axis1.GetNBins();

	std::vector<double> lowEdgesX = axis0.GetBinLowEdges();
	lowEdgesX.push_back(axis0.GetMaximum());

	std::vector<double> lowEdgesY = axis1.GetBinLowEdges();
	lowEdgesY.push_back(axis1.GetMaximum());
	
	TH2D rtHist("", "", 
				nBinsX, &lowEdgesX.at(0), 
				nBinsY, &lowEdgesY.at(0));
	rtHist.SetDirectory(0);
	
	for(size_t i = 0; i < histo_.GetNBins(); i++){
	  rtHist.Fill(histo_.GetBinCentre(i, 0), // x
				  histo_.GetAxes().GetBinCentre(i, 1), // y
				  histo_.GetBinContent(i)    // content
				  );
	}
	return rtHist;
}

TH2D
PdfConverter::ToTH2D(const BinnedPdf& pdf_, const bool widthCorrect_){
    if(pdf_.GetNDims() != 2)
		throw DimensionError("Only a 2D pdf can be represented by a TH2D");
	
	TH2D rtHist = PdfConverter::ToTH2D(pdf_.GetHistogram());
	return rtHist;
}