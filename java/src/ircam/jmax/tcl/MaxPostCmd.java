/*
 * MaxPostCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.tcl;

import ircam.jmax.*;
import cornell.Jacl.*;
import java.io.*;
import java.util.*;

/**
 * The "post" TCL command in ERMES.
 */
class MaxPostCmd implements Command {

    /**
     * This procedure is invoked to execute a post from the shell
     */
    public Object CmdProc(Interp interp, CmdArgs ca) {
    
	if (ca.argc < 2) {	
            throw new EvalException("wrong # args: should be \"" + ca.argv(0) + "args...");
        }
    
    String temp = new String();
    
    //create the string to print
    for (int i = 1; i<ca.argc; i++) {
    	temp += ca.argv(i) + " ";	
    }
    
    MaxApplication.getApplication().getPostStream().println(temp);

    return temp;
    }
}

