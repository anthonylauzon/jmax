product jmax
    id "jMax, Java MAX"
    image sw
        id "Software"
        version 2.3.8a
        order 9999
        subsys exec default
            id "jMax executables and libraries"
            replaces self
            exp jmax.sw.exec
        endsubsys
        subsys includes default
            id "jMax includes"
            replaces self
            exp jmax.sw.includes
        endsubsys
    endimage
    image doc
        id "jMax doc"
        version 2.4.8a
        order 1
        subsys documentation default
            id "jMax Documentation"
            replaces self
            exp jmax.doc.documentation
        endsubsys
    endimage
endproduct
