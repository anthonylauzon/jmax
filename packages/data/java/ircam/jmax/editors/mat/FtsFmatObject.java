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
public class FtsFmatObject extends FtsObjectWithEditor implements MatDataModel
{
  static
  {
    FtsObject.registerMessageHandler( FtsFmatObject.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
          ((FtsFmatObject)obj).clear();
      }
    });
    FtsObject.registerMessageHandler( FtsFmatObject.class, FtsSymbol.get("set"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsFmatObject)obj).set( args.getLength(), args.getAtoms());
      }
   });
    FtsObject.registerMessageHandler( FtsFmatObject.class, FtsSymbol.get("size"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsFmatObject)obj).setSize( args.getInt(0), args.getInt(1));
    }
    });
    FtsObject.registerMessageHandler( FtsFmatObject.class, FtsSymbol.get("start_upload"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsFmatObject)obj).startUpload();
    }
    });
    FtsObject.registerMessageHandler( FtsFmatObject.class, FtsSymbol.get("end_upload"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsFmatObject)obj).endUpload();
    }
    });    
  }

  /**
   * constructor.
   */
  public FtsFmatObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId, classname, args, offset, length); 
    
    listeners = new MaxVector();
      
    if( length > offset+1 && args[offset].isInt() && args[offset+1].isInt())
    {
      n_rows = args[offset].intValue;
      n_cols = args[offset+1].intValue;
    }
  }

  public FtsFmatObject(FtsServer server, FtsObject parent, int objId)
  {
    super(server, parent, objId); 
    listeners = new MaxVector();
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
                
        values[m][n++] = new Float(args[i].doubleValue);
      }
      
      notifyDataChanged();
    }
  }

  public void setSize(int m, int n)
  {
    if(n_rows != m || n_cols != n)
    {
      Object[][] temp = new Object[m][n];
      for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
        {
          if(i < n_rows && j < n_cols)
            temp[i][j] = values[i][j];
          else
            temp[i][j] = new Float(0.0);
        }
      n_rows = m;
      n_cols = n;
      values = temp;
      
      notifySizeChanged(n_rows, n_cols);
    }
  }
    
  boolean uploading = false;
  public void startUpload()
  {
    uploading = true;
    notifyUpload(true);
  }
  public void endUpload()
  {
    uploading = false;
    notifyUpload(false);
  } 
  
  public void nameChanged( String name)
  {
    super.nameChanged( name);
    notifyNameChanged( name);
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
      System.err.println("FtsFmatObject: I/O Error sending set Message!");
      e.printStackTrace(); 
    }    
  }
  
  public void requestAppendRow()
  {
    args.clear();
    args.addInt( n_rows+1);
    try{
      send( FtsSymbol.get("rows"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsFmatObject: I/O Error sending append_row Message!");
      e.printStackTrace(); 
    }    
  }
  
  public void requestAppendColumn()
  {
    args.clear();
    args.addInt( n_cols+1);
    try{
      send( FtsSymbol.get("cols"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsFMatObject: I/O Error sending append_column Message!");
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
  
  public boolean haveRowIdCol()
  {
    return true;
  }
  
  public boolean canAppendColumn()
  {
    return true;
  }
  
  public String getColumnName(int col_id)
  {
    return ""+col_id;
  }
  
  public String getType()
  {
    return "FMat";
  }  
  
  public String getName()
  {
    return super.getVariableName();
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
  private void notifyUpload(boolean uploading)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((MatDataListener) e.nextElement()).uploading(uploading);
  }
  private void notifyNameChanged(String name)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((MatDataListener) e.nextElement()).matNameChanged(name);
  }
  /*******************************************************************/
  private Object[][] values;
  MaxVector listeners = new MaxVector();
  protected FtsArgs args = new FtsArgs();
  int n_rows = 0;
  int n_cols = 0;
}











