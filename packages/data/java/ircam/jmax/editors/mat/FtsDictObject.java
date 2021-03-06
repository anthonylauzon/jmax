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
    FtsObject.registerMessageHandler( FtsDictObject.class, FtsSymbol.get("start_upload"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsDictObject)obj).startUpload();
    }
    });
    FtsObject.registerMessageHandler( FtsDictObject.class, FtsSymbol.get("end_upload"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsDictObject)obj).endUpload();
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
  public void createEditor()
  {
    if(getEditorFrame() == null)
      setEditorFrame( new MatWindow(this));
  }
  
  public void reinitEditorFrame()
  {
    setEditorFrame( new MatWindow((MatWindow)getEditorFrame()));
  }
  
  public void destroyEditor()
  {
    disposeEditor();
    listeners.removeAllElements();
    System.gc();
  }
  
  /**/
  boolean firstTime = true;
  boolean doing_open_editor = false;
  public void openEditor(int argc, FtsAtom[] argv)
  {  
    if(getEditorFrame() == null)
    {
      createEditor();// rest moved in endUpload   
      firstTime = true;
    }
    else
      firstTime = false;
    
    doing_open_editor = true;
  }  
  
  public boolean doingOpenEditor()
  {
    return doing_open_editor;
  }  
  
  public void showEditor()
  {
    showEditor(firstTime);
    doing_open_editor = false;
  }
  
  /**/  
  
  public void clear()
  {
    for(int i = 0; i < n_rows; i++)
      for(int j = 0; j < n_cols; j++)
        values[i][j] = null;
    
    SwingUtilities.invokeLater(new Runnable() {
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
                
        values[m][n++] = args[i].getValue();
      }
      
      SwingUtilities.invokeLater(new Runnable() {
        public void run()
       { 
          notifyDataChanged();
       }});
    }
  }

  public void setSize(int m, int n)
  {
    if(n_rows != m || n_cols != n)
    {
      n_rows = m;
      n_cols = n;
      values = new Object[n_rows][n_cols];
      
      SwingUtilities.invokeLater(new Runnable() {
        public void run()
       { 
          notifySizeChanged(n_rows, n_cols);
       }});
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
    
    SwingUtilities.invokeLater(new Runnable() {
      public void run()
      { 
        notifySizeChanged(n_rows, n_cols);
      }});
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
        showEditor();
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
    
    if(columnIndex == 0)
    {
      Object key = getValueAt(rowIndex, 0);
      Object val = getValueAt(rowIndex, 1);
      
      if((key instanceof String && aValue instanceof String && ((String)key).equals((String)aValue)) || 
         (key instanceof FtsSymbol && aValue instanceof String && ((FtsSymbol)key).toString().equals((String)aValue)) ||
         (key == aValue))
        return;
        
      args.add( key);
      if(aValue instanceof String)
        args.addSymbol( FtsSymbol.get((String) aValue));
      else
        args.add( aValue);
      try{
        send( FtsSymbol.get("rename"), args);
      }
      catch(IOException e)
      {
        System.err.println("FtsDictObject: I/O Error sending rename Message!");
        e.printStackTrace(); 
      } 
    }
    else
    {
      Object val = getValueAt(rowIndex, 1);
      if((val instanceof String && aValue instanceof String && ((String)val).equals((String)aValue)) ||
         (val instanceof FtsSymbol && aValue instanceof String && ((FtsSymbol)val).toString().equals((String)aValue)) ||
         val == aValue)
        return;
      
      args.add( getValueAt(rowIndex, 0));
      
      if(aValue instanceof String)
        args.addSymbol( FtsSymbol.get((String) aValue));
      else
        args.add( aValue);
  
      try{
        send( FtsSymbol.get("set"), args);
      }
      catch(IOException e)
      {
        System.err.println("FtsDictObject: I/O Error sending set Message!");
        e.printStackTrace(); 
      }    
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
  public void requestInsertColumn(int index){}
  public void requestDeleteCols(int startIndex, int size){}
  
  public void requestInsertRow(int index)
  {
    requestAppendRow();
  }
  
  public void requestDeleteRows(int startIndex, int size)
  {
    args.clear();
    for(int i = 0; i<size; i++)
      args.add( getValueAt(startIndex+i, 0));
    try{
      send( FtsSymbol.get("remove_entries"), args);
    }
    catch(IOException e)
    {
      System.err.println("FtsDictObject: I/O Error sending remove_entries Message!");
      e.printStackTrace(); 
    }    
  }

  
  public void requestImport()
  {
    try{
      send( FtsSymbol.get("import"));
    }
    catch(IOException e)
    {
      System.err.println("FtsDictObject: I/O Error sending import Message!");
      e.printStackTrace(); 
    }    
  }  

  public void requestExport()
  {
    try{
      send( FtsSymbol.get("export"));
    }
    catch(IOException e)
    {
      System.err.println("FtsDictObject: I/O Error sending export Message!");
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
    return "dict";
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
  
  public void addJMaxTableListener(JMaxTableListener theListener){}
  public void removeJMaxTableListener(JMaxTableListener theListener){} 
  
  public Dimension getDefaultSize()
  {
    return defaultSize;
  }
  
  public FtsGraphicObject getFtsMatrixObject()
  {
    return (FtsGraphicObject)this;
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
  
  private void notifyNameChanged(String name)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((MatDataListener) e.nextElement()).matNameChanged(name);
  }
  
  private void notifyUpload(boolean uploading)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((MatDataListener) e.nextElement()).uploading(uploading);
  }
  /*******************************************************************/
  private Object[][] values;
  MaxVector listeners = new MaxVector();
  protected FtsArgs args = new FtsArgs();
  int n_rows = 0;
  int n_cols = 0;

  public final static int DICT_DEFAULT_WIDTH  = 300;
  public final static int DICT_DEFAULT_HEIGHT = 200;
  static Dimension defaultSize = new Dimension(DICT_DEFAULT_WIDTH, DICT_DEFAULT_HEIGHT);  
}











