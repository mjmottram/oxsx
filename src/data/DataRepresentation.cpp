#include <DataRepresentation.h>
#include <Exceptions.h>
#include <iostream>

DataRepresentation::DataRepresentation(const std::vector<size_t>& indices_){
    SetIndices(indices_);
}

DataRepresentation::DataRepresentation(size_t index_){
    SetIndices(std::vector<size_t>(1, index_));
}

DataRepresentation::DataRepresentation(const DataRepresentation& other_){
    fName = other_.fName;
    SetIndices(other_.fIndices);
}


std::vector<size_t>
DataRepresentation::GetIndices() const {return fIndices;}

void 
DataRepresentation::SetIndices(const std::vector<size_t>& indices_)  {
    fIndices = indices_;
    fNObservables = indices_.size();

    // build inverse map
    fInverse.resize(0);
    for(size_t i = 0; i < fIndices.size(); i++)
        fInverse.push_back(std::pair<size_t,size_t>(fIndices.at(i), i));
    
}

std::string
DataRepresentation::GetName() const {return fName;}

void
DataRepresentation::SetName(const std::string& name_) {fName = name_;}

// FIXME -  needs bounds handling!
size_t
DataRepresentation::GetIndex(size_t indexNum_) const{return fIndices.at(indexNum_);}

size_t
DataRepresentation::GetNObservables() const {return fNObservables;}

size_t
DataRepresentation::GetDataIndexPos(size_t dataIndex_) const{
    for(size_t i = 0; i < fInverse.size(); i++){
        if (dataIndex_ == fInverse.at(i).first)
            return fInverse.at(i).second;
    }
    throw RepresentationError("Incompatible data representation"); // 
}

std::vector<size_t> 
DataRepresentation::GetRelativeIndices(const DataRepresentation& otherRep_) const{
    // given a longer represetation containing the indices of this rep, where are the shared indices
    // in the big rep

    if(otherRep_.fNObservables < fNObservables)
        throw RepresentationError("Tried to take relatice indices in compatible representations ");
        
    std::vector<size_t> relativeIndices(fIndices.size(), 0);
    for(size_t i = 0; i < fIndices.size(); i++)
        relativeIndices[i] = otherRep_.GetDataIndexPos(fIndices.at(i));
    
    return relativeIndices;
} 
