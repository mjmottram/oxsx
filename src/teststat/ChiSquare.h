/********************************************************************/
/* A routine to calculate a binned chi squared test.                */
/* NOTE: this can only make sense if all pdfs have the same binning */
/********************************************************************/

#ifndef __OXSX_CHI_SQUARED__
#define __OXSX_CHI_SQUARED__
#include <SystematicManager.h>
#include <BinnedPdfManager.h>
#include <TestStatistic.h>
#include <ComponentManager.h>

//FIXME::Enforce equal binning on the pdfs
class DataSet;
class ChiSquare : public TestStatistic{
 public:
    ChiSquare() : fCalculatedDataPdf(false) {}

    void SetDataSet(DataSet* d);
    DataSet* GetDataSet();
   
    // Fit Component interface
    double Evaluate();
    void   RegisterFitComponents();
    int    GetParameterCount() const;
    void   SetParameters(const std::vector<double>& params_);
    std::vector<double> GetParameters() const;
    std::vector<std::string> GetParameterNames() const;
    
 private:
    bool              fCalculatedDataPdf;
    BinnedPdf         fDataPdf;
    void              BinData();

    DataSet*          fDataSet;
    BinnedPdfManager  fPdfManager;
    SystematicManager fSystematicManager;
    ComponentManager  fComponentManager;
};
#endif
