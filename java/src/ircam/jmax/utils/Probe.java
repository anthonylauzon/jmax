package ircam.jmax.utils;

import java.lang.*;
import java.io.*;
import java.util.*;
import java.text.*;

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

    synchronous = false;

    prevTime = 0;

    count = 0;
    reportCount = 0;
    globalCount = 0;

    currentDepth = 0;

    times = new long[maxEvents];
    depths = new int[maxEvents];
    marks = new String[maxEvents]; 

    printlnAdapter = null;

    rj10 = new RightJustify( 10, ' ', true);
    rj4 = new RightJustify( 4, ' ', false);
  }

  public void setSynchronous( boolean synchronous)
  {
    this.synchronous = synchronous;
  }

  public boolean getSynchronous()
  {
    return synchronous;
  }

  private interface Println {
    public void println( String s);
    public void flush();
  }

  private class FileOutPrintlnAdapter implements Println {
    FileOutPrintlnAdapter( PrintWriter printOut) { this.printOut = printOut; }
    public void println( String s) { printOut.println( s); }
    public void flush() { printOut.flush(); }
    private PrintWriter printOut;
  }

  private class SystemErrPrintlnAdapter implements Println {
    public void println( String s) { System.err.println( s); }
    public void flush() { System.err.flush(); }
  }

  public void setOutputToFile()
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

  public void enterMethod( Object object, String methodName, String extraMsg)
  {
    mark( object.getClass().getName() + "." + methodName + "() " + extraMsg + " {");
    currentDepth++;
  }

  public void enterMethod( Object object, String methodName)
  {
    mark( object.getClass().getName() + "." + methodName + "() {");
    currentDepth++;
  }

  public void exitMethod()
  {
    currentDepth--;
    mark( "}");
  }

  protected void mark(String markName) 
  {
    int nEmpty = reportCount - count - 1;

    if (nEmpty < 0)
      nEmpty += times.length;
      
    if ( nEmpty == 0)
      {
	report();
      }

    times[count] = System.currentTimeMillis();
    depths[count] = currentDepth;
    marks[count] = markName;

    count++;
    if (count >= times.length)
      count = 0;

    if (synchronous)
      report();
  }  

  private class RightJustify {
    protected RightJustify( int fieldLength, char pad, boolean signed)
    {
      numberFormat = NumberFormat.getInstance( Locale.US);

      numberFormat.setMaximumFractionDigits(6);
      numberFormat.setMinimumFractionDigits(0);
      numberFormat.setGroupingUsed(false);

      fieldPositionInteger = new FieldPosition( NumberFormat.INTEGER_FIELD);

      this.fieldLength = fieldLength;
      this.pad = pad;
      this.signed = signed;
    }

    protected String format( long n)
    {
      StringBuffer buffer = new StringBuffer( 64);
      int padLength;

      numberFormat.format( n, buffer, fieldPositionInteger);

      padLength = fieldLength - fieldPositionInteger.getEndIndex();

      if (signed)
	{
	  if (n >= 0)
	    {
	      buffer.insert( 0, '+');
	      padLength--;
	    }
	}

      for ( int j = 0; j < padLength; j++)
	buffer.insert( 0, pad);

      return buffer.toString();
    }

    protected int fieldLength;
    protected char pad;
    protected boolean signed;
    protected NumberFormat numberFormat;
    protected FieldPosition fieldPositionInteger;
  }

  public void report()
  {
    if (printlnAdapter == null)
      printlnAdapter = new SystemErrPrintlnAdapter();

    if (reportCount == 0)
      prevTime = times[reportCount];

    while (reportCount != count)
      {
	long delta = times[reportCount] - prevTime;

	StringBuffer sb = new StringBuffer(128);

	for( int i = 0; i < depths[reportCount]; i++)
	  sb.append( "    ");

	sb.append( "|- ").append( marks[reportCount]);

	printlnAdapter.println( name + "[ " + rj4.format(reportCount) + " ] " + rj10.format(delta) + " " + sb);

	prevTime = times[reportCount];

	globalCount++;

	reportCount++;
	if ( reportCount >= times.length)
	  reportCount = 0;
      }
  }

  private String name;

  boolean synchronous;

  private long prevTime;

  private int count;
  private int reportCount;
  private int globalCount;

  private int currentDepth;

  private long times[];
  private int depths[];
  private String marks[];

  private Println printlnAdapter;

  RightJustify rj10;
  RightJustify rj4;

  private static final int DEFAULT_MAX_EVENTS = 65536; 
}
