product jmax
    id "jMax, Java MAX"
    image sw
        id "Software"
        version 2.3.8a
        order 9999
        subsys binaries default
            id "jMax binaries and libraries"
            replaces self
            exp jmax.sw.binaries
        endsubsys
    endimage
    image man
        id "Man Pages"
        version 1
        order 9999
        subsys manpages
            id "Man Pages"
            replaces self
            exp jmax.man.manpages
        endsubsys
        subsys relnotes
            id "Release Notes"
            replaces self
            exp jmax.man.relnotes
        endsubsys
    endimage
endproduct
