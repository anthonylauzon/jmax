
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

public class Probe implements OscillSource
{
  int MAX_EVENTS = 10000; 
  long times[];
  String labels[];
  long startTime;
  String itsName;
  int eventCounter = 0;
  int autoReport   = 0;
  boolean running=false;
  long lastDelta = 0;
  
  public Probe(String theName, int max)
  {
    itsName = theName;
    MAX_EVENTS = max;
    times = new long[MAX_EVENTS];
    labels = new String[MAX_EVENTS]; 
  }

  public int getValue() {    
    System.err.println(lastDelta);
    return (int) lastDelta/100;//for now, there's no scaling in the Oscilloscope
  }

  public Probe(String theName)
  {
    itsName = theName;
    times = new long[MAX_EVENTS];
    labels = new String[MAX_EVENTS]; 
  }

  public Probe(String theName, int max, int autoReport)
  {
    itsName = theName;
    MAX_EVENTS = max;
    this.autoReport = autoReport;
    times = new long[MAX_EVENTS];
    labels = new String[MAX_EVENTS]; 
  }

  public void setAutoReport(int v)
  {
    autoReport = v;
  }

  public void start()
  {
    running = true;
    startTime = System.currentTimeMillis();
  }

  public void mark(String markName) {

    if (running)
      {
	if (eventCounter < MAX_EVENTS)
	  {
	    times[eventCounter] = System.currentTimeMillis();
	    labels[eventCounter++] = markName;
	    lastDelta = times[eventCounter-1] - times[eventCounter-2];
	  }

	if (autoReport != 0)
	  if ((eventCounter % autoReport) == 0)
	    report(eventCounter - autoReport, eventCounter);
      }
  }

  public void silentMark() {
    if (eventCounter < MAX_EVENTS) {
    times[eventCounter] = System.currentTimeMillis();
    labels[eventCounter++] = null;
    }
  }
  
  public void stop() {
    mark("probe "+itsName+" stop");
    running = false;
  }

  public void report()
  {
    report(0, eventCounter - 1);
  }

  public void report(int end)
  {
    report(0, end);
  }

  public void report(int begin, int end)
  {
    if (begin < 0)
      begin = 0;

    if (end >=  eventCounter)
      end = eventCounter - 1;
      
    System.err.println("Probe " + itsName +" Report");

    for (int i = begin; i < end; i++)
      {
	if (labels[i] != null)
	  {
	    if (i == 0)
	      System.err.println(labels[i] + " @ ms:" + (times[i]-startTime));
	    else
	      System.err.println(labels[i] +" @ ms:"+ (times[i]-startTime) 
				 + " \tdelta: " + (times[i]-times[i-1]));
	  }
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
     if (labels[i] != null)
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





