//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.utils;

public class GlobalProbe {

  public static void report() 
  {
    probe.report();
  }

  public static void mark(String markName) 
  {
    probe.mark( markName);
  }

  private static Probe probe = new Probe( "global", 32);
}
