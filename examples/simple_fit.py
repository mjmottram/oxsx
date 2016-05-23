import argparse
import oxsx

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--treename", type=str, default="output", help="Ntuple tree name")
    parser.add_argument("background", type=str, help="background filename")
    parser.add_argument("signal", type=str, help="signal filename")
    parser.add_argument("data", type=str, help="data filename")
    args = parser.parse_args()

    ###
    # Set up PDFs
    ###

    # Set up binning
    axes = oxsx.AxisCollection()
    axes.add_axis(oxsx.PdfAxis("energy", 2, 3, 10, "Energy"))

    # Only interested in first bit of data ntuple
    data_rep = oxsx.DataRepresentation(0)

    # Set up pdf with these bins in this observable
    bg_pdf = oxsx.BinnedPdf(axes)
    signal_pdf = oxsx.BinnedPdf(axes)
    bg_pdf.set_data_rep(data_rep)
    signal_pdf.set_data_rep(data_rep)

    print "Initialised Pdfs"

    ###
    # Fill with data and normalise
    ###

    bg_mc = oxsx.ROOTNtuple(args.background, args.treename)
    signal_mc = oxsx.ROOTNtuple(args.signal, args.treename)

    for i in range(0, bg_mc.get_n_entries()):
        bg_pdf.fill(bg_mc.get_entry(i))
    for i in range(0, signal_mc.get_n_entries()):
        signal_pdf.fill(signal_mc.get_entry(i))

    bg_pdf.normalise()
    signal_pdf.normalise()

    print "Filled Pdfs"

    ###
    # Set up LH function
    ###

    data_ntuple = oxsx.ROOTNtuple(args.data, args.treename)
    lh_function = oxsx.BinnedNLLH()
    lh_function.set_data_set(data_ntuple)
    lh_function.add_pdf(bg_pdf)
    lh_function.add_pdf(signal_pdf)

    print "Built LH function"

    search = oxsx.GridSearch()
    
    search.set_minima([1, 1])
    search.set_maxima([1000, 1000])
    search.set_step_sizes([1, 1])

    ###
    # Fit
    ###
    
    result = search.optimise(lh_function)

    fit = result.get_best_fit()
    result.print_result()
    result.save_as("simple_fit_result.txt")
