package ircam.jmax.mda;

import java.io.*;
import java.util.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * This class represent the environement where a patch is loaded.
 * For the moment, it provide very trivial experimental functionalities.
 */

public class MaxEnv
{
  static MaxVector loadedEnvs = new MaxVector();

  static public void loadEnvFileFor(File file)
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
      loadEnvFile(envFile);
    else
      {
	envFile = new File(dir, "project.env");

	if (envFile.exists() && envFile.canRead())
	  loadEnvFile(envFile);
      }
  }

  static private void loadEnvFile(File file)
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
	MaxApplication.getTclInterp().eval("sourceFile " + file.getPath());
      }
    catch (tcl.lang.TclException e)
      {
      }
  }
}
