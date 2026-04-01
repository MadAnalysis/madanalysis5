def selection_3():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(0.0,0.02,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([0.0002,0.0006000000000000001,0.001,0.0014,0.0018000000000000002,0.0022,0.0026000000000000003,0.003,0.0034000000000000002,0.0038,0.004200000000000001,0.0046,0.005,0.0054,0.0058000000000000005,0.006200000000000001,0.0066,0.007,0.0074,0.0078000000000000005,0.0082,0.0086,0.009000000000000001,0.0094,0.0098,0.0102,0.0106,0.011000000000000001,0.0114,0.0118,0.0122,0.0126,0.013000000000000001,0.0134,0.013800000000000002,0.0142,0.0146,0.015000000000000001,0.0154,0.0158,0.0162,0.0166,0.017,0.017400000000000002,0.0178,0.0182,0.018600000000000002,0.019,0.0194,0.0198])

    # Creating weights for histo: y4_ALPHA_QED_0
    y4_ALPHA_QED_0_weights = numpy.array([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y4_ALPHA_QED_0_weights,\
             label="$testset$", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\alpha_{QED}$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{Events}$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y4_ALPHA_QED_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y4_ALPHA_QED_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_3.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_3.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_3.eps')

# Running!
if __name__ == '__main__':
    selection_3()
