def selection_89():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(-8.0,8.0,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([-7.84,-7.52,-7.2,-6.88,-6.5600000000000005,-6.24,-5.92,-5.6,-5.279999999999999,-4.96,-4.640000000000001,-4.32,-4.0,-3.6799999999999997,-3.3600000000000003,-3.04,-2.7199999999999998,-2.3999999999999995,-2.08,-1.7599999999999998,-1.4399999999999995,-1.12,-0.7999999999999998,-0.47999999999999954,-0.16000000000000014,0.16000000000000014,0.4800000000000004,0.8000000000000007,1.120000000000001,1.4399999999999995,1.7599999999999998,2.08,2.4000000000000004,2.7200000000000006,3.040000000000001,3.3599999999999994,3.6799999999999997,4.0,4.32,4.640000000000001,4.960000000000001,5.280000000000001,5.6,5.92,6.24,6.5600000000000005,6.880000000000001,7.200000000000001,7.52,7.84])

    # Creating weights for histo: y90_Y_0
    y90_Y_0_weights = numpy.array([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.01544783,0.04634349,0.06179132,0.2008218,0.3861957,0.3244044,0.4634349,0.8032872,1.035005,1.328513,1.189483,1.498439,1.544783,1.405753,1.544783,1.0659,0.7878393,0.7106002,0.3861957,0.3089566,0.2008218,0.1081348,0.03089566,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y90_Y_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$y$ $[ b ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ b$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y90_Y_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y90_Y_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_89.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_89.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_89.eps')

# Running!
if __name__ == '__main__':
    selection_89()
