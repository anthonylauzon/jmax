
package ircam.jmax.utils;

public class GlobalProbe {

  static Probe probe;

  static
  {
    probe = new Probe( "global");
  }

  // @deprecated
  public static void makeGlobalProbe()
  {
  }

  public static void setSynchronous( boolean synchronous)
  {
    probe.setSynchronous( synchronous);
  }

  public static void report() 
  {
    probe.report();
  }

  public static void setOutputToFile()
  {
    probe.setOutputToFile();
  }

  public static void enterMethod( Object object, String methodName, String extraMsg)
  {
    probe.enterMethod( object, methodName, extraMsg);
  }

  public static void enterMethod( Object object, String methodName)
  {
    probe.enterMethod( object, methodName);
  }

  public static void exitMethod()
  {
    probe.exitMethod();
  }

  public static void mark(String markName) 
  {
    probe.mark( markName);
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

