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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.lang.reflect.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

// import javax.swing.*;
import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;
import ircam.jmax.editors.patcher.actions.*;
import ircam.jmax.toolkit.*;

// The base class of all the graphic objects on the sketch.
// This class has a knowledge of its corrisponding fos object. It contains
// methods for:
// - painting, selecting, moving, dragging (services to the sketch)
// - create fos objects, change the value when the FTS value
// has changed (services to FTS), redefine itself.
// - sending values when the user interact with the object
// - handle the object with data and open the associated editor
// (example: subpatchers, table, etc.)

// A survivor...

abstract public class GraphicObject implements DisplayObject, Serializable
{
  public static class ObjectGeometry
  {
    // All this parameters can be changed without changing the geometry
    // of the patch, i.e. the connection positions, unless stated otherwise
	
    public static final int INOUTLET_WIDTH = 5;
    public static final int INOUTLET_HEIGHT = 3;
	
    public static final int HIGHLIGHTED_INOUTLET_HEIGHT = 6;
    public static final int HIGHLIGHTED_INOUTLET_WIDTH = 7;
	
    // The part of the highlight that go inside the 
    // Rectangle 

    public static final int HIGHLIGHTED_INLET_OVERLAP = 5;

    // The part of the highlight that go inside the 
    // Rectangle 

    public static final int HIGHLIGHTED_OUTLET_OVERLAP = 5;

    // PAD is the distance between the object border and the 
    // center of the inlet/outlet; CHANGE the Connection geometry

    public static final int INOUTLET_PAD = 4;

    // INLET_OVERLAP is the part of the inlet rectangle that
    // go inside the object

    public static final int INLET_OVERLAP = 2;

    // INLET_OFFSET is the vertical distance between the anchor point
    // and the object; CHANGE the Connection geometry

    public static final int INLET_OFFSET = 1;

    // OUTLET_OVERLAP is the part of the inlet rectangle that
    // go inside the object

    public static final int OUTLET_OVERLAP = 2;

    // OUTLET_OFFSET is the vertical distance between the anchor point
    // and the object; CHANGE the Connection geometry

    public static final int OUTLET_OFFSET = 0;

    // Sensibility areas 

    // Resize area 

    public static final int H_RESIZE_SENSIBLE_WIDTH = 4;
    public static final int V_RESIZE_SENSIBLE_HEIGHT = 5;

    // Inlet and outlets Max

    public static final int INOUTLET_MAX_SENSIBLE_AREA = 10;
 
    //dimension of in/outlet sensible area during connection
    
    public static final int INOUTLET_CONNECTED_SENSIBLE_AREA = 20;
  }

  // This two flags say if the object parts are sensible, and if the in/outlets are sensibles
  // global for all the objects.

  private static boolean followingLocations = false;
  private static boolean followingInOutletLocations = false;
  private static boolean followingInletLocations = false;
  private static boolean followingOutletLocations = false;

  static final int ERROR_MESSAGE_DISPLAY_PAD = ObjectGeometry.INOUTLET_PAD + ObjectGeometry.INOUTLET_WIDTH + 1;
  private transient static Font errorFont = new Font(ircam.jmax.Platform.FONT_NAME, Font.BOLD, ircam.jmax.Platform.FONT_SIZE);

  final public static void setFollowingInOutletLocations(boolean v)
  {
    followingInOutletLocations = v;
  }
  final public static void setFollowingInletLocations(boolean v)
  {
    followingInletLocations = v;
  }
  final public static void setFollowingOutletLocations(boolean v)
  {
    followingOutletLocations = v;
  }
  final public static void setFollowingLocations(boolean v)
  {
    followingLocations = v;
  }

  protected transient ErmesSketchPad itsSketchPad;

  protected transient FtsGraphicObject ftsObject = null;

  private transient boolean selected = false;

  private transient int inletDistance; // the distance between two inlets anchor point
  private transient int outletDistance; // the distance between two outlets anchor point
  public int getInletDistance(){return inletDistance;}
  public int getOutletDistance(){return outletDistance;}
  public void setInletDistance(int dist){inletDistance=dist;}
  public void setOutletDistance(int dist){outletDistance=dist;}

  protected Font itsFont = null;
  protected FontMetrics itsFontMetrics = null;
  
  public static final int ON_INLET  = 0;
  public static final int ON_OUTLET = 1;
  public static final int ON_OBJECT = 2;

  protected GraphicObject( FtsGraphicObject theFtsObject) 
  {
    ftsObject = theFtsObject;
    ftsObject.setObjectListener(this);

    selected = false;
      
    if(((FtsPatcherObject)ftsObject.getParent()).getEditorFrame() != null)
       setSketchPad( ((ErmesSketchWindow)((FtsPatcherObject)ftsObject.getParent()).getEditorFrame()).getSketchPad());
    else
      setDefaultFont();
  }

