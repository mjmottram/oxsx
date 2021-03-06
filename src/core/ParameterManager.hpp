#include <ContainerParameter.h>
#include <sstream>

template<typename Container> 
void  
ParameterManager::AddContainer(Container& cntr_, 
                               const std::string& sharedName_){
    std::stringstream ss;
    for(size_t i = 0; i < cntr_.size(); i++){
        ss << sharedName_ << " " << i;
        Add(new ContainerParameter<Container>(cntr_, i)
            , ss.str());
        ss.str("");
    }
}
