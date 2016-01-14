/**************************************************************/
/* Static Utility class for doing a variety of pdfconversions */
/**************************************************************/
#ifndef __PDF_CONVERTER__
#define __PDF_CONVERTER__
//#include <TH1D.h>
#include <vector>

class BinnedPdf;
class IntegrablePdf;
class AxisCollection;
class Histogram;

class PdfConverter{
 public:
  PdfConverter() {};
  ~PdfConverter() {};
    
    static BinnedPdf ToBinnedPdf(const IntegrablePdf&, const AxisCollection& axes_);
    //    static TH1D      ToTH1D(const BinnedPdf&);
    //    static TH1D      ToTH1D(const Histogram&);
    
    // 2nd arg is the vec of (data) indicies to keep (not to project out)
    static BinnedPdf Marginalise(const BinnedPdf& binnedPdf_, const std::vector<size_t>& indices_);
    static Histogram Marginalise(const Histogram& binnedPdf_, const std::vector<size_t>& indices_);
    static BinnedPdf Marginalise(const BinnedPdf& binnedPdf_, size_t index_);
};
#endif
