product jmax
    id "jMax, Java MAX"
    cutpoint /usr/lib/jmax
    image sw
        id "jMax environment"
        version "2.5.2"
        order 1
        subsys exec default
            id "jMax environment"
            replaces self
            exp ALL
        endsubsys
    endimage
endproduct
