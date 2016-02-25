#include <Pdf.h>
#include <CompositePdf.h>
#include <algorithm>
#include <iostream>
#include <PdfExceptions.h>
#include <DataExceptions.h>

Pdf::Pdf(const Pdf& other_){
    fDataRep = other_.fDataRep;
    fNDims = other_.fNDims;
}

unsigned Pdf::GetNDims() const{
    return fNDims;
}


double 
Pdf::Probability(const EventData& oberservations_) const{
    try{
        return operator()(oberservations_.ToRepresentation(fDataRep));
    }

    catch(const RepresentationError& e_){
        throw RepresentationError("Pdf::Probability() failed with  " + std::string(e_.what()) + " is the rep set correctly?");

    }
}

void
Pdf::SetDataRep(const DataRepresentation& rep_) {fDataRep = rep_;}

DataRepresentation
Pdf::GetDataRep() const {return fDataRep;}

/*
#include <boost/python.hpp>
void export_pdf() {
  // Abstract classes require the noncopyable descriptor
  boost::python::class_<Pdf, boost::noncopyable>("Pdf", boost::python::no_init)
    .def("get_n_dims", &Pdf::GetNDims)
    .def("set_data_rep", &Pdf::SetDataRep)
    .def("get_data_rep", &Pdf::GetDataRep);
}
*/
