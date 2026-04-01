def selection_113():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,200.0,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([2.0,6.0,10.0,14.0,18.0,22.0,26.0,30.0,34.0,38.0,42.0,46.0,50.0,54.0,58.0,62.0,66.0,70.0,74.0,78.0,82.0,86.0,90.0,94.0,98.0,102.0,106.0,110.0,114.0,118.0,122.0,126.0,130.0,134.0,138.0,142.0,146.0,150.0,154.0,158.0,162.0,166.0,170.0,174.0,178.0,182.0,186.0,190.0,194.0,198.0])

    # Creating weights for histo: y114_M_0
    y114_M_0_weights = numpy.array([15.16977,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y114_M_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$M$ $[ j ]$ $(GeV/c^{2})$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ j$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y114_M_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y114_M_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_113.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_113.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_113.eps')

# Running!
if __name__ == '__main__':
    selection_113()
