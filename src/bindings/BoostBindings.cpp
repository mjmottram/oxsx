#include <boost/python.hpp>
#include <Pdf.h>
#include <IntegrablePdf.h>
#include <BinnedPdf.h>
#include <DataRepresentation.h>
#include <Gaussian.h>
#include <AxisCollection.h>
#include <PdfAxis.h>
#include <PdfConverter.h>
#include <Systematic.h>
#include <Convolution.h>

typedef std::vector<double> DoubleList;
typedef std::vector<size_t> SizetList;

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

BOOST_PYTHON_MODULE(oxsx)
{

  using namespace boost::python;

  // See http://stackoverflow.com/questions/15842126 
  // for a better long term solution than defining list classes
  // the solution is more verbose on the binding side
  // but allows a more intuitive python interface (pass lists directly)
  class_<DoubleList>("DoubleList")
    .def(vector_indexing_suite<DoubleList>() );
  class_<SizetList>("SizetList")
    .def(vector_indexing_suite<SizetList>() );

  class_<AxisCollection>("AxisCollection")
    .def("add_axis", &AxisCollection::AddAxis);

  class_<PdfAxis>("PdfAxis", init<std::string, double, double, size_t, std::string>());

  // noncopyable appears necessary when a class includes virtual methods
  // (or at least when pure virtual methods exist)
  class_<Pdf, boost::noncopyable>("Pdf", no_init)
    .def("get_n_dims", &Pdf::GetNDims)
    .def("set_data_rep", &Pdf::SetDataRep)
    .def("get_data_rep", &Pdf::GetDataRep);

  class_<IntegrablePdf, boost::noncopyable, bases<Pdf> >("IntegrablePdf", no_init);

  class_<BinnedPdf, bases<Pdf> >("BinnedPdf", no_init)
    .def("get_n_bins", &BinnedPdf::GetNBins)
    .def("get_bin_content", &BinnedPdf::GetBinContent)
    .def("get_bin_contents", &BinnedPdf::GetBinContents);

  class_<Gaussian, bases<IntegrablePdf> >("Gaussian", init<int>())
    .def(init<double, double>())
    .def(init<DoubleList, DoubleList>())
    .def("get_mean", &Gaussian::GetMean);

  class_<PdfConverter>("PdfConverter", init<>())
    .def("to_binned_pdf", &PdfConverter::ToBinnedPdf)
    .staticmethod("to_binned_pdf");

  class_<DataRepresentation>("DataRepresentation", init<>())
    .def(init<SizetList>());
 
  class_<Systematic>("Systematic", init<>())
    .def("set_data_rep", &Systematic::SetDataRep)
    .def("set_pdf_data_rep", &Systematic::SetPdfDataRep)
    .def("__call__", &Systematic::operator()); // not sure why the brackets are needed

  class_<Convolution, bases<Systematic> >("Convolution", init<>())
    .def("set_pdf", &Convolution::SetPdf)
    .def("set_axes", &Convolution::SetAxes)
    .def("construct", &Convolution::Construct);

}
