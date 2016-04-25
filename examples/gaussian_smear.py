import oxsx
import matplotlib.pyplot as plt

if __name__=="__main__":

    # Have to pass a custom list type for now 
    # (see http://stackoverflow.com/questions/15842126 )
    v1 = [1.0, 1.0]
    v2 = [2.0, 2.0]
    test_gaus = oxsx.Gaussian([1.0, 1.0], [2.0, 2.0])

    axis1 = oxsx.PdfAxis("energy", 2, 3, 10, "Energy")
    axis2 = oxsx.PdfAxis("radius", 2, 3, 10, "rad")
    axes = oxsx.AxisCollection()
    axes.add_axis(axis1)
    axes.add_axis(axis2)

    test_pdf = oxsx.PdfConverter.to_binned_pdf(test_gaus, axes)

    gaus_pdf = oxsx.Gaussian([1], [2]) # Must always pass lists
    conv = oxsx.Convolution()
    
    conv.set_function(gaus_pdf)
    conv.set_axes(axes)

    smear_rep = oxsx.DataRepresentation([0])
    pdf_rep = oxsx.DataRepresentation([0, 1])

    conv.set_data_rep(smear_rep)
    conv.set_pdf_data_rep(pdf_rep)
    
    conv.construct()

    transformed = conv(test_pdf)

    # Still need to transform from DoubleList to an actual list...
    # Not sure how to extract the axis information.
    contents = test_pdf.get_bin_contents()
    plt.plot(range(len(contents)), contents, 'bo') 
    plt.show()
