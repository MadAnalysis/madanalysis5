def selection_156():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,6.4,33,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([0.1,0.30000000000000004,0.5,0.7000000000000001,0.9,1.1,1.3,1.5,1.7000000000000002,1.9000000000000001,2.1,2.3000000000000003,2.5,2.7,2.9000000000000004,3.1,3.3000000000000003,3.5,3.7,3.9000000000000004,4.1000000000000005,4.3,4.5,4.7,4.9,5.1000000000000005,5.300000000000001,5.5,5.7,5.9,6.1000000000000005,6.300000000000001])

    # Creating weights for histo: y157_DPHI_0_2PI_0
    y157_DPHI_0_2PI_0_weights = numpy.array([0.6797045,0.6797045,0.7723915,0.8959741,1.174035,1.467544,1.204931,1.374857,1.112244,1.143139,1.220379,0.9268698,0.7414958,0.9268698,0.6951523,0.6488089,0.5870175,0.8805263,0.8032872,0.8650785,1.0659,1.189483,1.112244,1.390305,1.004109,1.436648,1.359409,1.019557,0.8341828,0.726048,0.726048,0.2317174])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y157_DPHI_0_2PI_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\Delta\Phi_{0,2\pi}$ $[ j, j ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ (j, j)\ \mathrm{pairs}$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y157_DPHI_0_2PI_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y157_DPHI_0_2PI_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_156.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_156.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_156.eps')

# Running!
if __name__ == '__main__':
    selection_156()
