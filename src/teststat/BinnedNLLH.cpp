#include <BinnedNLLH.h>
#include <math.h>
#include <DataSet.h>
#include <Exceptions.h>
#include <iostream>

double 
BinnedNLLH::Evaluate(){
    if(!fDataSet && !fCalculatedDataPdf) 
        throw LogicError("BinnedNNLH function called with no data set and no DataPdf! set one of these first");
    
    if (!fCalculatedDataPdf)
        BinData();
    
    // Construct systematics
    fSystematicManager.Construct();

    // Apply systematics
    fPdfManager.ApplySystematics(fSystematicManager);

    // Apply Shrinking
    fPdfManager.ApplyShrink(fPdfShrinker);

    // loop over bins and calculate the likelihood
    double nLogLH = 0;
    for(size_t i = 0; i < fDataPdf.GetNBins(); i++){
        double prob = fPdfManager.BinProbability(i);	
        if(!prob)
            throw std::runtime_error("BinnedNLLH::Encountered zero probability bin!");
        nLogLH -= fDataPdf.GetBinContent(i) *  log(prob);        
    }


    // Extended LH correction
    const std::vector<double>& normalisations = fPdfManager.GetNormalisations();
    for(size_t i = 0; i < normalisations.size(); i++)
        nLogLH += normalisations.at(i);
            
    // Constraints
    // FIXME:: Put this back in!
//     for(size_t i = 0; i < fSystematicConstraints.size(); i++)
//         nLogLH += fSystematicConstraints.at(i).operator()(fSystematicParams);
    
//     for(size_t i = 0; i < fNormalisationConstraints.size(); i++)
//         nLogLH += fNormalisationConstraints.at(i).operator()(normalisations);

    return nLogLH;
}

void
BinnedNLLH::BinData(){
    BinnedPdf dataPdf(fPdfManager.GetOriginalPdf(0)); // make a copy for same binning and data rep
    dataPdf.Empty();
    for(size_t i = 0; i < fDataSet -> GetNEntries(); i++){
        EventData dat = fDataSet -> GetEntry(i);
        dataPdf.Fill(dat);
    }
    fDataPdf = fPdfShrinker.ShrinkPdf(dataPdf);
    fCalculatedDataPdf = true;
}

void
BinnedNLLH::SetPdfManager(const BinnedPdfManager& man_){
    fPdfManager = man_;
}

void
BinnedNLLH::SetSystematicManager(const SystematicManager& man_){
    fSystematicManager = man_;
}


void
BinnedNLLH::SetSystematicConstraint(size_t index_, const QuadraticConstraint& constr_){
    if(index_ >= fSystematicConstraints.size())
        throw DimensionError("Tried to set systematic constraint that doesn't exist : logic error");
    fSystematicConstraints[index_] = constr_;
}


void
BinnedNLLH::AddNormalisationConstraint(const QuadraticConstraint& constr_){
    fNormalisationConstraints.push_back(constr_);
}

void
BinnedNLLH::SetNormalisationConstraint(size_t index_, const QuadraticConstraint& constr_){
    if(index_ >= fNormalisationConstraints.size())
        throw DimensionError("BinnedNNLH::Tried to set systematic constraint that doesn't exist : logic error");
    fNormalisationConstraints[index_] = constr_;
}

QuadraticConstraint
BinnedNLLH::GetNormalisationConstraint(size_t index_) const{
    if(index_ >= fNormalisationConstraints.size())
        throw DimensionError("BinnedNLLH::Attempted access on non existent constraint");
    return fNormalisationConstraints.at(index_);
}


QuadraticConstraint
BinnedNLLH::GetSystematicConstraint(size_t index_) const{
    if(index_ >= fSystematicConstraints.size())
        throw DimensionError("BinnedNLLH::Attempted access on non existent constraint");
    return fSystematicConstraints.at(index_);
}

void
BinnedNLLH::AddPdf(const BinnedPdf& pdf_){
    fPdfManager.AddPdf(pdf_);
}

void 
BinnedNLLH::AddSystematic(Systematic* sys_){
    fSystematicManager.Add(sys_);
}

void
BinnedNLLH::SetDataSet(DataSet& dataSet_){
    fDataSet = &dataSet_;
    fCalculatedDataPdf = false;
}

void
BinnedNLLH::SetDataSet(DataSet* dataSet_){
    fDataSet = dataSet_;
    fCalculatedDataPdf = false;
}

DataSet*
BinnedNLLH::GetDataSet(){
    return fDataSet;
}

void
BinnedNLLH::SetDataPdf(const BinnedPdf& binnedPdf_){
  fDataPdf = fPdfShrinker.ShrinkPdf(binnedPdf_);
  fCalculatedDataPdf = true;
}

BinnedPdf
BinnedNLLH::GetDataPdf() const{
    return fDataPdf;
}


void
BinnedNLLH::SetBuffer(size_t dim_, unsigned lower_, unsigned upper_){
    fPdfShrinker.SetBuffer(dim_, lower_, upper_);
}

std::pair<unsigned, unsigned>
BinnedNLLH::GetBuffer(size_t dim_) const{
    return fPdfShrinker.GetBuffer(dim_);
}

void
BinnedNLLH::SetBufferAsOverflow(bool b_){
    fPdfShrinker.SetUsingOverflows(b_);
}

bool
BinnedNLLH::GetBufferAsOverflow() const{
    return fPdfShrinker.GetUsingOverflows();
}

void
BinnedNLLH::AddPdfs(const std::vector<BinnedPdf>& pdfs_){
  for(size_t i = 0; i < pdfs_.size(); i++)
    AddPdf(pdfs_.at(i));
}

void
BinnedNLLH::AddSystematics(const std::vector<Systematic*> systematics_){
  for(size_t i = 0; i < systematics_.size(); i++)
    AddSystematic(systematics_.at(i));
}


void
BinnedNLLH::SetNormalisations(const std::vector<double>& norms_){    
    fPdfManager.SetNormalisations(norms_);
}

std::vector<double>
BinnedNLLH::GetNormalisations() const{
    return fPdfManager.GetNormalisations();
}


/////////////////////////////////////////////////////////
// Declare which objects should be adjusted by the fit //
/////////////////////////////////////////////////////////
void
BinnedNLLH::RegisterFitComponents(){
    fComponentManager.AddComponent(&fPdfManager);
    for(size_t i = 0; i < fSystematicManager.GetSystematics().size(); i++)
        fComponentManager.AddComponent(fSystematicManager.GetSystematics().at(i));
}

void
BinnedNLLH::SetParameters(const std::vector<double>& params_){
    try{
        fComponentManager.SetParameters(params_);
    }
    catch(const ParameterCountError& e_){
        throw ParameterCountError(std::string("BinnedNLLH::") + e_.what());
    }
}
                                             
                 
std::vector<double>
BinnedNLLH::GetParameters() const{
    return fComponentManager.GetParameters();
}

int
BinnedNLLH::GetParameterCount() const{
        return fComponentManager.GetTotalParameterCount();
}

std::vector<std::string>
BinnedNLLH::GetParameterNames() const{
    return fComponentManager.GetParameterNames();
}
