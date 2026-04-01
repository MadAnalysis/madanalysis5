def selection_8():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,18,19,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5,12.5,13.5,14.5,15.5,16.5,17.5])

    # Creating weights for histo: y9_NPID_0
    y9_NPID_0_weights = numpy.array([4.078227,3.553001,3.583897,4.232705,15.44783,3.692031,3.614792,4.093675,4.047331,4.093675,4.047331,3.692031,3.614792,15.44783,3.583897,4.232705,4.078227,3.553001])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y9_NPID_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$NPID$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ \mathrm{particles}$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y9_NPID_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y9_NPID_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Labels for x-Axis
    xLabels = numpy.array(["vm~","mu+","ve~","e+","b~","c~","s~","u~","d~","d","u","s","c","b","e-","ve","mu-","vm"])
    plt.xticks(xData, xLabels, rotation="vertical")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_8.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_8.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_8.eps')

# Running!
if __name__ == '__main__':
    selection_8()
