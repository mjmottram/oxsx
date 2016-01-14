import oxsx
import matplotlib.pyplot as plt

if __name__=="__main__":

    # Have to pass a custom list type for now 
    # (see http://stackoverflow.com/questions/15842126 )
    v1 = oxsx.DoubleList()
    v1[:] = [2, 1]
    v2 = oxsx.DoubleList()
    v2[:] = [2, 2]
    test_gaus = oxsx.Gaussian(v1, v2)

    axis1 = oxsx.PdfAxis("energy", 2, 3, 10, "Energy")
    axis2 = oxsx.PdfAxis("radius", 2, 3, 10, "rad")
    axes = oxsx.AxisCollection()
    axes.add_axis(axis1)
    axes.add_axis(axis2)

    test_pdf = oxsx.PdfConverter.to_binned_pdf(test_gaus, axes)

    gaus_pdf = oxsx.Gaussian(1, 2)
    conv = oxsx.Convolution()
    
    conv.set_pdf(gaus_pdf)
    conv.set_axes(axes)

    v1 = oxsx.SizetList()
    v1[:] = [0]
    smear_rep = oxsx.DataRepresentation(v1)
    v1[:] = [0, 1]
    pdf_rep = oxsx.DataRepresentation(v1)

    conv.set_data_rep(smear_rep)
    conv.set_pdf_data_rep(pdf_rep)
    
    conv.construct()

    transformed = conv(test_pdf)

    # Print and plot things
    # for i_bin in range(transformed.get_n_bins()):
    #     print "bin", i_bin, ":", transformed.get_bin_content(i_bin)

    # Still need to transform from DoubleList to an actual list...
    # Not sure how to extract the axis information.
    contents1 = []
    contents2 = []
    x1 = []
    x2 = []
    for i, v in enumerate(transformed.get_bin_contents()):
        contents1.append(v)
        x1.append(i)
    for i, v in enumerate(test_pdf.get_bin_contents()):
        contents2.append(v)
        x2.append(i)
#    plt.plot(x1, contents1, 'ro')
    plt.plot(x2, contents2, 'bo') 
    plt.show()
