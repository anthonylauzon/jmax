/*
 * ErmesPackage.java --
 *
 */

package ircam.jmax.editors.ermes.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclErmesPackage 
{
  /**
   * Default constructor. Create the built-in commands and load the
   * start-up scripts (ToDo).
   */

  static public void  installPackage()
  {
    Interp interp = MaxApplication.getTclInterp();

    /* Ermes commands*/
    
    /* patcher commands */
    interp.createCommand("PatNew", new ErmesPatNewCmd());
    
    /* object commands */
    interp.createCommand("ObjNew", new ErmesObjNewObjCmd());
    interp.createCommand("ObjConnect", new ErmesConnectCmd());
    interp.createCommand("ObjDisconnect", new ErmesDisconnectCmd());
    
    interp.createCommand("ObjSetPos", new ErmesObjectSetPositionCmd());
    interp.createCommand("ObjSetSize", new ErmesObjectSetSizeCmd());
    interp.createCommand("ObjGetPosX", new ErmesObjectGetPositionXCmd());
    interp.createCommand("ObjGetPosY", new ErmesObjectGetPositionYCmd());
    interp.createCommand("ObjGetSizeW", new ErmesObjectGetSizeWCmd());
    interp.createCommand("ObjGetSizeH", new ErmesObjectGetSizeHCmd());

    // Move is not there
    //    interp.createCommand("ObjMove", new ErmesObjectMoveCmd());

    /* current selection */
    interp.createCommand("SelGetObjList", new ErmesSelectedCmd());

    /* graphics */
    interp.createCommand("graphicson", new ErmesGraphicsOnCmd());
    interp.createCommand("graphicsoff", new ErmesGraphicsOffCmd());
    //no more used interp.createCommand("show", new ErmesShowCmd());

    /* obsolete */
    interp.createCommand("new",	new ErmesPatNewCmd());
    interp.createCommand("newobj", new ErmesObjNewObjCmd());
    interp.createCommand("connect", new ErmesConnectCmd());
    interp.createCommand("selected", new ErmesSelectedCmd());
    interp.createCommand("debugregions", new ErmesDebugRegionsCmd());
  }
}







