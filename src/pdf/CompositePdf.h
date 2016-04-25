/***************************************************************************************************/
/* The outer product of two pdfs is also a pdf composite = pdf1 * pdf2                             */
/* This implemented here using recursion, so any level of composition can be used.                 */
/* This class owns its own copies of constituent pdfs                                              */
/* Calls to operator() and Probability() are forwarded to the constituent pdfs, in the latter case */
/* each will know where to look from its data representation                                       */
/***************************************************************************************************/

#ifndef __COMPOSITE_PDF__
#define __COMPOSITE_PDF__
#include <Pdf.h>
#include <vector>
class EventData;

class CompositePdf : public Pdf{
 public:
    CompositePdf(const Pdf* p1_, const Pdf* p2_);
    CompositePdf(const std::vector<Pdf*>& pdfs_); 
    virtual ~CompositePdf();

    virtual double operator() (const std::vector<double>& vals_) const;
    virtual double Probability(const EventData&) const;
    virtual Pdf*   Clone() const; // required for futher compositions

    virtual double Integral()  const;
    virtual void   Normalise();
    unsigned GetNDims() const;

 private:
    
    std::vector<Pdf*> fPdfPtrs;
};

CompositePdf operator * (const Pdf&, const Pdf&);

#endif
