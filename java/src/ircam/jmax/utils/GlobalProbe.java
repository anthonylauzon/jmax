
package ircam.jmax.utils;

public class GlobalProbe {
  static Probe itsProbe;

  public static void makeGlobalProbe() {
    itsProbe = new Probe("global");
  }

  public static void start() {
    if (itsProbe == null) makeGlobalProbe();
    itsProbe.start();
  }

  public static void stop() {
    if (itsProbe != null)
      itsProbe.stop();
  }

  public static void report() {
    if (itsProbe != null) 
      itsProbe.report();
  }

  public static void silentMark() {
    if (itsProbe != null)
      itsProbe.silentMark();
  }

  public static void mark(String theMark) {
    if (itsProbe != null) 
      itsProbe.mark(theMark);
  }
}

