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
public class FtsMatObject extends FtsObjectWithEditor implements MatDataModel
{
  static
  {
    FtsObject.registerMessageHandler( FtsMatObject.class, FtsSymbol.get("append"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        System.err.println("FtsMatObject append !!!!");
        ((FtsMatObject)obj).appendRow(args.getLength(), args.getAtoms());
    }
    });
    FtsObject.registerMessageHandler( FtsMatObject.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
          ((FtsMatObject)obj).clear();
      }
    });
    FtsObject.registerMessageHandler( FtsMatObject.class, FtsSymbol.get("set"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsMatObject)obj).set( args.getLength(), args.getAtoms());
      }
   });
    FtsObject.registerMessageHandler( FtsMatObject.class, FtsSymbol.get("size"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsMatObject)obj).setSize( args.getInt(0), args.getInt(1));
    }
    });
  }

  /**
   * constructor.
   */
  public FtsMatObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
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
  
  public void appendRow(int nArgs, FtsAtom args[])
  {
    Object[][] temp = new Object[n_rows+1][n_cols];
    for(int i = 0; i < n_rows; i++)
      for(int j = 0;j<n_cols; j++)
        temp[i][j] = values[i][j];
    for(int j = 0; j< n_cols && j<nArgs; j++)
      temp[n_rows+1][j] = args[j].getValue();
    
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
    args.addInt( rowIndex);
    args.addInt( columnIndex);
    if(aValue instanceof String)
      args.addSymbol( FtsSymbol.get((String) aValue));
    else
      args.add( aValue);
    
    try{
      send( FtsSymbol.get("set"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatObject: I/O Error sending set Message!");
      e.printStackTrace(); 
    }    
  }
  
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
  
  public String getColumnName(int col_id)
  {
    return "col "+col_id;
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











