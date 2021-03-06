/******************************************************************/
/* Abstract base for a class that hands out data from some source */
/******************************************************************/
#ifndef __OXSX_DATA_SET__
#define __OXSX_DATA_SET__
#include <stddef.h>
#include <vector>
#include <string>

class DataRepresentation;
class EventData;
class DataSet{
 public:
    virtual EventData GetEntry(size_t iEvent_) const = 0;
    virtual ~DataSet() {}

    virtual unsigned GetNEntries() const = 0; 
    virtual unsigned GetNObservables() const = 0; 

    virtual std::vector<std::string> GetObservableNames() const = 0;
    DataRepresentation MakeDataRep(const std::vector<std::string> observableNames_) const;

};
#endif
