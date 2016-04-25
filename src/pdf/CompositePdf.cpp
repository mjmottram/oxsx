#include <CompositePdf.h>
#include <EventData.h>
#include <Exceptions.h>
#include <iostream>

CompositePdf::CompositePdf(const Pdf* p1_, const Pdf* p2_) {
    fPdfPtrs.push_back(p1_ -> Clone());
    fPdfPtrs.push_back(p2_ -> Clone());  

}

CompositePdf::CompositePdf(const std::vector<Pdf*>& pdfs_){
    // if one of the pdfs is composite itself the copy will happen recursively all the way down
    for(size_t i = 0; i < pdfs_.size(); i++){
        fPdfPtrs.push_back(pdfs_[i] -> Clone());
    }
}
CompositePdf::~CompositePdf() {
    for(size_t i = 0; i < fPdfPtrs.size(); i++)
        delete fPdfPtrs[i];
}

double 
CompositePdf::operator() (const std::vector<double>& vals_) const{
    double prob = 1;
    try{
        for(size_t i = 0; i < fPdfPtrs.size(); i++)
            prob *= fPdfPtrs[i] -> operator() (vals_);
    }

    catch(const DimensionError& e_){
        throw DimensionError("Passed vector of values into composite where pdfs are not all same dim! Use Probability() if you want the data distributed across the internal pdfs");
    }
    return prob;
}

double 
CompositePdf::Probability(const EventData& data_) const{
    double prob = 1;
    for(size_t i = 0; i < fPdfPtrs.size(); i++)
        prob *= fPdfPtrs[i] -> Probability(data_);
    return prob;
}

void 
CompositePdf::Normalise(){
    for(size_t i = 0; i < fPdfPtrs.size(); i++)
        fPdfPtrs[i] -> Normalise();
}

double 
CompositePdf::Integral() const{
    double integral = 1;
    for(size_t i = 0; i < fPdfPtrs.size(); i++)
        integral *= fPdfPtrs[i] -> Integral();
    return integral;
}

Pdf* 
CompositePdf::Clone() const {
    Pdf *cp = new CompositePdf(fPdfPtrs);
    return static_cast<Pdf*>(cp);
}


CompositePdf
operator * (const Pdf& pdf1_, const Pdf& pdf2_){
    return CompositePdf(&pdf1_, &pdf2_);
}


unsigned
CompositePdf::GetNDims() const{
    unsigned nDims = 0;
    for(size_t i = 0; i < fPdfPtrs.size(); i++)
        nDims += fPdfPtrs.at(i)->GetNDims();
    return nDims;
}
