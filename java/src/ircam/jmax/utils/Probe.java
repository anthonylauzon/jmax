
package ircam.jmax.utils;
import java.io.*;
import java.util.*;

/**
 * A simple profile utility.
 * Usage: after creating a probe with a name, call start to
 * start the mesure, and mark() to store a symbolic name that will
 * be associated to the time of the call.
 * The time shown is relative to the preceding mark, so you can use the "silentMark"
 * method to establish starting points that will not be shown.
 * Call stop when you're done with the Probe, and before making reports via report() or
 * reportToFile() calls.
 */

public class Probe {
  final static int MAX_EVENTS = 1000; 
  long times[] = new long[MAX_EVENTS];
  String labels[] = new String[MAX_EVENTS]; 
  long startTime;
  String itsName;
  int eventCounter = 0;
  boolean running=false;

  public Probe(String theName) {
    itsName = theName;
  }

  public void start() {
    running = true;
    mark("probe "+itsName+" start");
  }

  public void mark(String markName) {
    if (running) {
      times[eventCounter] = System.currentTimeMillis();
      labels[eventCounter++] = markName;
    }  
    else start();//System.out.println("Probe "+itsName+" is stopped. Can't store time stamp");
  }

  public void silentMark() {
    times[eventCounter] = System.currentTimeMillis();
    labels[eventCounter++] = "";
  }
  
  public void stop() {
    mark("probe "+itsName+" stop");
    running = false;
  }
  public void report() {
   for (int i=1; i<eventCounter; i++) {
     if (labels[i].equals("")) continue;
     System.err.println("Probe "+itsName+" ****   after ms:"+(times[i]-times[i-1])+"   \t["+(times[i]-startTime)+"]\t"+labels[i]);
   }
  }

  public void reportToFile(String path) {
   PrintWriter out;
   FileWriter aFileWriter;
   try {
     aFileWriter = new FileWriter(new File(path));
     out = new PrintWriter(aFileWriter);
   }
   catch (Exception e){
     System.out.println("Error while creating report "+path+": "+e);
     return;
   }

   for (int i=1; i<eventCounter; i++) {
     if (labels[i].equals("")) continue;
     out.println("Probe "+itsName+" ****   after ms:"+(times[i]-times[i-1])+"   \t["+(times[i]-startTime)+"]\t"+labels[i]);
   }
   try {
     aFileWriter.flush();
     aFileWriter.close();
   }
   catch (Exception e) {
      System.out.println("Error while closing report "+path+": "+e);
   }
  }
}
