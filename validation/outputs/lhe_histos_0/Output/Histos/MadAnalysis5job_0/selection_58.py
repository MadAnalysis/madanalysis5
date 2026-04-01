def selection_58():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,1.2,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([0.012,0.036000000000000004,0.06,0.084,0.108,0.132,0.156,0.18,0.20400000000000001,0.228,0.252,0.276,0.3,0.324,0.34800000000000003,0.372,0.396,0.42,0.444,0.468,0.492,0.516,0.54,0.5640000000000001,0.588,0.612,0.636,0.66,0.684,0.708,0.732,0.756,0.78,0.804,0.8280000000000001,0.852,0.876,0.9,0.924,0.9480000000000001,0.972,0.996,1.02,1.044,1.068,1.092,1.116,1.1400000000000001,1.164,1.188])

    # Creating weights for histo: y59_BETA_0
    y59_BETA_0_weights = numpy.array([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,7.631228,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y59_BETA_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\beta$ $[ mu ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ mu$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y59_BETA_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y59_BETA_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_58.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_58.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_58.eps')

# Running!
if __name__ == '__main__':
    selection_58()
