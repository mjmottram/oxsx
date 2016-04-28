#ifndef __BinnedNLLH__
#define __BinnedNLLH__
#include <TestStatistic.h>
#include <BinnedPdfManager.h>
#include <SystematicManager.h>
#include <BinnedPdfShrinker.h>
#include <vector>
#include <QuadraticConstraint.h>
#include <ComponentManager.h>
#include <DataSet.h>

class DataSet;
class BinnedNLLH : public TestStatistic{
 public:
    BinnedNLLH() : fCalculatedDataPdf(false), fDataSet(NULL) {}
    ~BinnedNLLH(){}

    void   SetPdfManager(const BinnedPdfManager&);
    void   SetSystematicManager(const SystematicManager&);

    void   AddPdf(const BinnedPdf&);
    void   AddSystematic(Systematic*);

    void   AddPdfs(const std::vector<BinnedPdf>&);
    void   AddSystematics(const std::vector<Systematic*>);

    void AddSystematicConstraint(const QuadraticConstraint& constr_) {}; // not implemented
    void AddNormalisationConstraint(const QuadraticConstraint& constr_);

    void SetSystematicConstraint(size_t index_, const QuadraticConstraint& constr_);
    void SetNormalisationConstraint(size_t index_, const QuadraticConstraint& constr_);

    QuadraticConstraint GetSystematicConstraint(size_t index_)    const;
    QuadraticConstraint GetNormalisationConstraint(size_t index_)  const;

    void SetNormalisations(const std::vector<double>& norms_);
    std::vector<double> GetNormalisations() const;

    void  BinData();

    void SetDataPdf(const BinnedPdf&);
    BinnedPdf GetDataPdf() const;

    void SetDataSet(DataSet&);
    void SetDataSet(DataSet*);
    DataSet* GetDataSet();

    void SetBuffer(size_t dim_, unsigned lower_, unsigned upper_);
    std::pair<unsigned, unsigned> GetBuffer(size_t dim_) const;
    void SetBufferAsOverflow(bool b_); // true by default
    bool GetBufferAsOverflow() const;
    
    // Test statistic interface
    void RegisterFitComponents(); 
    void SetParameters(const std::vector<double>&);
    std::vector<double> GetParameters() const;
    int  GetParameterCount() const;
    double Evaluate();
    std::vector<std::string> GetParameterNames() const;

 private:
    BinnedPdfManager  fPdfManager;
    SystematicManager fSystematicManager;
    BinnedPdfShrinker fPdfShrinker;
    DataSet* fDataSet;
    
    std::vector<QuadraticConstraint> fSystematicConstraints;
    std::vector<QuadraticConstraint> fNormalisationConstraints;
    
    BinnedPdf fDataPdf;
    bool      fCalculatedDataPdf;
    ComponentManager fComponentManager;    
};
#endif
