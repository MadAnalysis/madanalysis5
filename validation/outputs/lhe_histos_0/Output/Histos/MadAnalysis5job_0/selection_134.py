def selection_134():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,3000.0,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([30.0,90.0,150.0,210.0,270.0,330.0,390.0,450.0,510.0,570.0,630.0,690.0,750.0,810.0,870.0,930.0,990.0,1050.0,1110.0,1170.0,1230.0,1290.0,1350.0,1410.0,1470.0,1530.0,1590.0,1650.0,1710.0,1770.0,1830.0,1890.0,1950.0,2010.0,2070.0,2130.0,2190.0,2250.0,2310.0,2370.0,2430.0,2490.0,2550.0,2610.0,2670.0,2730.0,2790.0,2850.0,2910.0,2970.0])

    # Creating weights for histo: y135_M_0
    y135_M_0_weights = numpy.array([0.07723915,15.37059,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y135_M_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$M$ $[ j_{1} j_{2} ]$ $(GeV/c^{2})$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{Events}$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y135_M_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y135_M_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_134.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_134.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_134.eps')

# Running!
if __name__ == '__main__':
    selection_134()
