#include <AxisCollection.h>
#include <iostream>
#include <PdfExceptions.h>

size_t AxisCollection::GetNBins() const {return fNBins;}
size_t AxisCollection::GetNDimensions() const {return fNDimensions;}

const 
PdfAxis& AxisCollection::GetAxis(size_t axisIndex_) const{
    try {
        return fAxes.at(axisIndex_);
    }

    catch (const std::out_of_range& e_){
        throw DimensionError(e_.what());
    }
}

void 
AxisCollection::CountBins(){
    if (!fNDimensions)
        fNBins = 0;

    else{
        fNBins = 1;
        for(size_t i = 0; i < fNDimensions; i++)
            fNBins *= fAxisNbins[i];
    }
}

void 
AxisCollection::AddAxis(const PdfAxis& axis_){
    if (HasAxis(axis_.GetName())){
        throw DimensionError("Can't add axis " + axis_.GetName() + " already exists!"); 
    }
        
    fAxes.push_back(axis_);
    fAxisNbins.push_back(axis_.GetNBins());
    fNDimensions++;
    CountBins();
}

void 
AxisCollection::AddAxes(const std::vector<PdfAxis>& axes_){
    for(size_t i = 0; i < axes_.size(); i++)
        AddAxis(axes_[i]);
}

size_t 
AxisCollection::FlattenIndices(const std::vector<size_t>& indices_) const{
    if(indices_.size() != fNDimensions)
        throw DimensionError("Can't flatten! wrong number of indices");

    if(fNDimensions == 1)
        return indices_.at(0);

    size_t index = indices_.back();
    for (size_t i = 0; i < fNDimensions-1; i++){
        size_t x = indices_[i];
        for(size_t j = i+1; j < fNDimensions; j++)
            x *= fAxisNbins[j];
        index += x;
    }
    return index;
}


size_t 
AxisCollection::FindBin(const std::vector<double>& vals_) const{
    if (vals_.size() != fNDimensions)
        throw DimensionError("Can't find bin! wrong number of vals");

    std::vector<size_t> indices(fNDimensions, 0);
    for (size_t i = 0; i < fNDimensions; i++)
        indices[i] = fAxes.at(i).FindBin(vals_[i]);

    return FlattenIndices(indices);
}

size_t 
AxisCollection::UnflattenIndex(size_t index_, size_t dim_) const{
    if(fNDimensions == 1)
        return index_;

    if (index_ >= fNBins)
        throw OutOfBoundsError("index out of bounds");
    size_t x =1;
    for (size_t i = dim_+1 ; i < fNDimensions; i++ )
        x *= fAxisNbins[i];
    return (index_/x) % fAxisNbins[dim_];
}

std::vector<size_t>
AxisCollection::UnpackIndices(size_t index_) const{
    std::vector<size_t> indices(fNDimensions, 0);
    for(size_t i = 0; i < fNDimensions; i++)
        indices[i] = UnflattenIndex(index_, i);
    return indices;
}

bool 
AxisCollection::HasAxis(const std::string& name_){
    for(size_t i = 0; i < fNDimensions; i++)
        if(name_ == fAxes[i].GetName())
            return true;
    return false;
}

void
AxisCollection::GetBinLowEdges(size_t bin_, std::vector<double>& output_) const{

    if(bin_ > fNBins)
        throw OutOfBoundsError("Bin Edge call on out of bounds bin!");

    if(output_.size() != fNDimensions)
        throw DimensionError("wrong size vec passed to get bin edges");

    for(size_t i = 0; i <fNDimensions; i++){
        output_[i] = fAxes[i].GetBinLowEdge(UnflattenIndex(bin_, i));
    }
}

void
AxisCollection::GetBinHighEdges(size_t bin_, std::vector<double>& output_) const{

    if(bin_ > fNBins)
        throw OutOfBoundsError("Bin Edge call on out of bounds bin!");
    

    if(output_.size() != fNDimensions)
        throw DimensionError("wrong size vec passed to get bin edges");

    for(size_t i = 0; i <fNDimensions; i++){
        output_[i] = fAxes[i].GetBinHighEdge(UnflattenIndex(bin_, i));
    }
}

void
AxisCollection::GetBinCentres(size_t bin_, std::vector<double>& output_) const{

    if(bin_ >= fNBins)
        throw OutOfBoundsError("Bin Edge call on out of bounds bin!");

    if(output_.size() != fNDimensions)
        throw DimensionError("wrong size vec passed to get bin edges");

    for(size_t i = 0; i <fNDimensions; i++){
        output_[i] = fAxes[i].GetBinCentre(UnflattenIndex(bin_, i));
    }
}

double
AxisCollection::GetBinLowEdge(size_t bin_, size_t dim_) const{
    if(dim_ >= fNDimensions)
        throw DimensionError("Axis Collection::Bin edge on non existent dimension!");

    if(bin_ >= fNBins)
        throw OutOfBoundsError("AxisCollection::Called bin edge on out of bounds bin!");
    
    return fAxes.at(dim_).GetBinLowEdge(UnflattenIndex(bin_, dim_));
}


double
AxisCollection::GetBinHighEdge(size_t bin_, size_t dim_) const{
    if(dim_ >= fNDimensions)
        throw DimensionError("Axis Collection::Bin edge on non existent dimension!");
   
    if(bin_ >= fNBins)
        throw OutOfBoundsError("AxisCollection::Called bin edge on out of bounds bin!");
    
    return fAxes.at(dim_).GetBinHighEdge(UnflattenIndex(bin_, dim_));
}

double
AxisCollection::GetBinCentre(size_t bin_, size_t dim_) const{
    if(dim_ >= fNDimensions)
        throw DimensionError("Axis Collection::Bin edge on non existent dimension!");

    if(bin_ >= fNBins)
        throw OutOfBoundsError("AxisCollection::Called bin edge on out of bounds bin!");
    
    return fAxes.at(dim_).GetBinCentre(UnflattenIndex(bin_, dim_));
}

double
AxisCollection::GetBinWidth(size_t bin_, size_t dim_) const{
    if(dim_ >= fNDimensions)
        throw DimensionError("Axis Collection::Bin edge on non existent dimension!");

    if(bin_ >= fNBins)
        throw OutOfBoundsError("AxisCollection::Called bin edge on out of bounds bin!");
    
    return fAxes.at(dim_).GetBinWidth(UnflattenIndex(bin_, dim_));
}

