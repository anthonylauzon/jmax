product jmax
    id "jMax, Java MAX"
    image sw
        id "jMax environment"
        version "2.4.9e"
        order 1
        subsys exec default
            id "jMax environment"
            replaces self
            exp jmax.sw.exec
        endsubsys
    endimage
endproduct
