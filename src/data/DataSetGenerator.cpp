#include <DataSetGenerator.h>
#include <DataSet.h>
#include <math.h>
#include <EventData.h>
#include <stdlib.h>

void
DataSetGenerator::SetDataHandles(const std::vector<DataHandle*> handles_){
    fDataHandles = handles_;
}

void
DataSetGenerator::SetExpectedRates(const std::vector<double>& rates_){
    fExpectedRates = rates_;
}

DataSet
DataSetGenerator::ExpectedRatesDataSet() const{
    DataSet dataSet;
    for(size_t i = 0; i < fDataHandles.size(); i++){
        unsigned expectedCounts = round(fExpectedRates.at(i));
    for(unsigned j = 0; j < expectedCounts; j++)
        dataSet.AddEntry(RandomEvent(i));
    }
    return dataSet;
}

EventData
DataSetGenerator::RandomEvent(size_t handleIndex_) const{
    unsigned eventNum = (rand() % (int)(fDataHandles.at(handleIndex_)->GetNEntries() + 1));
    return fDataHandles.at(handleIndex_)->GetEntry(eventNum);
}