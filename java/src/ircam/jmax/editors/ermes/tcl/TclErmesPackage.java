/*
 * ErmesPackage.java --
 *
 */

package ircam.jmax.editors.ermes.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclErmesPackage extends cornell.Jacl.Package 
{
  /**
   * Default constructor. Create the built-in commands and load the
   * start-up scripts (ToDo).
   */
  public TclErmesPackage(Interp interp) {
    super(interp);
    
    /* Ermes commands*/
    
    /* patcher commands */
    interp.CreateCommand("PatNew", new ErmesPatNewCmd());
    
    /* object commands */
    interp.CreateCommand("ObjNew", new ErmesObjNewObjCmd());
    interp.CreateCommand("ObjConnect", new ErmesConnectCmd());
    interp.CreateCommand("ObjDisconnect", new ErmesDisconnectCmd());
    
    interp.CreateCommand("ObjSetPos", new ErmesObjectSetPositionCmd());
    interp.CreateCommand("ObjSetSize", new ErmesObjectSetSizeCmd());
    interp.CreateCommand("ObjGetPosX", new ErmesObjectGetPositionXCmd());
    interp.CreateCommand("ObjGetPosY", new ErmesObjectGetPositionYCmd());
    interp.CreateCommand("ObjGetSizeW", new ErmesObjectGetSizeWCmd());
    interp.CreateCommand("ObjGetSizeH", new ErmesObjectGetSizeHCmd());


    /* current selection */
    interp.CreateCommand("SelGetObjList", new ErmesSelectedCmd());

    /* graphics */
    interp.CreateCommand("graphicson", new ErmesGraphicsOnCmd());
    interp.CreateCommand("graphicsoff", new ErmesGraphicsOffCmd());
    interp.CreateCommand("show", new ErmesShowCmd());

    /* obsolete */
    interp.CreateCommand("new",	new ErmesPatNewCmd());
    interp.CreateCommand("newobj", new ErmesObjNewObjCmd());
    interp.CreateCommand("connect", new ErmesConnectCmd());
    interp.CreateCommand("selected", new ErmesSelectedCmd());
  }
  
  
  /**
   * This method is called when the "exit" command is issued.
   * @return false iff the "System.exit()" method shouldn't be
   * called.
   */
  public boolean ExitNotify() {    
    return false;
  }
}







