//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.mat;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsDictObject extends FtsObjectWithEditor implements MatDataModel
{
  static
  {
    FtsObject.registerMessageHandler( FtsDictObject.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
          ((FtsDictObject)obj).clear();
      }
    });
    FtsObject.registerMessageHandler( FtsDictObject.class, FtsSymbol.get("set"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsDictObject)obj).set( args.getLength(), args.getAtoms());
      }
   });
    FtsObject.registerMessageHandler( FtsDictObject.class, FtsSymbol.get("size"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsDictObject)obj).setSize( args.getInt(0), args.getInt(1));
    }
    });
    FtsObject.registerMessageHandler( FtsDictObject.class, FtsSymbol.get("dict_append_row"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsDictObject)obj).appendRow();
    }
    });
  }

  /**
   * constructor.
   */
  public FtsDictObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId, classname, args, offset, length); 
    
    listeners = new MaxVector();
      
    if( length > offset+1 && args[offset].isInt() && args[offset+1].isInt())
    {
      n_rows = args[offset].intValue;
      n_cols = args[offset+1].intValue;
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES called from the server
  //////////////////////////////////////////////////////////////////////////////////////
  
  public void openEditor(int argc, FtsAtom[] argv)
  {
    if(getEditorFrame() == null)
      setEditorFrame( new MatWindow(this));
    
    showEditor();
  }
  
  public void destroyEditor()
  {
    disposeEditor();
  }
    
  public void clear()
  {
    for(int i = 0; i < n_rows; i++)
      for(int j = 0; j < n_cols; j++)
        values[i][j] = null;
    
    notifyClear();
  }

  public void set(int nArgs , FtsAtom args[])
  {        
    if(nArgs > 2)
    {
      int m = args[0].intValue;
      int n = args[1].intValue;
            
      for(int i = 2; i < nArgs; i++)
      {
        if(n >= n_cols)
        {
          n = 0;
          m++;
        }
                
        values[m][n++] = args[i].getValue();
      }
      
      notifyDataChanged();
    }
  }

  public void setSize(int m, int n)
  {
    if(n_rows != m || n_cols != n)
    {
      n_rows = m;
      n_cols = n;
      values = new Object[n_rows][n_cols];
      
      notifySizeChanged(n_rows, n_cols);
    }
  }
  
  public void appendRow()
  {
    Object[][] temp = new Object[n_rows+1][n_cols];
    for(int i = 0; i < n_rows; i++)
      for(int j = 0;j<n_cols; j++)
        temp[i][j] = values[i][j];
    for(int j = 0; j < n_cols; j++)
      temp[n_rows][j] = new Integer(0);
    
    values = temp;
    n_rows++;
    
    notifySizeChanged(n_rows, n_cols);
  }
  
  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES to the server
  //////////////////////////////////////////////////////////////////////////////////////
  
  public void requestSetValue( java.lang.Object aValue, int rowIndex, int columnIndex)
  {
    args.clear();
    
    if(columnIndex == 0)
    {
      Object val = getValueAt(rowIndex, 1);
      
      /* remove old key entry */
      args.add( getValueAt(rowIndex, 0));
      try{
        send( FtsSymbol.get("remove"), args);
      }
      catch(IOException e)
      {
        System.err.println("FtsDictObject: I/O Error sending remove Message!");
        e.printStackTrace(); 
      }    
      
      /* no new value, only remove entry */
      if(aValue == null || ((aValue instanceof String) && ((String)aValue).equals("")) )
        return;
      
      /* add new key entry with old value */
      args.clear();
      if(aValue instanceof String)
        args.addSymbol( FtsSymbol.get((String) aValue));
      else
        args.add( aValue);
      
      args.add(val);
    }
    else
    {
      /* */
      args.add( getValueAt(rowIndex, 0));
      
      if(aValue instanceof String)
        args.addSymbol( FtsSymbol.get((String) aValue));
      else
        args.add( aValue);
    }
    try{
      send( FtsSymbol.get("set"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsDictObject: I/O Error sending set Message!");
      e.printStackTrace(); 
    }    
  }
  
  public void requestAppendRow()
  {
    args.clear();
    args.addSymbol(FtsSymbol.get("<empty key>"));
    args.addSymbol(FtsSymbol.get("<empty value>"));
    try{
      send( FtsSymbol.get("set"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsDictObject: I/O Error sending append Message!");
      e.printStackTrace(); 
    }    
  }
  
  public void requestAppendColumn(){}

  /************************************************************************************
  ** MatDataModel interface
  *************************************************************************************/
  public int getSize()
  {
    return n_rows * n_cols;
  }
  
  public int getRows()
  {
    return n_rows;
  }
  
  public int getColumns()
  {
    return n_cols;
  }
  
  public boolean haveRowIdCol()
  {
    return false;
  }
  
  public boolean canAppendColumn()
  {
    return false;
  }
  
  public String getColumnName(int col_id)
  {
    switch(col_id)
    {
      case 0: return "keys";
      case 1: return "values";
      default: return "";
    }
  }
  
  public String getType()
  {
    return "Dict";
  }
  
  public void setRows(int m)
  {
    setSize(m, n_cols);
  }
  
  public void setColumns(int n)
  {
    setSize(n_rows, n);
  }
  
  public Object getValueAt(int m, int n)
  {
    return values[m][n];
  }
  public void setValueAt(int m, int n, Object value)
  {
    values[m][n] = values;
  }

  public void addMatListener(MatDataListener theListener) 
  {
    listeners.addElement(theListener);
  }
  
  public void removeMatListener(MatDataListener theListener) 
  {
    listeners.removeElement(theListener);
  }

 /********************************************************************
  * notifications
  */
    
  private void notifyClear()
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((MatDataListener) e.nextElement()).matCleared();
  }

  private void notifyDataChanged()
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((MatDataListener) e.nextElement()).matDataChanged();
  }
  
  private void notifySizeChanged(int n_rows, int n_cols)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((MatDataListener) e.nextElement()).matSizeChanged(n_rows, n_cols);
  }
  /*******************************************************************/
  private Object[][] values;
  MaxVector listeners = new MaxVector();
  protected FtsArgs args = new FtsArgs();
  int n_rows = 0;
  int n_cols = 0;
}











