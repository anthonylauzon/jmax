package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import java.lang.*;
import java.io.*;
import java.util.*;

public class ExplodeRemoteData extends FtsRemoteData {

  private class ExplodeNote {
    ExplodeNote( Object args[])
    {
      note = new int[5];

      for ( int i = 0; i < 5; i++)
	{
	  note[i] = ((Integer)args[i]).intValue();
	}
    }

    int getValue( int index)
    {
      return note[index];
    }

    private int note[];
  }

  public ExplodeRemoteData()
    {
      super();

      System.err.println( "instanciated " + this.getClass().getName());

      notes = new Vector();
    }

  public int length()
  {
    return notes.size();
  }

  public Object getValue( int index, int which)
  {
    return new Integer( ((ExplodeNote)notes.elementAt(index)).getValue( which));
  }

  public final void call( int key, Object args[])
    {
      switch( key) {
      case 1:
	notes.removeAllElements();
	break;
      case 2:
	notes.addElement( new ExplodeNote( args));
	break;
      default:
	break;
      }
    }

  private Vector notes;
}
