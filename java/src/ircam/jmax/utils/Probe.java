package ircam.jmax.utils;

import java.lang.*;
import java.io.*;
import java.util.*;

public class Probe {

  public Probe()
  {
    this( "NoName", DEFAULT_MAX_EVENTS);
  }

  public Probe( String name)
  {
    this( name, DEFAULT_MAX_EVENTS);
  }

  public Probe( int maxEvents)
  {
    this( "NoName", maxEvents);
  }

  public Probe( String name, int maxEvents) 
  {
    this.name = name;
    this.maxEvents = maxEvents;

    prevTime = 0;
    reportCount = 0;
    times = new long[maxEvents];
    marks = new String[maxEvents]; 
    count = 0;

    printlnAdapter = new SystemOutPrintlnAdapter();
  }

  private interface Println {
    public void println( String s);
  }

  private class FileOutPrintlnAdapter implements Println {
    FileOutPrintlnAdapter( PrintWriter printOut) { this.printOut = printOut; }
    public void println( String s) { printOut.println( s); }
    private PrintWriter printOut;
  }

  private class SystemOutPrintlnAdapter implements Println {
    public void println( String s) { System.out.println( s); }
  }

  public void setFile( String fileName)
  {
    try
      {
	PrintWriter printOut;

	printOut = new PrintWriter( new FileOutputStream( name + ".probe.out" ), true);
	printlnAdapter = new FileOutPrintlnAdapter( printOut);
      }
    catch ( java.io.IOException e)
      {
	System.err.println( "[Probe] Cannot create output file");
      }
  }

  public void mark(String markName) 
  {
    if (count >= maxEvents)
      {
	report();
	count = 0;
      }

    times[count] = System.currentTimeMillis();
    marks[count] = markName;
    count++;
  }  

  public void report()
  {
    for ( int i = 0; i < count; i++)
      {
	long delta = times[i] - prevTime;
	printlnAdapter.println( name + "[" + reportCount + "] mark='" + marks[i] + "' delta=" + delta + "ms " );

	reportCount++;
	prevTime = times[i];
      }
  }

  private static final int DEFAULT_MAX_EVENTS = 1024; 
  private String name;
  private int maxEvents;

  private int reportCount;
  private long prevTime;

  private int count;
  private long times[];
  private String marks[];
  private Println printlnAdapter;
}
