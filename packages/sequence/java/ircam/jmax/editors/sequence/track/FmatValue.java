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


package ircam.jmax.editors.sequence.track;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.*;
import java.io.*;
import javax.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.datatransfer.*;

import ircam.fts.client.*;

public class FmatValue extends AbstractEventValue
{
  double DEFAULT_DURATION = 64.0;
  Object value = new Integer(50);
  Object objId = new Integer(-1);
  Object varname = "";
  Object object  = null;
  static FtsArgs args = new FtsArgs();
  
  public FmatValue()
  {
    super();
    
    setProperty("value", value);
    setProperty("duration", new Double(DEFAULT_DURATION));
    setProperty("type", "fmat");
    setProperty("objid", objId);
  }
  
  public void setProperty(String name, Object value)
  {        
    if(name.equals("objid"))
    {
      if(((Integer)objId).intValue() != ((Integer)value).intValue())
      {
        objId = value;
        FtsObject fmat = JMaxApplication.getFtsServer().getObject(((Integer)objId).intValue());
        
        String varname = null;
        if(fmat != null)
          varname = ((FtsGraphicObject)fmat).getVariableName();
        
        if(varname != null && !varname.equals(""))
          setProperty("name", varname);
        else
          setProperty("name", "#"+((Integer)objId).intValue());
                
        setProperty("object", fmat);
      }
    }
    else if(name.equals("value"))
      this.value = value;
    else if(name.equals("name"))
      this.varname = value;
    else if(name.equals("object"))
      this.object = value;
    else 
      super.setProperty(name, value);
  }
  
  public Object getProperty(String name)
  {
    if(name.equals("objid"))
      return objId;
    else if(name.equals("object"))
      return object;
    else if(name.equals("value"))
      return value;
    else if(name.equals("name"))
      return varname;
    else 
      return super.getProperty(name);
  }
  
  public void edit(int x, int y, int modifiers, Event evt, SequenceGraphicContext gc)
  {
    int id = ((Integer)objId).intValue();
    FtsObject fmat = JMaxApplication.getFtsServer().getObject(id);
    
    if(fmat == null)
    {
      args.clear();
      args.addString(FMAT_NAME);
      fmat = JMaxApplication.getObjectManager().makeFtsObject(id, FMAT_NAME, args.getAtoms());
    }
    ((FtsObjectWithEditor)fmat).requestOpenEditor();
  }
      
  public ValueInfo getValueInfo() 
  {
    return info;
  }
  
  public void updateLength(TrackEvent evt, SequenceGraphicContext gc)
  {
    FontMetrics fm = gc.getGraphicDestination().getFontMetrics( SequencePanel.rulerFont);
    String text = (String)varname;
    int width =  fm.stringWidth(text) + 6;
    int duration = ((FmatAdapter)gc.getAdapter()).getInvWidth(width);
    
    setProperty("duration", new Double(duration));
  }  
  
  static class FmatValueInfo extends AbstractValueInfo {
    /**
    * Returns the name of this value object */
    public String getName()
    {
      return FMAT_NAME;
    }
    
    public String getPublicName()
    {
      return FMAT_PUBLIC_NAME;
    }
    
    public ImageIcon getIcon()
    {
      return FMAT_ICON;
    }
    
    public Object newInstance()
    {
      return new FmatValue();
    }
    
    public DataFlavor getDataFlavor()
    {
      return FmatValueDataFlavor.getInstance();
    }
  }
  
  /**
    * Returns its specialized renderer (an AmbitusEventRenderer) */
  public SeqObjectRenderer getRenderer()
  {
    return FmatEventRenderer.getRenderer();
  }
  
  public String[] getLocalPropertyNames()
  {
    return localNameArray;
  }
  
  public int getLocalPropertyCount()
  {
    return localPropertyCount;
  }
  
  public Object[] getLocalPropertyValues()
  {
    for(int i = 0; i<localPropertyCount; i++)
      propertyValuesArray[i] = getProperty(localNameArray[i]);
    
    return propertyValuesArray;
  }
  
  public void setLocalPropertyValues(int nArgs, Object args[])
  {
    for(int i = 0; i<nArgs; i++)
      setProperty(localNameArray[i], args[i]);
  }
  
  //--- Fields
  public static final String fs = File.separator;
  public static final String FMAT_NAME = "fmat";
  public static final String FMAT_PUBLIC_NAME = "fmat";
  public static FmatValueInfo info = new FmatValueInfo();
  static String path;
  public static ImageIcon FMAT_ICON;
  static String localNameArray[] = null;
  static int localPropertyCount = 0;
  
  static 
  {
    path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;
    FMAT_ICON = new ImageIcon(path+"seq_fmat.gif");
  }
}
