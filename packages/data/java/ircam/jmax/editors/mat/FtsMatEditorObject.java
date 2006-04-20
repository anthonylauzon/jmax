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
import java.awt.*;

/**
* A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsMatEditorObject extends FtsUndoableObject implements MatDataModel
{
  static
  {
    FtsObject.registerMessageHandler( FtsMatEditorObject.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsMatEditorObject)obj).clear();
      }
    });
    FtsObject.registerMessageHandler( FtsMatEditorObject.class, FtsSymbol.get("set"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsMatEditorObject)obj).set( args.getLength(), args.getAtoms());
      }
    });
    FtsObject.registerMessageHandler( FtsMatEditorObject.class, FtsSymbol.get("size"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsMatEditorObject)obj).setSize( args.getInt(0), args.getInt(1));
      }
    });
    FtsObject.registerMessageHandler( FtsMatEditorObject.class, FtsSymbol.get("start_upload"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsMatEditorObject)obj).startUpload();
      }
    });
    FtsObject.registerMessageHandler( FtsMatEditorObject.class, FtsSymbol.get("end_upload"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsMatEditorObject)obj).endUpload();
      }
    });     
  }

  /**
  * constructor.
   */
  public FtsMatEditorObject(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);
    matrix = parent;
  
    listeners = new MaxVector();
    type = (matrix instanceof FtsFmatObject);
  }
  
  public boolean isFmat()
  {
    return type;
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES called from fts.
  //////////////////////////////////////////////////////////////////////////////////////
  
  public void clear()
  {
    for(int i = 0; i < n_rows; i++)
      for(int j = 0; j < n_cols; j++)
        values[i][j] = null;
    
    SwingUtilities.invokeLater(new Runnable(){
      public void run()
    {
        notifyClear();
    }});
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
        
        if(matrix instanceof FtsFmatObject)
          values[m][n++] = new Float(args[i].doubleValue);
        else
          values[m][n++] =  args[i].getValue();
      }
    
      SwingUtilities.invokeLater(new Runnable(){
        public void run()
        {
          notifyDataChanged();
        }});
    }
  }

  Float zeroFloat = new Float(0.0);
  
  public void setSize(int m, int n)
  {    
    if(n_rows != m || n_cols != n)
    {
      Object[][] temp = new Object[m][n];
      for(int i = 0; i < m; i++)
      {
        for(int j = 0; j < n; j++)
        {
          if(i < n_rows && j < n_cols)
            temp[i][j] = values[i][j];
          else
            temp[i][j] = zeroFloat;
        }
      }
      n_rows = m;
      n_cols = n;
      values = temp;
      
      SwingUtilities.invokeLater(new Runnable(){
        public void run()
      {
          notifySizeChanged(n_rows, n_cols);
      }});
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
    SwingUtilities.invokeLater(new Runnable(){
      public void run()
      {
        ((FtsObjectWithEditor)matrix).showEditor();
        FtsObject.requestResetGui();
        uploading = false;
        notifyUpload(false);
      } 
    });    
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
      /*matrix.*/send( FtsSymbol.get("set"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending set Message!");
      e.printStackTrace(); 
    }    
  }
  
  public void requestAppendRow()
  {
    args.clear();
    args.addInt( n_rows+1);
    try{
      matrix.send( FtsSymbol.get("rows"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending append_row Message!");
      e.printStackTrace(); 
    }    
  }
  
  public void requestAppendColumn()
  {
    args.clear();
    args.addInt( n_cols+1);
    try{
      matrix.send( FtsSymbol.get("cols"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending append_column Message!");
      e.printStackTrace(); 
    }   
  }
  
  public void requestInsertRow(int index)
  {
    args.clear();
    args.addInt( index);
    
    try{
      matrix.send( FtsSymbol.get("insert"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending insert Message!");
      e.printStackTrace(); 
    }    
  }    
  
  public void requestInsertColumn(int index)
  {
    args.clear();
    args.addInt( index);
    
    try{
      matrix.send( FtsSymbol.get("insert_cols"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending insert_cols Message!");
      e.printStackTrace(); 
    }    
  }    
  
  public void requestDeleteRows(int startIndex, int size)
  {
    args.clear();
    args.addInt( startIndex);
    if(size > 1)
      args.addInt(size);
    
    try{
      matrix.send( FtsSymbol.get("delete"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending delete Message!");
      e.printStackTrace(); 
    }    
  }  
  
  public void requestDeleteCols(int startIndex, int size)
  {
    args.clear();
    args.addInt( startIndex);
    if(size > 1)
      args.addInt(size);
    
    try{
      matrix.send( FtsSymbol.get("delete_cols"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending delete_cols Message!");
      e.printStackTrace(); 
    }    
  }  
  
  public void requestImport()
  {
    try{
      matrix.send( FtsSymbol.get("import"));
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending import Message!");
      e.printStackTrace(); 
    }    
  }  
  
  public void requestExport()
  {
    try{
      matrix.send( FtsSymbol.get("export"));
    }
    catch(IOException e)
    {
      System.err.println("FtsMatEditorObject: I/O Error sending export Message!");
      e.printStackTrace(); 
    }    
  }
  
  int lastVisibleRow = 0;
  public void requestSetLastVisibleRow(int lastVRow)
  {    
    if(lastVRow > lastVisibleRow)
    {
      lastVisibleRow = lastVRow;
      args.clear();
      args.addInt(lastVisibleRow);
      
      try{
        send( FtsSymbol.get("set_last_visible_row"), args);
      }
      catch(IOException e)
      {
        System.err.println("FtsMatEditorObject: I/O Error sending set_last_visible_row Message!");
        e.printStackTrace(); 
      }
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
    return "fmat";
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
  
  Float prut = new Float(0.0); 
  public Object getValueAt(int m, int n)
  {
    if((m < n_rows) && (n < n_cols))
      return values[m][n];
    else
      return prut;// to avoid out_of_range access 
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
  
  public Dimension getDefaultSize()
  {
    return defaultSize;
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
  
  private FtsObject matrix;
  private boolean type; /* true if vector is an FtsFvecObject */
  private Object[][] values;
  MaxVector listeners = new MaxVector();
  protected FtsArgs args = new FtsArgs();
  int n_rows = 0;
  int n_cols = 0;
  
  public final static int MATEDITOR_DEFAULT_WIDTH  = 370;
  public final static int MATEDITOR_DEFAULT_HEIGHT = 250;
  static Dimension defaultSize = new Dimension(MATEDITOR_DEFAULT_WIDTH, MATEDITOR_DEFAULT_HEIGHT);  
}