  public void setSketchPad(ErmesSketchPad sketch)
  {
    String fontName = null;
    int fontSize = -1;
    int fontStyle = -1;
    
    itsSketchPad = sketch;
    
    fontName = ftsObject.getFont();
    fontSize = ftsObject.getFontSize();
    fontStyle = ftsObject.getFontStyle();
    
    if (fontName == null)
      fontName = itsSketchPad.getDefaultFontName();
    
    if (fontSize < 0)      
      fontSize = itsSketchPad.getDefaultFontSize();
    if (fontStyle < 0)      
      fontStyle = itsSketchPad.getDefaultFontStyle();
    
    itsFont = FontCache.lookupFont(fontName, fontSize, fontStyle, itsSketchPad);
    itsFontMetrics = FontCache.lookupFontMetrics(fontName, fontSize, fontStyle, itsSketchPad);
    
    updateInOutlets();
  }
  
  void setDefaultFont()
  {
    itsFont = new Font(ircam.jmax.Platform.FONT_NAME, ircam.jmax.Platform.FONT_STYLE, ircam.jmax.Platform.FONT_SIZE);
    itsFontMetrics = Toolkit.getDefaultToolkit().getFontMetrics(itsFont);
  }
  
  // Destructor 

  public void delete()
  {
    itsSketchPad.getDisplayList().releaseConnectionsForObject(this);
    itsSketchPad.getDisplayList().remove(this);
    itsSketchPad.getFtsPatcher().removeObject( ftsObject);
    if( inspector != null) 
      {
	inspector.setVisible( false);
	inspector.dispose();
	inspector = null;
      }
    dispose();
  }
  
  public final int getX() 
  {
    return ScaleTransform.getInstance( itsSketchPad).scaleX(ftsObject.getX());
  }

