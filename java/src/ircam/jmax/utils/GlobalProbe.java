
package ircam.jmax.utils;

public class GlobalProbe {
  static Probe itsProbe;

  public static void makeGlobalProbe() {
    itsProbe = new Probe("global");
    itsProbe.setFile( "global");
  }

  public static void report() {
    if (itsProbe != null) 
      itsProbe.report();
  }

  public static void mark(String theMark) {
    if (itsProbe != null) 
      itsProbe.mark(theMark);
  }

  // @deprecated
  public static void start()
  {
  }

  // @deprecated
  public static void stop()
  {
  }

}

