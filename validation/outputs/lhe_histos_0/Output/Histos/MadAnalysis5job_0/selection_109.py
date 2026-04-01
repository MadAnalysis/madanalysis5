def selection_109():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,8.0,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([0.08,0.24,0.4,0.56,0.72,0.88,1.04,1.2,1.36,1.52,1.68,1.84,2.0,2.16,2.32,2.48,2.64,2.8000000000000003,2.96,3.12,3.2800000000000002,3.44,3.6,3.7600000000000002,3.92,4.08,4.24,4.4,4.5600000000000005,4.72,4.88,5.04,5.2,5.36,5.5200000000000005,5.68,5.84,6.0,6.16,6.32,6.48,6.640000000000001,6.8,6.96,7.12,7.28,7.44,7.6000000000000005,7.76,7.92])

    # Creating weights for histo: y110_ABSETA_0
    y110_ABSETA_0_weights = numpy.array([3.27494,3.228596,2.826953,2.718818,2.734266,2.56434,1.915531,2.054561,1.930979,1.668366,1.328513,1.174035,1.019557,0.6488089,0.5715697,0.3398523,0.3553001,0.2471653,0.1699261,0.04634349,0.06179132,0.01544783,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y110_ABSETA_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$|\eta|$ $[ j ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ j$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y110_ABSETA_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y110_ABSETA_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_109.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_109.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_109.eps')

# Running!
if __name__ == '__main__':
    selection_109()