  public void setX( int x) 
  {
    ftsObject.setX(ScaleTransform.getInstance( itsSketchPad).invScaleX(x));
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getY() 
  {
    return ScaleTransform.getInstance( itsSketchPad).scaleY(ftsObject.getY());
  }

  public void setY( int y) 
  {
    ftsObject.setY(ScaleTransform.getInstance( itsSketchPad).invScaleY(y));
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getWidth() 
  {
    return ScaleTransform.getInstance( itsSketchPad).scaleX(ftsObject.getWidth() + getVariableWidth());
  }

  public void setWidth( int w) 
  {
    if (w > 0)
      {
	ftsObject.setWidth(ScaleTransform.getInstance( itsSketchPad).invScaleX( w - getVariableWidth()));
	updateInOutlets();
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }
  public void setWidthShift( int w) 
  {
    setWidth(w);
  }

  /* redefined only in Standard object */
  public int getVariableWidth()
  {
    return 0;
  }

  public final int getHeight() 
  {
    if(isSquare())
      return ScaleTransform.getInstance( itsSketchPad).scaleX(ftsObject.getHeight());
    else
      return ScaleTransform.getInstance( itsSketchPad).scaleY(ftsObject.getHeight());
  }

  public void setHeight( int h) 
  {
    if (h > 0)
      {
	if(isSquare())
	  ftsObject.setHeight(ScaleTransform.getInstance( itsSketchPad).invScaleX(h));
	else
	  ftsObject.setHeight(ScaleTransform.getInstance( itsSketchPad).invScaleY(h));
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }

  public void setCurrentBounds(int x, int y, int w, int h)
  {
    if(( w <= 0)||( h <= 0))
      {
	ftsObject.setCurrentX( ScaleTransform.getInstance( itsSketchPad).invScaleX(x));
	ftsObject.setCurrentY( ScaleTransform.getInstance( itsSketchPad).invScaleY(y));
	setDefaults();
      }
    else
      ftsObject.setCurrentBounds( ScaleTransform.getInstance( itsSketchPad).invScaleX(x),
				  ScaleTransform.getInstance( itsSketchPad).invScaleY(y),
				  ScaleTransform.getInstance( itsSketchPad).invScaleX(w - getVariableWidth()),
				  isSquare() ? ScaleTransform.getInstance( itsSketchPad).invScaleX(h) 
				  : ScaleTransform.getInstance( itsSketchPad).invScaleY(h ));
    
  }

  public void setCurrentName( String name){}

  public void setDefaults(){}

  public boolean isSquare()
  {
    return false;
  }

  public void scale(float scaleX, float scaleY)
  {
    setX(Math.round(getX()*scaleX));
    setY(Math.round(getY()*scaleY));
    setWidth(Math.round(getWidth()*scaleX));
    setHeight(Math.round(getHeight()*scaleY));
  }

  // Special version that do not update the connections.

  protected void setHeightNoConnections( int h) 
  {
    if (h > 0)
      {
	setHeight(h);
      }
  }

  public Font getFont() 
  {
    return itsFont;
  }
  
  public String getFontName()
  {
    return itsFont.getName();
  }

  public int getFontSize()
  {
    return itsFont.getSize();
  }

  public int getFontStyle()
  {
    return itsFont.getStyle();
  }

  public FontMetrics getFontMetrics() 
  {
    return itsFontMetrics;
  }

  public void setFontName(String fontName)
  {
    setFont(FontCache.lookupFont(fontName, itsFont.getSize(), itsFont.getStyle(), itsSketchPad));
  }

  public void setFontSize(int size)
  {
    setFont(FontCache.lookupFont(itsFont.getName(), size, itsFont.getStyle(), itsSketchPad));
  }

  public void setFontStyle(int style)
  {
    setFont(FontCache.lookupFont(itsFont.getName(), itsFont.getSize(), style, itsSketchPad));
  }

  public void changeFontStyle(String style, boolean selected)
  {
    int fstyle = -1;
      
    if(style.equals("Bold"))
      {
	if(itsFont.isBold() && !selected)
	  if(itsFont.isItalic())  fstyle = Font.ITALIC;
	  else fstyle = Font.PLAIN;
	else
	  if(!itsFont.isBold() && selected)
	    if(!itsFont.isItalic())  fstyle = Font.BOLD;
	    else fstyle = Font.BOLD + Font.ITALIC; 
      }
    else
      if(style.equals("Italic"))
	{
	  if(itsFont.isItalic() && !selected)
	    if(itsFont.isBold())  fstyle = Font.BOLD;
	    else fstyle = Font.PLAIN;
	  else
	    if(!itsFont.isItalic() && selected)
	      if(!itsFont.isBold())  fstyle = Font.ITALIC;
	      else fstyle = Font.BOLD + Font.ITALIC;
	}

    if(fstyle!=-1)
      setFont(FontCache.lookupFont(itsFont.getName(), itsFont.getSize(), fstyle, itsSketchPad));
  }

  public void fontSmaller()
  {
    int size;
    
    size = itsFont.getSize();

    if (size > 8)
      setFont(FontCache.lookupFont(itsFont.getName(), size - 2, itsFont.getStyle(), itsSketchPad));
  }

  public void fontBigger()
  {
    setFont(FontCache.lookupFont(itsFont.getName(), itsFont.getSize() + 2, itsFont.getStyle(), itsSketchPad));
  }

  public void setFont( Font theFont) 
  {
    itsFont = theFont;
    itsFontMetrics = itsSketchPad.getFontMetrics( theFont);

    ftsObject.setFont(itsFont.getName());
    ftsObject.setFontSize(itsFont.getSize());
    ftsObject.setFontStyle(itsFont.getStyle());
  }

  public void setCurrentFont( String fontName, int fontSize, int fontStyle) 
  {
    if(fontName == null)
      fontName = itsSketchPad.getDefaultFontName();
    if( fontSize <= 0)
      fontSize = itsSketchPad.getDefaultFontSize();
    if( ( fontStyle != Font.PLAIN) && ( fontStyle != Font.ITALIC) && 
	( fontStyle != Font.BOLD) && (fontStyle != (Font.BOLD + Font.ITALIC)))
      fontStyle = itsSketchPad.getDefaultFontStyle();

    setCurrentFont( FontCache.lookupFont( fontName, fontSize, fontStyle, itsSketchPad));
  }

  public void setCurrentFont(Font font)
  {
    itsFont = font;
    itsFontMetrics = itsSketchPad.getFontMetrics( itsFont);

    ftsObject.setCurrentFontName( itsFont.getName());
    ftsObject.setCurrentFontSize( itsFont.getSize());
    ftsObject.setCurrentFontStyle( itsFont.getStyle());
  }

  public void fitToText()
  {
  }

  public int getOutletAnchorX(int outlet)
  {
    return getX() + ObjectGeometry.INOUTLET_PAD + outlet * outletDistance;
  }

  public int getOutletAnchorY(int outlet)
  {
    return getY() + getHeight();
  }

  public int getInletAnchorX(int inlet)
  {
    return getX() + ObjectGeometry.INOUTLET_PAD + inlet * inletDistance;
  }

  public int getInletAnchorY(int inlet)
  {
    return getY() - 1;
  }

  public void paintInlets(Graphics g)
  {
    int n = ftsObject.getNumberOfInlets();

    for ( int i = 0; i < n; i++)
      {
	int x, y;

	x = getInletAnchorX(i);
	y = getInletAnchorY(i);

	if (itsSketchPad.isHighlightedInlet(this, i))
	  {
	    int w = ObjectGeometry.HIGHLIGHTED_INOUTLET_WIDTH;
	    int h = ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT;
	    int xHL = x - w / 2;
	    int yHL = y + ObjectGeometry.HIGHLIGHTED_INLET_OVERLAP + ObjectGeometry.INLET_OFFSET;

	    g.fillRect( xHL, yHL - h, w, h - 3);
	    g.drawLine( x - 2, yHL - 3, x + 2, yHL - 3);
	    g.drawLine( x - 1, yHL - 2, x + 1, yHL - 2);
	    g.drawLine( x - 0, yHL - 1, x + 0, yHL - 1);
	  }
	else
	  {
	    int w = ObjectGeometry.INOUTLET_WIDTH;
	    int h = ObjectGeometry.INOUTLET_HEIGHT;

	    g.fillRect( x - w / 2, y - h + ObjectGeometry.INLET_OVERLAP + ObjectGeometry.INLET_OFFSET, w, h);
	  }
      }
  }

  public void paintOutlets(Graphics g)
  {
    int n = ftsObject.getNumberOfOutlets();

    for ( int i = 0; i < n; i++)
      {
	int x, y;

	x = getOutletAnchorX(i);
	y = getOutletAnchorY(i);

	if (itsSketchPad.isHighlightedOutlet(this, i))
	  {
	    int w = ObjectGeometry.HIGHLIGHTED_INOUTLET_WIDTH;
	    int h = ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT;
	    int xHL = x - w / 2;
	    int yHL = y - ObjectGeometry.HIGHLIGHTED_OUTLET_OVERLAP - ObjectGeometry.OUTLET_OFFSET;

	    g.drawLine( x - 0, yHL + 0, x + 0, yHL + 0);
	    g.drawLine( x - 1, yHL + 1, x + 1, yHL + 1);
	    g.drawLine( x - 2, yHL + 2, x + 2, yHL + 2);
	    g.fillRect( xHL, yHL + 3, w, h - 3);
	  }
	else
	  {
	    int w = ObjectGeometry.INOUTLET_WIDTH;
	    int h = ObjectGeometry.INOUTLET_HEIGHT;

	    g.fillRect( x - w / 2, y - ObjectGeometry.OUTLET_OVERLAP - ObjectGeometry.OUTLET_OFFSET, w, h);
	  }
      }
  }

  public void paint( Graphics g)
  {
    if(ftsObject.isError())
      g.setColor( Color.gray);
    else
      g.setColor( Color.black);	  

    g.drawRect( getX(), getY(), getWidth()-1, getHeight()-1);

    paintInlets(g);
    paintOutlets(g);
  }

  public void updatePaint( Graphics g){}  


  // Add one to cope with the inlet/outlets

  public void redraw()
  {
    if(itsSketchPad != null)
      itsSketchPad.repaint(getX(),
                           getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
                           ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP - 1,
                           getWidth(),
                           getHeight() + 2 * ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT  -
                           ObjectGeometry.INLET_OFFSET - ObjectGeometry.INLET_OVERLAP -
                           ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP + 2);
  }

  public void updateRedraw()
  {
    itsSketchPad.paintAtUpdateEnd(this, getX(),
                                  getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
                                  ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP,
                                  getWidth(),
                                  getHeight() + 2 * ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT  -
                                  ObjectGeometry.INLET_OFFSET - ObjectGeometry.INLET_OVERLAP -
                                  ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP);
  }

  public void redrawConnections()
  {
    itsSketchPad.getDisplayList().redrawConnectionsFor(this);
  }

  public void updateInOutlets()
  {
    if (ftsObject.getNumberOfInlets() > 1)
      inletDistance = (getWidth() - getVariableWidth() - 2 * ObjectGeometry.INOUTLET_PAD) / (ftsObject.getNumberOfInlets() - 1);
    
    if (ftsObject.getNumberOfOutlets() > 1)
      outletDistance = (getWidth() - getVariableWidth() - 2 * ObjectGeometry.INOUTLET_PAD) / (ftsObject.getNumberOfOutlets() - 1);
  }

  // redefine provide a default empty implementation
  // for the object that do not redefine themselves

  public void redefine(String text) 
  {
    updateInOutlets();
  }

  public void redefined(){}

  final public void setSelected(boolean v) 
  {
    selected = v;
  }

  final public boolean isSelected() 
  {
    return selected;
  }

  public final FtsGraphicObject getFtsObject() 
  {
    return ftsObject;
  }

  public ErmesSketchPad getSketchPad() 
  {
    return itsSketchPad;
  }
  
  // Utility functions for getSensibilityAreaAt

  private SensibilityArea makeInletArea(int mouseX, int mouseY, int n, int xcost)
  {
    final int inletsAnchorY  = getY() - 1;
    SensibilityArea area = SensibilityArea.get(this, Squeack.INLET);

    if ((mouseY < inletsAnchorY) || (mouseX < getX()) ||
	(mouseX > getX()+getWidth()) || (mouseY > inletsAnchorY+getHeight()))
      {
	area.setTransparent(true);
	area.setCost(xcost + Math.abs(mouseY - inletsAnchorY));
      }

    area.setNumber(n);

    return area;
  }

  private SensibilityArea makeOutletArea(int mouseX, int mouseY, int n, int xcost)
  {
    final int outletsAnchorY = getY() + getHeight();
    SensibilityArea area = SensibilityArea.get(this, Squeack.OUTLET);

    if ((mouseY > outletsAnchorY) || (mouseX < getX()) ||
	(mouseY < getY()-1) || (mouseX > getX()+getWidth()))
      {
	area.setTransparent(true);
	area.setCost(xcost + Math.abs(mouseY - outletsAnchorY));
      }

    area.setNumber(n);
    return area;
  }

  public SensibilityArea getSensibilityAreaAt( int mouseX, int mouseY)
  {
    if(itsSketchPad.getDisplayList().isDragLine())
      return getDrawingLineSensibilityAreaAt(mouseX, mouseY);
    else
      return getMouseMovingSensibilityAreaAt(mouseX, mouseY);
  }


  SensibilityArea getMouseMovingSensibilityAreaAt( int mouseX, int mouseY)
  {
    SensibilityArea area = null;
    final int x = getX();
    final int y = getY();
    final int w = getWidth();
    final int h = getHeight();
    final int verticalInOutletSensibility;
    final int horizontalInletSensibility;
    final int horizontalOutletSensibility;
    final int inletsAnchorY  = getY() - 1;
    final int outletsAnchorY = getY() + getHeight();
    final int nInlets = ftsObject.getNumberOfInlets();
    final int nOutlets = ftsObject.getNumberOfOutlets();

    if (nInlets == 1)
      horizontalInletSensibility  = Math.min(ObjectGeometry.INOUTLET_MAX_SENSIBLE_AREA, w / 2);
    else
      horizontalInletSensibility  = Math.min(ObjectGeometry.INOUTLET_MAX_SENSIBLE_AREA, inletDistance / 2);
    
    if (nOutlets == 1)
      horizontalOutletSensibility = Math.min(ObjectGeometry.INOUTLET_MAX_SENSIBLE_AREA, w / 2);
    else
      horizontalOutletSensibility = Math.min(ObjectGeometry.INOUTLET_MAX_SENSIBLE_AREA, outletDistance / 2);
    
    verticalInOutletSensibility = Math.min(ObjectGeometry.INOUTLET_MAX_SENSIBLE_AREA, h / 3);
    
    if (followingInOutletLocations)
      {
	// if the point is the vertical inlet zone,
	// check if the point in an inlet sensibility area

	if ((nInlets > 0) &&
	    (mouseY >= inletsAnchorY - verticalInOutletSensibility) &&
	    (mouseY <= inletsAnchorY + verticalInOutletSensibility))
	  {
	    int start = getInletAnchorX(0);
	    int d;

	    if ((nInlets == 1) || (mouseX < start))
	      {
		d = Math.abs(mouseX - start);
		if (d < horizontalInletSensibility)
		  return makeInletArea(mouseX, mouseY, 0, d);
	      }
	    else if (nInlets > 1)
	      {
		int n = (mouseX - start) / inletDistance;
		d = (mouseX - start) % inletDistance;
		if ((d < horizontalInletSensibility) && (n >= 0) && (n < nInlets))
		  return makeInletArea(mouseX, mouseY, n, d);
		else if ((d > inletDistance - horizontalInletSensibility) && (n >= 0) && (n < (nInlets - 1)))
		  return makeInletArea(mouseX, mouseY, n + 1, inletDistance - d);
	      }
	  }

	// if we have outlets, and the point is the vertical outlet zone,
	// check if the point in an outlet sensibility area

	if ((nOutlets > 0) &&
	    (mouseY >= outletsAnchorY - verticalInOutletSensibility) &&
	    (mouseY <= outletsAnchorY + verticalInOutletSensibility))
	  {
	    int start = getOutletAnchorX(0);
	    int d = 0, n = 0;

	    if (nOutlets == 1)
	      {
		n = 0;
		d = Math.abs(mouseX - start);
	      }
	    else if (nOutlets > 1)
	      {
		n = (mouseX - start) / outletDistance;
		d = Math.abs((mouseX - start) % outletDistance);
	      }

	    if (n == 0)
	      {
		if (d < horizontalOutletSensibility)
		  return makeOutletArea(mouseX, mouseY, 0, d);
	      }
	    else if (n > 0)
	      {
		if ((d < horizontalOutletSensibility) && (n >= 0) && (n < nOutlets))
		  return makeOutletArea(mouseX, mouseY, n, d);
		else if ((d > outletDistance - horizontalOutletSensibility) && (n >= 0) && (n < (nOutlets - 1)))
		  return makeOutletArea(mouseX, mouseY, n + 1, outletDistance - d);
	      }
	  }
      }

    if (followingLocations)
      {
	// Every other sensibility area is internal, so we check
	// now if the point is inside the rectangle, and return null if outside

	if ((mouseX < x) || (mouseX > (x + w)) ||
	    (mouseY < y) || (mouseY > (y + h)))
	  return null;

	// Check for horizantal resize area (assuming a point inside the rectangle)

	
	if( ( isResizable()) && ( mouseX >= x + w - ObjectGeometry.H_RESIZE_SENSIBLE_WIDTH 
				  && mouseY > y + ObjectGeometry.H_RESIZE_SENSIBLE_WIDTH
				  && mouseY < y + h - ObjectGeometry.H_RESIZE_SENSIBLE_WIDTH))
	  {
	    return SensibilityArea.get(this, Squeack.HRESIZE_HANDLE);
	  }

	// Try subclass specialized methods

	area = findSensibilityArea(mouseX, mouseY);

	if (area == null)
	  area = SensibilityArea.get(this, Squeack.OBJECT);
      }
    return area;
  }

  SensibilityArea getDrawingLineSensibilityAreaAt( int mouseX, int mouseY)
  {
    SensibilityArea area = null;

    if (followingInletLocations)
      area = getDrawingLineInletSensibilityArea(mouseX , mouseY);
    else 
      if (followingOutletLocations)
	area = getDrawingLineOutletSensibilityArea(mouseX , mouseY);
 
    return area;
  }

  SensibilityArea getDrawingLineInletSensibilityArea( int mouseX, int mouseY)
  {
    SensibilityArea area = null;
    final int verticalSensibility;
    final int horizontalSensibility;
    final int anchorY  = getY() - 1;
    final int nInlets = ftsObject.getNumberOfInlets();
    
    if( itsSketchPad.getConnectingObject()!=this)
      {	   	
	int d;
	int start = getInletAnchorX(0);

	if(pointInObject(mouseX, mouseY))//mouse in object
	  {
	    if (nInlets == 1){
	      d = Math.abs(mouseX - start);
	      return makeInletArea(mouseX, mouseY, 0, d);
	    }
	    else
	      if(nInlets > 0){
		horizontalSensibility = inletDistance / 2;

		int n = (mouseX - start) / inletDistance;
		d = (mouseX - start) % inletDistance;
		
		if ((d <= horizontalSensibility) && (n >= 0) && (n < nInlets))
		  return makeInletArea(mouseX, mouseY, n, d);
		else if ((d >= inletDistance - horizontalSensibility) && (n >= 0) && (n < (nInlets - 1)))
		  return makeInletArea(mouseX, mouseY, n + 1, inletDistance - d);
	      }
	  }
	else //mouse out of object 
	  {
	    verticalSensibility = ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA;
	    
	    if ((mouseY >= anchorY - verticalSensibility) &&
		(mouseY <= anchorY + verticalSensibility))
	      {
		if (nInlets == 1)
		  {
		    horizontalSensibility = ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA;
		    d = Math.abs(mouseX - start);
		    
		    if (d < horizontalSensibility)
		      return makeInletArea(mouseX, mouseY, 0, d);
		  }
		else
		  if(nInlets > 0)
		    {
		      if(mouseX < start)//left of first inlet
			{
			  horizontalSensibility = ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA;
			  d = Math.abs(mouseX - start);
		
			  if (d < horizontalSensibility)
			    return makeInletArea(mouseX, mouseY, 0, d);
			}
		      else
			{
			  horizontalSensibility = inletDistance / 2;
			  int n = (mouseX - start) / inletDistance;
			  d = (mouseX - start) % inletDistance;
			  
			  if ((d <= horizontalSensibility) && (n >= 0) && (n < nInlets))
			    return makeInletArea(mouseX, mouseY, n, d);
			  else if ((d >= inletDistance - horizontalSensibility) && 
				   (n >= 0) && (n < (nInlets - 1)))
			    return makeInletArea(mouseX, mouseY, n + 1, inletDistance - d);
			}
		      
		    }
	      }
	  }
      }
    return area;
  }

  
  SensibilityArea getDrawingLineOutletSensibilityArea( int mouseX, int mouseY)
  {
    SensibilityArea area = null;
    final int verticalSensibility;
    final int horizontalSensibility;
    final int anchorY = getY() + getHeight();
    final int nOutlets = ftsObject.getNumberOfOutlets();

    if(itsSketchPad.getConnectingObject()!=this)
      {	   

	int d;
	int start = getInletAnchorX(0);

	if(pointInObject(mouseX, mouseY))//mouse in object
	  {
	    if (nOutlets == 1){
	      d = Math.abs(mouseX - start);
	      return makeOutletArea(mouseX, mouseY, 0, d);
	    }
	    else
	      if(nOutlets > 0){
		horizontalSensibility = outletDistance / 2;

		int n = (mouseX - start) / outletDistance;
		d = (mouseX - start) % outletDistance;
		
		if ((d <= horizontalSensibility) && (n >= 0) && (n < nOutlets))
		  return makeOutletArea(mouseX, mouseY, n, d);
		else if ((d >= outletDistance - horizontalSensibility) && (n >= 0) && (n < (nOutlets - 1)))
		  return makeOutletArea(mouseX, mouseY, n + 1, outletDistance - d);
	      }
	  }
	else //mouse out of object 
	  {
	    verticalSensibility = ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA;

	    if ((mouseY >= anchorY - verticalSensibility) &&
		(mouseY <= anchorY + verticalSensibility))
	      {
		if (nOutlets == 1)
		  {
		    horizontalSensibility = ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA;
		    d = Math.abs(mouseX - start);
		    
		    if (d < horizontalSensibility)
		      return makeOutletArea(mouseX, mouseY, 0, d);
		  }
		else
		  if(nOutlets > 0)
		    {
		      if(mouseX < start)//left of first inlet
			{
			  horizontalSensibility = ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA;
			  d = Math.abs(mouseX - start);
			  
			  if (d < horizontalSensibility)
			    return makeOutletArea(mouseX, mouseY, 0, d);
			}
		      else
			{
			  horizontalSensibility = outletDistance / 2;
			  int n = (mouseX - start) / outletDistance;
			  d = (mouseX - start) % outletDistance;
			  
			  if ((d <= horizontalSensibility) && (n >= 0) && (n < nOutlets))
			    return makeOutletArea(mouseX, mouseY, n, d);
			  else 
			    if ((d >= outletDistance - horizontalSensibility) && 
				(n >= 0) && (n < (nOutlets - 1)))
			      return makeOutletArea(mouseX, mouseY, n + 1, outletDistance - d);
			}
		      
		    }
	      }
	  }
      }
    return area;
  }

  protected SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    return null;
  }

  int wherePopup = ON_OBJECT;
  public int wherePopup()
  {
    return wherePopup;
  }
  public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    String text = "";
    if(onInlet)
      {
	wherePopup = ON_INLET;
	text = "Disconnect Inlet n."+area.getNumber();
      }    
    else if(onOutlet)
      {
	wherePopup = ON_OUTLET; 
	text = "Disconnect Outlet n."+area.getNumber();
      }
    else
      {
	wherePopup = ON_OBJECT;
	text = "";
      }
    ObjectPopUp.update(wherePopup, text, isInspectable()); 
  }

  public boolean isPopUpVisible()
  {
      return ObjectPopUp.getInstance().isVisible();
  }
  public void popUpEdit(Point p)
  {
      ObjectPopUp.popup(itsSketchPad, this, p.x, p.y);
  }

  public void runModePopUpEdit(Point p)
  {
      JPopupMenu popup = getRunModePopUpMenu();
      if(popup!=null)
	   popup.show(itsSketchPad, p.x-2, p.y-2);
  }
  public JPopupMenu getRunModePopUpMenu()
  {
      return null;
  }
  public void popUpReset(){}

  // This method is called when we want to edit the object 
  // within this editor (its text content or equivalent)
  // The point argument give the mouse click location, when
  // it is usefull

  public void edit()
  {
    edit(null);
  }

  public void edit(Point p)
  {
  }

  // This method is called whenever we want to edit the content of
  // an object within a separate editor; do nothing by default

  public void editContent()
  {
  }
  public boolean hasContent()
  {
    return false;
  }

  // Squeack handling; note that squeacks delivered to objects
  // have the locations fields masked to zero, to simplify code writing.

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
  }

  public void moveBy( int dx, int dy) 
  {
    if (dx != 0)
      setX( getX() + dx);
    if (dy != 0)
      setY( getY() + dy);
  }

  public void resizing(boolean isResizing){}

  public boolean isResizable()
  {
    return true;
  }

  public boolean isInspectable()
  {
    return false;
  }

  ObjectInspector inspector = null;
  public void inspect()
  {
    if( isInspectable())
      {      
	if(inspector == null)
	  inspector = new ObjectInspector( this);
	else
	  inspector.relocateToObject();
      }
  }

  public void inspectionDone()
  {
    inspector.setVisible(false);
    inspector.dispose();
    inspector = null;    
  }

  public String getName()
  {
    return null;
  }

  public ObjectControlPanel getControlPanel(){ return null;}

  // Get the bounds in a rectangle

  public void getBounds(Rectangle bounds) 
  {
    bounds.x = getX();
    bounds.y = (getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
		ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP);
    bounds.width  = getWidth();
    bounds.height = (getHeight() + 2 * ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT  -
		     ObjectGeometry.INLET_OFFSET - ObjectGeometry.INLET_OVERLAP -
		     ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP);
  }

  // There are two intersect function: one cover the paint needs, and include all
  // the possible space covered by temporary details like highlighted inlets,
  // while the other, coreIntersect, only consider the core objetc, and it is 
  // used for selection

  public final boolean intersects(Rectangle r)
  {
    return !((r.x + r.width <= getX()) ||
	     (r.y + r.height <= (getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
				 ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP - 1)) ||
	     (r.x >= getX() + getWidth()) ||
	     (r.y >= (getY() + getHeight() + ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT -
		      ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP + 1)));
  }

  public final boolean coreIntersects(Rectangle r)
  {
    return !((r.x + r.width <= getX()) ||
	     (r.y + r.height <= getY()) ||
	     (r.x >= getX() + getWidth()) ||
	     (r.y >= (getY() + getHeight())));
  }

  public boolean pointInObject(int px, int py)
  {
    int x = getX();
    int y = getY();
    return ((px>=x)&&(px<=x+getWidth())&&(py>=y)&&(py<=y+getHeight()));
  }

  public void rectangleUnion(Rectangle r)
  {
    SwingUtilities.computeUnion(getX(), getY(), getWidth(), getHeight(), r);
  }

  final public void setLayer(int v)
  {
    ftsObject.setLayer(v);
  }

  final public int getLayer()
  {
    return ftsObject.getLayer();
  }

  // Called at GraphicObject disposal

  public void dispose()
  {
    ftsObject.setObjectListener(null);
  }

  public void showErrorDescription()
  {
    if ( ftsObject != null) 
      {
	int ax, ay, ah, aw;
	String annotation;
	String value;
	Graphics g;

	annotation = ftsObject.getErrorDescription();

	if (ftsObject.isError() && annotation != null)
	  {
            g = itsSketchPad.getGraphics();   

	    g.setColor( Color.red);
	      
	    paintInlets(g);
	    paintOutlets(g);
	      
	    g.drawRect( getX(), getY(), getWidth() - 1, getHeight() - 1);
	      
	    aw = itsSketchPad.getFontMetrics(errorFont).stringWidth( annotation) + 1;
	    ah = itsSketchPad.getFontMetrics(errorFont).getHeight();
	    ax = getX() + ERROR_MESSAGE_DISPLAY_PAD;
	    ay = getY() + ah / 4;
	      
	    g.setFont(errorFont);
	      
	    g.setColor( Color.white);
	    g.drawString( annotation, ax + 0, ay - 1);
	    g.drawString( annotation, ax + 0, ay + 1);
	    g.drawString( annotation, ax + -1, ay + 0);
	    g.drawString( annotation, ax + 1, ay + 0);
	      
	    g.setColor( Color.red);
	    g.drawString( annotation, ax + 0, ay + 0);
	      
	    g.dispose();
	  }
      }
  }

  // Assist code
  // Protected against repetitions of assist messages

  //private static FtsAtom assistArgs[] = null;

  static transient FtsObject lastAssistedObject;
  static transient int lastPosition;

  static final int ASSIST_OBJECT = 1;
  static final int ASSIST_INLET = 2;
  static final int ASSIST_OUTLET = 3;

  static int lastAssistOperation = 0;

  public boolean canDoAssist(int operation, int n)
  {
    boolean ret = false;

    if (itsSketchPad.isMessageReset())
      ret = true;
    else if (lastAssistOperation != operation)
      ret = true;
    else if (lastAssistedObject != ftsObject)
      ret = true;
    else if ((lastAssistOperation == ASSIST_INLET) || (lastAssistOperation == ASSIST_OUTLET))
      ret = (n != lastPosition);

    lastAssistOperation = operation;
    lastPosition = n;
    lastAssistedObject = ftsObject;

    return ret;
  }
  
  public void assistOnObject()
  {
    if (canDoAssist(ASSIST_OBJECT, 0))
      {
	  /*assistArgs[0].stringValue = "object";
	    ftsObject.sendMessage(-1, "assist", 1, assistArgs);*/
      }
  }

  public void assistOnInlet(int n)
  {
    if (canDoAssist(ASSIST_INLET, n))
      {
	  /*assistArgs[0].setString("inlet");
	    assistArgs[1].setInt(n);
	    ftsObject.sendMessage(-1, "assist", 2, assistArgs);*/
      }
  }

  public void assistOnOutlet(int n)
  {
    if (canDoAssist(ASSIST_OUTLET, n))
      {
	  /*assistArgs[0].setString("outlet");
	    assistArgs[1].setInt(n);
	    ftsObject.sendMessage(-1, "assist", 2, assistArgs);*/
      }
  }

  public String toString()
  {
    return "GraphicObject<" + ftsObject.toString() + ">";
  }

  /************** Undo/Redo *******************/
  int ux, uy, uw, uh;
  int rx, ry, rw, rh;
  Font uFont, rFont;

  public void setUndo()
  {
    ux = getX(); uy = getY(); uw = getWidth(); uh = getHeight();
    uFont = getFont();
  }
  public void setRedo()
  {
    rx = getX(); ry = getY(); rw = getWidth(); rh = getHeight();
    rFont = getFont();
  }
  public void undo()
  {
    if( uFont != getFont()) setFont( uFont);
    setX( ux); 
    setY( uy); 
    setWidth( uw); 
    setHeight( uh); 
  }
  public void redo()
  {
    if( rFont != getFont()) setFont( rFont);
    setX( rx); 
    setY( ry); 
    setWidth( rw); 
    setHeight( rh);
  }

  public boolean instantEdit()
  {
    return false;
  }
}
