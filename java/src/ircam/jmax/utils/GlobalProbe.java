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
