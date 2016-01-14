#include <PdfConverter.h>
#include <BinnedPdf.h>
#include <IntegrablePdf.h>
#include <AxisCollection.h>
#include <vector>
#include <PdfExceptions.h>
#include <iostream>
#include <algorithm>
#include <Histogram.h>

BinnedPdf
PdfConverter::ToBinnedPdf(const IntegrablePdf& analytic_, const AxisCollection& axes_){
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
/*
TH1D
PdfConverter::ToTH1D(const BinnedPdf& pdf_){
    if(pdf_.GetNDims() != 1)
        throw DimensionError("Only a 1D pdf can be represented by a TH1D");
 
    const PdfAxis& axis  = pdf_.GetAxes().GetAxis(0);
    const unsigned nBins = axis.GetNBins(); 
    TH1D rtHist("", "", nBins, axis.GetBinLowEdge(0), axis.GetBinHighEdge(nBins - 1));
    rtHist.SetDirectory(0);

    for(unsigned bin = 0; bin < nBins; bin++)
        rtHist.SetBinContent(bin+1, pdf_.GetBinContent(bin));
    
    return rtHist;                         
}

TH1D
PdfConverter::ToTH1D(const Histogram& histo_){
    if(histo_.GetNDims() != 1)
        throw DimensionError("Only a 1D pdf can be represented by a TH1D");
 
    const PdfAxis& axis  = histo_.GetAxes().GetAxis(0);
    const unsigned nBins = axis.GetNBins(); 
    TH1D rtHist("", "", nBins, axis.GetBinLowEdge(0), axis.GetBinHighEdge(nBins - 1));
    rtHist.SetDirectory(0);

    for(unsigned bin = 0; bin < nBins; bin++)
        rtHist.SetBinContent(bin+1, histo_.GetBinContent(bin));    
    return rtHist;                         
}
*/

BinnedPdf
PdfConverter::Marginalise(const BinnedPdf& binnedPdf_, 
                          const std::vector<size_t>& indices_){
    // check the pdf does contain the indices asked for
    DataRepresentation dRep = binnedPdf_.GetDataRep();
    std::vector<size_t> oldDataRepIndices = dRep.GetIndices();

    if(!indices_.size())
        throw DimensionError("Requested marginalisation with no indices to project!");

    if (!dRep.GetLength())
        throw DimensionError("Cant project out, pdf doesn't have indices - set the data rep!");

    for(size_t i = 0; i < indices_.size(); i++)
        if (std::find(oldDataRepIndices.begin(), 
                      oldDataRepIndices.end(), 
                      indices_.at(i)) == oldDataRepIndices.end()){
            throw DimensionError("PdfConverter: Tried to project pdf onto dimension it doesn't contain!");
        }
    
    std::vector<size_t> relativeIndices = DataRepresentation(indices_).GetRelativeIndices(dRep);

    // Get the axes you are interested in,  in the order requested
    AxisCollection newAxes;
    for(size_t i = 0;  i < indices_.size(); i++)
        newAxes.AddAxis(binnedPdf_.GetAxes().GetAxis(indices_.at(i)));

    // New pdf
    BinnedPdf marginalisedPdf(newAxes);
    
    // Now loop over the bins in old and fill new pdfs
    for(size_t bin = 0; bin < binnedPdf_.GetNBins(); bin++){
        std::vector<size_t> oldIndices = binnedPdf_.UnpackIndices(bin);
        std::vector<size_t> newIndices;

        for(size_t i = 0; i < relativeIndices.size(); i++)
            newIndices.push_back(oldIndices.at(relativeIndices.at(i)));
        
        
        size_t newBin = marginalisedPdf.FlattenIndices(newIndices);

        marginalisedPdf.AddBinContent(newBin, binnedPdf_.GetBinContent(bin));
    }
    return marginalisedPdf;
}

Histogram
PdfConverter::Marginalise(const Histogram& histo_, 
                          const std::vector<size_t>& indices_){
    // check the pdf does contain the indices asked for
  for(size_t i = 0; i < indices_.size(); i++){
    if (indices_.at(i) >= histo_.GetNDims())
      throw DimensionError("Marginalise Histogram::Tried to project out non existent dim!");
  }
    // Get the axes you are interested in,  in the order requested
    AxisCollection newAxes;
    for(size_t i = 0;  i < indices_.size(); i++)
        newAxes.AddAxis(histo_.GetAxes().GetAxis(indices_.at(i)));

    // New pdf
    Histogram marginalised(newAxes);
    
    std::vector<size_t> oldIndices(histo_.GetNDims());
    std::vector<size_t> newIndices(indices_.size());
    size_t newBin = -1;

    // Now loop over the bins in old and fill new pdfs
    for(size_t bin = 0; bin < histo_.GetNBins(); bin++){
        for(size_t i = 0; i < histo_.GetNDims(); i++)
            oldIndices[i] = histo_.GetAxes().UnflattenIndex(bin, i);

        for(size_t i = 0; i < indices_.size(); i++)
            newIndices[i] = oldIndices.at(indices_.at(i));
                
        newBin = marginalised.FlattenIndices(newIndices);
        marginalised.AddBinContent(newBin, histo_.GetBinContent(bin));
    }
    return marginalised;
}

BinnedPdf
PdfConverter::Marginalise(const BinnedPdf& pdf_, size_t index_){
    return Marginalise(pdf_, std::vector<size_t> (1, index_));
}
