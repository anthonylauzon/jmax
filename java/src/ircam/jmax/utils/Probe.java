package ircam.jmax.utils;

import java.lang.*;
import java.io.*;
import java.util.*;
import java.text.*;

class RightJustify {
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

    count = 0;

    times = new long[ maxEvents];
    marks = new String[ maxEvents]; 

    prevTime = 0;
    reportCount = 0;

    try
      {
	printOut = new PrintWriter( new FileOutputStream( name + ".probe.out" ), true);
      }
    catch ( java.io.IOException e)
      {
	System.err.println( this.getClass().getName() + ": cannot create output file.");
	printOut = null;
      }
  }

  protected void mark( String markName) 
  {
    int nEmpty = reportCount - count - 1;

    if ( nEmpty < 0)
      nEmpty += times.length;
      
    if ( nEmpty == 0)
      {
	report();
      }

    times[count] = System.currentTimeMillis();
    marks[count] = markName;

    count++;
    if ( count >= times.length )
      count = 0;
  }  

  public void report()
  {
    if ( printOut == null)
      return;

    while ( reportCount != count)
      {
	long delta = times[reportCount] - prevTime;
	
	printOut.println( name + "[ " + reportCount + " ] " + delta + " " + marks[reportCount]);

	prevTime = times[reportCount];

	reportCount++;
	if ( reportCount >= times.length)
	  reportCount = 0;
      }
  }

  private String name;

  private int count;

  private long times[];
  private String marks[];

  private long prevTime;
  private int reportCount;

  private PrintWriter printOut;

  private static final int DEFAULT_MAX_EVENTS = 65536; 
}
