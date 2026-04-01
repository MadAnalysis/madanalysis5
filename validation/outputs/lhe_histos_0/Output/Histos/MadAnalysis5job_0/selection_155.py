def selection_155():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,3.2,33,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([0.05,0.15000000000000002,0.25,0.35000000000000003,0.45,0.55,0.65,0.75,0.8500000000000001,0.9500000000000001,1.05,1.1500000000000001,1.25,1.35,1.4500000000000002,1.55,1.6500000000000001,1.75,1.85,1.9500000000000002,2.0500000000000003,2.15,2.25,2.35,2.45,2.5500000000000003,2.6500000000000004,2.75,2.85,2.95,3.0500000000000003,3.1500000000000004])

    # Creating weights for histo: y156_DPHI_0_PI_0
    y156_DPHI_0_PI_0_weights = numpy.array([0.5870175,0.7723915,0.7106002,0.6488089,0.8032872,0.7414958,0.8650785,0.9268698,1.204931,1.143139,1.575679,1.359409,1.575679,0.8341828,1.204931,1.544783,1.143139,1.081348,1.112244,1.174035,1.297618,1.143139,0.9886611,0.8650785,0.7106002,0.7723915,0.9577655,0.8959741,0.6797045,0.7106002,0.4634349,0.4016436])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y156_DPHI_0_PI_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\Delta\Phi_{0,\pi}$ $[ j, j ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ (j, j)\ \mathrm{pairs}$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y156_DPHI_0_PI_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y156_DPHI_0_PI_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_155.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_155.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_155.eps')

# Running!
if __name__ == '__main__':
    selection_155()
