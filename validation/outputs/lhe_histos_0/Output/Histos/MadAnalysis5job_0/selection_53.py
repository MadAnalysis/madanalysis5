def selection_53():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(-1000.0,1000.0,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([-980.0,-940.0,-900.0,-860.0,-820.0,-780.0,-740.0,-700.0,-660.0,-620.0,-580.0,-540.0,-500.0,-460.0,-420.0,-380.0,-340.0,-300.0,-260.0,-220.0,-180.0,-140.0,-100.0,-60.0,-20.0,20.0,60.0,100.0,140.0,180.0,220.0,260.0,300.0,340.0,380.0,420.0,460.0,500.0,540.0,580.0,620.0,660.0,700.0,740.0,780.0,820.0,860.0,900.0,940.0,980.0])

    # Creating weights for histo: y54_PX_0
    y54_PX_0_weights = numpy.array([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.03089566,0.0,0.1544783,0.7106002,2.888744,2.718818,0.8805263,0.1699261,0.04634349,0.03089566,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y54_PX_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$p_{x}$ $[ mu ]$ $(GeV/c)$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ mu$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y54_PX_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y54_PX_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_53.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_53.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_53.eps')

# Running!
if __name__ == '__main__':
    selection_53()
