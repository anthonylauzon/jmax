//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.mda;

import java.io.*;
import java.util.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;

/**
 * This class represent the environement where a patch is loaded.
 * For the moment, it provide very trivial experimental functionalities.
 * Actually this file should be somewhere else !!! not in mda package.
 * Kind of an hack.
 *
 * ^^^^ This file do not work in the multiserver configuration, as everything
 * ^^^^ connected to TCL; in order to have it working, we need to rewrite
 * ^^^^ all the configuration code and in general all the tcl code in order
 * ^^^^ to keep track of the server we are using.
 */

public class MaxEnv
{
  static MaxVector loadedEnvs = new MaxVector();

  static public void loadEnvFileFor(MaxContext context, File file)
  {
    String dir;
    String name;
    File envFile;

    dir = file.getParent();
    name = file.getName();

    if (name.lastIndexOf('.') != -1)
      name = name.substring(0, name.lastIndexOf('.')) + ".env";
    else
      name = name + ".env";

    envFile = new File(dir, name);

    if (envFile.exists() && envFile.canRead())
      loadEnvFile(context, envFile);
    else
      {
	envFile = new File(dir, "project.env");

	if (envFile.exists() && envFile.canRead())
	  loadEnvFile(context, envFile);
      }
  }

  static private void loadEnvFile(MaxContext ignore, File file)
  {
    int i;

    for (i = 0; i < loadedEnvs.size(); i++)
      {
	if (file.equals(loadedEnvs.elementAt(i)))
	  return;
      }

    loadedEnvs.addElement(file);

    try
      {
	String dir;
	MaxVector command = new MaxVector();


	dir = file.getParent();

	if (dir == null)
	  dir = System.getProperty("user.dir");

	command.addElement("set");
	command.addElement("projectdir");
	command.addElement(dir);

	MaxApplication.getFts().ucsCommand(command);
	MaxApplication.getTclInterp().eval("sourceFile " + file.getPath());

      }
    catch (tcl.lang.TclException e)
      {
      }
  }
}
