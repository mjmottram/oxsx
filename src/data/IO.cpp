#include <IO.h>
#include <DataSet.h>
#include <OXSXDataSet.h>
#include <H5Cpp.h>
#include <iostream>
#include <cassert>
#include <DataExceptions.h>

const char IO::fDelimiter = ':';

void
IO::SaveDataSet(const DataSet& dataSet_, const std::string& filename_){
    // Get the relevant params
    hsize_t nEntries     = dataSet_.GetNEntries();
    hsize_t nObs  = dataSet_.GetNObservables();
    hsize_t nData = nObs * nEntries;

    // create colon separated string from list of observables
    std::vector<std::string> observableNames = dataSet_.GetObservableNames();
    if (observableNames.size() != nObs)
        throw HdfIOError("SaveDataSet::Require one name and one name only for each observable");
    

    // Set up files
    H5::H5File file(filename_, H5F_ACC_TRUNC);
    
    // Flatten data into 1D array
    // HDF5 likes c arrays. Here use a vector and pass pointer to first element 
    // memory guaranteed to be contiguous
    std::vector<double> flattenedData;
    std::vector<double> eventData;
    flattenedData.reserve(nData);
    for(size_t i = 0; i < nEntries; i++){
        eventData = dataSet_.GetEntry(i).GetData();
        flattenedData.insert(flattenedData.end(), eventData.begin(), eventData.end());
    }
    
    // Set up the data set
    // 1D, ndata long, called "observations". Saved as native doubles on this computer
    H5::DataSpace dataSpace(1, &nData);  
    H5::DataSet   theData(file.createDataSet("observations", H5::PredType::NATIVE_DOUBLE, dataSpace));
    
    //  Set up the attributes - the number of obs per event and the names of the observables
    //  64 chars max in str to save space
    H5::StrType   strType(H5::PredType::C_S1, 64);
    H5::DataSpace attSpace(H5S_SCALAR);
    H5::Attribute obsListAtt = theData.createAttribute("observed_quantities", strType, attSpace);
    obsListAtt.write(strType, FlattenStringVector(observableNames));

    H5::Attribute countAtt = theData.createAttribute("n_observables",
                                                     H5::PredType::NATIVE_INT,
                                                     attSpace);
    countAtt.write(H5::PredType::NATIVE_INT, &nObs);

    //  Write the data
    theData.write(&flattenedData.at(0), H5::PredType::NATIVE_DOUBLE);
    
}

OXSXDataSet
IO::LoadDataSet(const std::string& filename_){
    // Get Data Set
    H5::H5File  file(filename_, H5F_ACC_RDONLY);
    H5::DataSet dataSet = file.openDataSet("observations");
 
    // read meta information
    unsigned nObs = 0;
    H5::Attribute nameAtt  = dataSet.openAttribute("observed_quantities");
    H5::Attribute countAtt  = dataSet.openAttribute("n_observables");
    H5std_string strreadbuf("");
    nameAtt.read(nameAtt.getDataType(), strreadbuf);
    countAtt.read(countAtt.getDataType(), &nObs);

    
    
    // Read data out as 1D array
    hsize_t nData = 0;
    dataSet.getSpace().getSimpleExtentDims(&nData, NULL);
    size_t nEntries = nData/nObs;

    std::vector<double> flatData(nData, 0);
    dataSet.read(&flatData.at(0), H5::PredType::NATIVE_DOUBLE);

    // Assemble into an OXSX data set
    OXSXDataSet oxsxDataSet;

    // Set the variable names
    oxsxDataSet.SetObservableNames(UnpackString(strreadbuf));

    // then the data
    std::vector<double> oneEventObs(nObs, 0);
    for(size_t i = 0; i < nEntries; i++){
        for(size_t j = 0; j < nObs; j++)
            oneEventObs[j] = flatData.at(i * nObs + j);
        
        oxsxDataSet.AddEntry(EventData(oneEventObs));
    }
      
    return oxsxDataSet;
}

std::string
IO::FlattenStringVector(const std::vector<std::string>& vec_){
    std::string flatString;
    if(!vec_.size())
        return flatString;

    flatString += vec_.at(0);
    for(size_t i = 1; i < vec_.size(); i++)
        flatString +=  fDelimiter + vec_.at(i);

    return flatString;
}

std::vector<std::string>
IO::UnpackString(const std::string& str_){
    // count instances
    unsigned count = 0;
    for (size_t i = 0; i < str_.size(); i++)
        if(str_.at(i) == fDelimiter)
            count++;

    std::vector<std::string> strs(count + 1); // one more string than delimiter a:b
    size_t currentStr = 0;
    for(size_t i = 0; i < str_.size(); i++){
        if(str_[i] == fDelimiter)
            currentStr++;
        else
            strs[currentStr] += str_[i];
    }
    
    return strs;
}


/*void 
IO::SaveHistogram(const Histogram& histo_, const std::string& filename_){
    H5::H5File file(filename_, H5F_ACC_TRUNC);
    hsize_t nDims = histo_.GetNDims();
    hsize_t nBins = histo_.GetNBins();
    
    std
}
*/
