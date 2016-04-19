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

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

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
    // To overload would require casting the vector, awkward
    // Maybe still pythonic to just force users to use single element lists
    // http://stackoverflow.com/questions/8140155/boost-python-confused-about-similar-constructor
    .def(init< std::vector<double>, std::vector<double> >())
    .def("get_mean", &Gaussian::GetMean);

  class_<PdfConverter>("PdfConverter", init<>())
    .def("to_binned_pdf", &PdfConverter::ToBinnedPdf)
    .staticmethod("to_binned_pdf");

  class_<DataRepresentation>("DataRepresentation", init<>())
    .def(init< std::vector<size_t> >());
 
  class_<Systematic>("Systematic", init<>())
    .def("set_data_rep", &Systematic::SetDataRep)
    .def("set_pdf_data_rep", &Systematic::SetPdfDataRep)
    .def("__call__", &Systematic::operator()); // not sure why the brackets are needed

  class_<Convolution, bases<Systematic> >("Convolution", init<>())
    .def("set_pdf", &Convolution::SetPdf)
    .def("set_axes", &Convolution::SetAxes)
    .def("construct", &Convolution::Construct);

}
