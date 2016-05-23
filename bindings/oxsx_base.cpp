
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <FitComponent.h>
#include <FitParameter.h>

#include <Function.h>
#include <IntegrableFunction.h>
#include <Gaussian.h>
//#include <Heaviside.h>

#include <AxisCollection.h>
#include <Histogram.h>
#include <PdfAxis.h>
#include <BoolCut.h>
#include <BoxCut.h>
#include <Cut.h>
#include <CutCollection.h>
#include <LineCut.h>
#include <DataRepresentation.h>
#include <DataSet.h>
#include <DataSetGenerator.h>
#include <EventData.h>
#include <IO.h>
#include <OXSXDataSet.h>
#include <ROOTNtuple.h>
#include <BinnedPdfManager.h>
#include <BinnedPdfShrinker.h>
#include <PdfManager.h>
#include <SystematicManager.h>
#include <BayesIntervalCalc.h>
#include <FitResult.h>
#include <GridSearch.h>
#include <MetropolisHastings.h>
#include <Minuit.h>
#include <Optimiser.h>
#include <AnalyticPdf.h>
#include <CompositePdf.h>
#include <Pdf.h>
#include <PdfConverter.h>
#include <BinnedPdf.h>
#include <PdfMapping.h>
#include <SpectralFitPdf.h>
#include <Rand.h>
#include <Convolution.h>
#include <Scale.h>
#include <Systematic.h>
#include <BinnedNLLH.h>
#include <ChiSquare.h>
#include <QuadraticConstraint.h>
#include <TestStatistic.h>

#include <TH2D.h>
#include <TH1D.h>

// Taken directly from http://stackoverflow.com/questions/15842126 
// See comments in main BOOST_PYTHON function.
struct iterable_converter
{
  template <typename Container>
  iterable_converter&
  from_python()
  {
    boost::python::converter::registry::push_back(
      &iterable_converter::convertible,
      &iterable_converter::construct<Container>,
      boost::python::type_id<Container>());

    // Support chaining.
    return *this;
  }

/// @brief Check if PyObject is iterable.
  static void* convertible(PyObject* object)
  {
    return PyObject_GetIter(object) ? object : NULL;
  }

  /// @brief Convert iterable PyObject to C++ container type.
  ///
  /// Container Concept requirements:
  ///
  ///   * Container::value_type is CopyConstructable.
  ///   * Container can be constructed and populated with two iterators.
  ///     I.e. Container(begin, end)
  template <typename Container>
  static void construct(
    PyObject* object,
    boost::python::converter::rvalue_from_python_stage1_data* data)
  {

    namespace python = boost::python;
    // Object is a borrowed reference, so create a handle indicting it is
    // borrowed for proper reference counting.
    python::handle<> handle(python::borrowed(object));

    // Obtain a handle to the memory block that the converter has allocated
    // for the C++ type.
    typedef python::converter::rvalue_from_python_storage<Container>
                                                                storage_type;
    void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

    typedef python::stl_input_iterator<typename Container::value_type>
                                                                    iterator;

    // Allocate the C++ type into the converter's memory block, and assign
    // its handle to the converter's convertible variable.  The C++
    // container is populated by passing the begin and end iterators of
    // the python object to the container's constructor.
    new (storage) Container(
      iterator(python::object(handle)), // begin
      iterator());                      // end
    data->convertible = storage;
  }
};


// from http://stackoverflow.com/questions/5314319/how-to-export-stdvector
template<class T>
struct VectorToList
{
  static PyObject* convert(const std::vector<T>& vec)
  {
    boost::python::list* list = new boost::python::list();
    for(size_t i = 0; i < vec.size(); i++)
      (*list).append(vec[i]);
    return list->ptr();
  }
};


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(BinnedPdfFillOverloads, BinnedPdf::Fill, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(RandUniformOverloads, Rand::Uniform, 0, 1);
BOOST_PYTHON_FUNCTION_OVERLOADS(RandGausOverloads, Rand::Gaus, 0, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(RandSetSeedOverloads, Rand::SetSeed, 0, 1);


BOOST_PYTHON_MODULE(oxsx)
{

  using namespace boost::python;

  // See http://stackoverflow.com/questions/15842126 
  // for a better long term solution than defining list classes
  // the solution is more verbose on the binding side
  // but allows a more intuitive python interface (pass lists directly)
  // Also appear to have issues when using this solution with return
  // values of vectors.

  // To convert list arguments TO vectors
  iterable_converter()
    .from_python<std::vector<double> >()
    .from_python<std::vector<size_t> >();

  // To convert vector returns TO lists
  // Alternative is to create custom classes for python lists.
  // But this requires the custom classes, which users may not expect.
  to_python_converter<std::vector<double, class::std::allocator<double> >, VectorToList<double> > ();
  to_python_converter<std::vector<size_t, class::std::allocator<size_t> >, VectorToList<size_t> > ();

#include <core/FitComponent.wrap>
#include <core/FitParameter.wrap>

#include <core/function/Function.wrap>
#include <core/function/IntegrableFunction.wrap>
#include <core/function/Gaussian.wrap>
  //#include <core/function/Heaviside.wrap> // not included yet as pure virtual methods of base not implemented

#include <core/histogram/PdfAxis.wrap>
#include <core/histogram/AxisCollection.wrap>
#include <core/histogram/Histogram.wrap>

#include <cut/Cut.wrap>
#include <cut/BoolCut.wrap>
#include <cut/BoxCut.wrap>
#include <cut/CutCollection.wrap>
#include <cut/LineCut.wrap>

#include <data/DataRepresentation.wrap>
#include <data/DataSet.wrap>
#include <data/DataSetGenerator.wrap>
#include <data/EventData.wrap>
#include <data/ROOTNtuple.wrap>
#include <data/IO.wrap>
#include <data/OXSXDataSet.wrap>

#include <fitutil/BinnedPdfManager.wrap>
#include <fitutil/BinnedPdfShrinker.wrap>
#include <fitutil/PdfManager.wrap>
#include <fitutil/SystematicManager.wrap>

#include <interval/BayesIntervalCalc.wrap>

#include <optimise/Optimiser.wrap>
#include <optimise/FitResult.wrap>
#include <optimise/GridSearch.wrap>
#include <optimise/MetropolisHastings.wrap>
#include <optimise/Minuit.wrap>

#include <pdf/Pdf.wrap>
#include <pdf/AnalyticPdf.wrap>
#include <pdf/CompositePdf.wrap>
#include <pdf/PdfConverter.wrap>
#include <pdf/binned/BinnedPdf.wrap>
#include <pdf/binned/PdfMapping.wrap>
#include <pdf/binned/SpectralFitPdf.wrap>

#include <rand/Rand.wrap>

#include <systematic/Systematic.wrap>
#include <systematic/Convolution.wrap>
#include <systematic/Scale.wrap>

  
#include <teststat/TestStatistic.wrap>
#include <teststat/QuadraticConstraint.wrap>
#include <teststat/BinnedNLLH.wrap>
#include <teststat/ChiSquare.wrap>

}
