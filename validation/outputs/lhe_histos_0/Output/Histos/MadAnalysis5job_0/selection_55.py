def selection_55():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(-2000.0,2000.0,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([-1960.0,-1880.0,-1800.0,-1720.0,-1640.0,-1560.0,-1480.0,-1400.0,-1320.0,-1240.0,-1160.0,-1080.0,-1000.0,-920.0,-840.0,-760.0,-680.0,-600.0,-520.0,-440.0,-360.0,-280.0,-200.0,-120.0,-40.0,40.0,120.0,200.0,280.0,360.0,440.0,520.0,600.0,680.0,760.0,840.0,920.0,1000.0,1080.0,1160.0,1240.0,1320.0,1400.0,1480.0,1560.0,1640.0,1720.0,1800.0,1880.0,1960.0])

    # Creating weights for histo: y56_PZ_0
    y56_PZ_0_weights = numpy.array([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.01544783,0.01544783,0.01544783,0.06179132,0.2780609,0.540674,2.626131,3.151357,0.5561219,0.2935088,0.07723915,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y56_PZ_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$p_{z}$ $[ mu ]$ $(GeV/c)$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ mu$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y56_PZ_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y56_PZ_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_55.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_55.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_55.eps')

# Running!
if __name__ == '__main__':
    selection_55()
