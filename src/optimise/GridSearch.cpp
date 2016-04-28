#include <GridSearch.h>
#include <TestStatistic.h>
#include <iostream>
#include <sstream>
#include <Histogram.h>
#include <FitResult.h>

void 
GridSearch::SetMinima(const std::vector<double>& minima_){
    fMinima = minima_;
}

void 
GridSearch::SetMaxima(const std::vector<double>& maxima_){
    fMaxima = maxima_;
}

void
GridSearch::SetStepSizes(const std::vector<double>& steps_){
    fStepSizes = steps_;
}

std::vector<double> 
GridSearch::GetMaxima() const{
    return fMaxima;
}

std::vector<double> 
GridSearch::GetMinima() const{
    return fMinima;
}

std::vector<double>
GridSearch::GetStepSizes() const{
    return fStepSizes;
}

const FitResult&
GridSearch::Optimise(TestStatistic* testStat_){
    // list of rates followed by list of systematics
    testStat_->RegisterFitComponents();
    std::vector<double> bestFit;
    bestFit.resize(testStat_ -> GetParameterCount());
    fMinVal = 0;

    // Initialise LH space and count the number of grid steps
    unsigned maxSteps = 1;
    std::vector<double> gridCounts;

    for(size_t i = 0; i < fMinima.size(); i++){
        size_t axisCounts = 1 + static_cast<size_t>((fMaxima.at(i) - fMinima.at(i)) / fStepSizes.at(i));
        maxSteps *= axisCounts;
        gridCounts.push_back(axisCounts);
    }
    
    // start at min value
    fParams = fMinima;
    
    // count interations
    unsigned stepCount = 0;
    
    while(Increment(0)){
        // calculate the new value
        // if bigger, grab this as new best fit

        if(!(stepCount++ %1000000)){
            std::cout << stepCount << " / " << maxSteps  << "\t"
                      <<  100 * double(stepCount)/maxSteps 
                      << "%" << std::endl;
        }

        testStat_ -> SetParameters(fParams);
        double currentVal = testStat_ -> Evaluate();

        // if maximising, take the negative of the test stat
        if(fMaximising)
            currentVal *= -1;

        if (currentVal < fMinVal || !fMinVal){
                fMinVal = currentVal;
                bestFit = fParams;
        }
       
    } 
    fFitResult.SetBestFit(bestFit);
    fFitResult.SetParameterNames(testStat_->GetParameterNames());
    return fFitResult;
}


bool 
GridSearch::Increment(size_t index_){
    fParams[index_] += fStepSizes.at(index_);

    // wrap around past the maximum
    if (fParams[index_] > fMaxima.at(index_)){
        fParams[index_] = fMinima.at(index_);

        // if its the last index no rippling to do
        if (index_ == fStepSizes.size() - 1)
            return false;

        // ripple up to next index
        if (Increment(index_ + 1)) // propagate the false down
            return true;
        else
            return false;

    }
    return true;
}

FitResult
GridSearch::GetFitResult() const{
    return fFitResult;
}

bool
GridSearch::GetMaximising() const{
    return fMaximising;
}

void
GridSearch::SetMaximising(bool b_){
    fMaximising = b_;
}
