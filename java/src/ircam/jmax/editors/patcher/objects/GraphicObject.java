 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;


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

abstract public class GraphicObject implements DisplayObject
{
  static class ObjectGeometry
  {
    // All this parameters can be changed without changing the geometry
    // of the patch, i.e. the connection positions, unless stated otherwise

    static final int INOUTLET_WIDTH = 5;
    static final int INOUTLET_HEIGHT = 3;

    static final int HIGHLIGHTED_INOUTLET_HEIGHT = 6;
    static final int HIGHLIGHTED_INOUTLET_WIDTH = 7;

    // The part of the highlight that go inside the 
    // Rectangle 

    static final int HIGHLIGHTED_INLET_OVERLAP = 5;

    // The part of the highlight that go inside the 
    // Rectangle 

    static final int HIGHLIGHTED_OUTLET_OVERLAP = 5;

    // PAD is the distance between the object border and the 
    // center of the inlet/outlet; CHANGE the Connection geometry

    static final int INOUTLET_PAD = 4;

    // INLET_OVERLAP is the part of the inlet rectangle that
    // go inside the object

    static final int INLET_OVERLAP = 2;

    // INLET_OFFSET is the vertical distance between the anchor point
    // and the object; CHANGE the Connection geometry

    static final int INLET_OFFSET = 1;

    // OUTLET_OVERLAP is the part of the inlet rectangle that
    // go inside the object

    static final int OUTLET_OVERLAP = 2;

    // OUTLET_OFFSET is the vertical distance between the anchor point
    // and the object; CHANGE the Connection geometry

    static final int OUTLET_OFFSET = 0;

    // Sensibility areas 

    // Resize area 

    static final int H_RESIZE_SENSIBLE_WIDTH = 4;
    static final int V_RESIZE_SENSIBLE_HEIGHT = 5;

    // Inlet and outlets Max

    static final int INOUTLET_MAX_SENSIBLE_AREA = 10;
 
    static final int INOUTLET_CONNECTED_SENSIBLE_AREA = 40;
  }


  // This two flags say if the object parts are sensible, and if the in/outlets are sensibles
  // global for all the objects.

  private static boolean followingLocations = false;
  private static boolean followingInOutletLocations = false;
  private static boolean followingInletLocations = false;
  private static boolean followingOutletLocations = false;

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

  protected ErmesSketchPad itsSketchPad;

  protected FtsObject ftsObject = null;

  private boolean selected = false;

  int inletDistance; // the distance between two inlets anchor point
  int outletDistance; // the distance between two outlets anchor point

  protected Font itsFont = null;
  protected FontMetrics itsFontMetrics = null;

  // A Static method that work as a virtual constructor;
  // given an FTS object, build the proper FTS Object

  static public GraphicObject makeGraphicObject( ErmesSketchPad sketch, FtsObject object) 
  {
    GraphicObject gobj;
    String theName = object.getClassName();

    if (theName.equals( "messbox"))
      gobj = new ircam.jmax.editors.patcher.objects.Message( sketch, object);
    else if (theName.equals( "button"))
      gobj = new ircam.jmax.editors.patcher.objects.Bang( sketch, object);
    else if (theName.equals( "toggle"))
      gobj = new ircam.jmax.editors.patcher.objects.Toggle( sketch, object);
    else if (theName.equals( "intbox"))
      gobj = new ircam.jmax.editors.patcher.objects.IntBox( sketch, object);
    else if (theName.equals( "floatbox"))
      gobj = new ircam.jmax.editors.patcher.objects.FloatBox( sketch, object);
    else if (theName.equals( "jcomment"))
      gobj = new ircam.jmax.editors.patcher.objects.Comment( sketch, object);
    else if ( theName.equals( "slider"))
      gobj = new ircam.jmax.editors.patcher.objects.Slider( sketch, object);
    else if (theName.equals( "inlet"))
      gobj = new ircam.jmax.editors.patcher.objects.Inlet( sketch, object);
    else if (theName.equals( "outlet"))
      gobj = new ircam.jmax.editors.patcher.objects.Outlet( sketch, object);
    else if (theName.equals( "jpatcher"))
      gobj = new ircam.jmax.editors.patcher.objects.Patcher( sketch, object);
    else
      gobj = new ircam.jmax.editors.patcher.objects.Standard( sketch, object);

    object.setObjectListener(gobj);

    return gobj;
  }

  protected GraphicObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    String fontName;
    int fontSize;

    itsSketchPad = theSketchPad;
    ftsObject = theFtsObject;

    selected = false;

    fontName = ftsObject.getFont();
    fontSize = ftsObject.getFontSize();

    if (fontName == null)
      fontName = itsSketchPad.getDefaultFontName();

    if (fontSize < 0)      
      fontSize = itsSketchPad.getDefaultFontSize();

    itsFont = FontCache.lookupFont(fontName, fontSize);
    itsFontMetrics = FontCache.lookupFontMetrics(fontName, fontSize);

    updateInOutlets();
  }

  // Destructor 

  public void delete()
  {
    itsSketchPad.getDisplayList().remove(this);
    dispose();
    ftsObject.delete();
  }
  
  public final int getX() 
  {
    return ftsObject.getX();
  }

  protected void setX( int x) 
  {
    ftsObject.setX(x);
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getY() 
  {
    return ftsObject.getY();
  }

  protected void setY( int y) 
  {
    ftsObject.setY(y);
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getWidth() 
  {
    return ftsObject.getWidth();
  }

  public void setWidth( int w) 
  {
    if (w > 0)
      {
	ftsObject.setWidth(w);
	updateInOutlets();
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }

  public final int getHeight() 
  {
    return ftsObject.getHeight();
  }

  public void setHeight( int h) 
  {
    if (h > 0)
      {
	ftsObject.setHeight(h);
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }

  // Special version that do not update the connections.

  protected void setHeightNoConnections( int h) 
  {
    if (h > 0)
      {
	ftsObject.setHeight(h);
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

  public FontMetrics getFontMetrics() 
  {
    return itsFontMetrics;
  }

  public void setFontName(String fontName)
  {
    setFont(FontCache.lookupFont(fontName, itsFont.getSize()));
  }

  public void setFontSize(int size)
  {
    setFont(FontCache.lookupFont(itsFont.getName(), size));
  }

  public void fontSmaller()
  {
    int size;
    
    size = itsFont.getSize();

    if (size > 8)
      setFont(FontCache.lookupFont(itsFont.getName(), size - 2));
  }

  public void fontBigger()
  {
    setFont(FontCache.lookupFont(itsFont.getName(), itsFont.getSize() + 2));
  }

  protected void setFont( Font theFont) 
  {
    itsFont = theFont;
    itsFontMetrics = itsSketchPad.getFontMetrics( theFont);

    ftsObject.setFont(itsFont.getName());
    ftsObject.setFontSize(itsFont.getSize());;
  }


  public final int getOutletAnchorX(int outlet)
  {
    return getX() + ObjectGeometry.INOUTLET_PAD + outlet * outletDistance;
  }

  public final int getOutletAnchorY(int outlet)
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

  private void paintInlets(Graphics g)
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

  private void paintOutlets(Graphics g)
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
    g.setColor( Color.black);

    paintInlets(g);
    paintOutlets(g);
    
    g.drawRect( getX(), getY(), getWidth()-1, getHeight()-1);
  }

  public void updatePaint( Graphics g){}  

  public void inspect() 
  {
  }

  // Add one to cope with the inlet/outlets

  public void redraw()
  {
    itsSketchPad.repaint(ftsObject.getX(),
			    ftsObject.getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
			 ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP - 1,
			 ftsObject.getWidth(),
			 ftsObject.getHeight() + 2 * ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT  -
			 ObjectGeometry.INLET_OFFSET - ObjectGeometry.INLET_OVERLAP -
			 ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP + 2);
  }

  public void updateRedraw()
  {
      itsSketchPad.paintAtUpdateEnd(this, ftsObject.getX(),
				    ftsObject.getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
				    ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP,
				    ftsObject.getWidth(),
				    ftsObject.getHeight() + 2 * ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT  -
				    ObjectGeometry.INLET_OFFSET - ObjectGeometry.INLET_OVERLAP -
				    ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP);
  }

  public void redrawConnections()
  {
    itsSketchPad.getDisplayList().redrawConnectionsFor(this);
  }

  protected void updateInOutlets()
  {
    if (ftsObject.getNumberOfInlets() > 1)
      inletDistance = (getWidth() - 2 * ObjectGeometry.INOUTLET_PAD) / (ftsObject.getNumberOfInlets() - 1);
    
    if (ftsObject.getNumberOfOutlets() > 1)
      outletDistance = (getWidth() - 2 * ObjectGeometry.INOUTLET_PAD) / (ftsObject.getNumberOfOutlets() - 1);
  }

  // redefine provide a default empty implementation
  // for the object that do not redefine themselves

  public void redefine(String text) 
  {
    updateInOutlets();
  }

  final public void setSelected(boolean v) 
  {
    selected = v;
  }

  final public boolean isSelected() 
  {
    return selected;
  }

  public final FtsObject getFtsObject() 
  {
    return ftsObject;
  }

  public ErmesSketchPad getSketchPad() 
  {
    return itsSketchPad;
  }
  
  // Utility functions for getSensibilityAreaAt

  private SensibilityArea makeInletArea(int mouseY, int n, int xcost)
  {
    final int inletsAnchorY  = getY() - 1;
    SensibilityArea area = SensibilityArea.get(this, Squeack.INLET);

    if (mouseY < inletsAnchorY)
      {
	area.setTransparent(true);
	area.setCost(xcost + Math.abs(mouseY - inletsAnchorY));
      }

    area.setNumber(n);

    return area;
  }

  private SensibilityArea makeOutletArea(int mouseY, int n, int xcost)
  {
    final int outletsAnchorY = getY() + getHeight();
    SensibilityArea area = SensibilityArea.get(this, Squeack.OUTLET);

    if (mouseY > outletsAnchorY)
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
      return getDraggingLineSensibilityAreaAt(mouseX, mouseY);
    else
      return getMouseMovingSensibilityAreaAt(mouseX, mouseY);
  }



  SensibilityArea getMouseMovingSensibilityAreaAt( int mouseX, int mouseY)
  {
    SensibilityArea area = null;
    final int x = ftsObject.getX();
    final int y = ftsObject.getY();
    final int w = ftsObject.getWidth();
    final int h = ftsObject.getHeight();
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
		  return makeInletArea(mouseY, 0, d);
	      }
	    else if (nInlets > 1)
	      {
		int n = (mouseX - start) / inletDistance;
		d = (mouseX - start) % inletDistance;
		if ((d < horizontalInletSensibility) && (n >= 0) && (n < nInlets))
		  return makeInletArea(mouseY, n, d);
		else if ((d > inletDistance - horizontalInletSensibility) && (n >= 0) && (n < (nInlets - 1)))
		  return makeInletArea(mouseY, n + 1, inletDistance - d);
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
		  return makeOutletArea(mouseY, 0, d);
	      }
	    else if (n > 0)
	      {
		if ((d < horizontalOutletSensibility) && (n >= 0) && (n < nOutlets))
		  return makeOutletArea(mouseY, n, d);
		else if ((d > outletDistance - horizontalOutletSensibility) && (n >= 0) && (n < (nOutlets - 1)))
		  return makeOutletArea(mouseY, n + 1, outletDistance - d);
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

	if ( mouseX >= x + w - ObjectGeometry.H_RESIZE_SENSIBLE_WIDTH 
	     && mouseY > y + ObjectGeometry.H_RESIZE_SENSIBLE_WIDTH
	     && mouseY < y + h - ObjectGeometry.H_RESIZE_SENSIBLE_WIDTH)
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

  SensibilityArea getDraggingLineSensibilityAreaAt( int mouseX, int mouseY)
  {
    SensibilityArea area = null;
    final int x = ftsObject.getX();
    final int y = ftsObject.getY();
    final int w = ftsObject.getWidth();
    final int h = ftsObject.getHeight();
    final int verticalInOutletSensibility;
    final int horizontalInletSensibility;
    final int horizontalOutletSensibility;
    final int inletsAnchorY  = getY() - 1;
    final int outletsAnchorY = getY() + getHeight();
    final int nInlets = ftsObject.getNumberOfInlets();
    final int nOutlets = ftsObject.getNumberOfOutlets();

    if(itsSketchPad.getConnectingObject()!=this){	   

      verticalInOutletSensibility = Math.max(ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA, h / 2);
      
      if (followingInletLocations)
	{	
	  // if the point is the vertical inlet zone,
	  // check if the point in an inlet sensibility area
	  if (nInlets == 1)
	    horizontalInletSensibility  = Math.max(ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA, w / 2);
	  else
	    horizontalInletSensibility  = Math.min(ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA, inletDistance / 2);
	  
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
		    return makeInletArea(mouseY, 0, d);
		}
	      else if (nInlets > 1)
		{
		  int n = (mouseX - start) / inletDistance;
		  d = (mouseX - start) % inletDistance;
		  
		  if ((d < horizontalInletSensibility) && (n >= 0) && (n < nInlets))
		    return makeInletArea(mouseY, n, d);
		  else if ((d > inletDistance - horizontalInletSensibility) && (n >= 0) && (n < (nInlets - 1)))
		    return makeInletArea(mouseY, n + 1, inletDistance - d);
		}
	    }
	}
      // if we have outlets, and the point is the vertical outlet zone,
      // check if the point in an outlet sensibility area
      
      if (followingOutletLocations)
	{
	  if (nOutlets == 1)
	    horizontalOutletSensibility = Math.min(ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA, w / 2);
	  else
	    horizontalOutletSensibility = Math.min(ObjectGeometry.INOUTLET_CONNECTED_SENSIBLE_AREA, outletDistance / 2);
	  
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
		    return makeOutletArea(mouseY, 0, d);
		}
	      else if (n > 0)
		{
		  if ((d < horizontalOutletSensibility) && (n >= 0) && (n < nOutlets))
		    return makeOutletArea(mouseY, n, d);
		  else if ((d > outletDistance - horizontalOutletSensibility) && (n >= 0) && (n < (nOutlets - 1)))
		    return makeOutletArea(mouseY, n + 1, outletDistance - d);
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

  // This method is called to edit the object by means of a popup
  // structure

  public void popUpEdit(Point p)
  {
  }

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
      setX( ftsObject.getX() + dx);

    if (dy != 0)
      setY( ftsObject.getY() + dy);
  }

  // Get the bounds in a rectangle

  public void getBounds(Rectangle bounds) 
  {
    bounds.x = ftsObject.getX();
    bounds.y = (ftsObject.getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
		ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP);
    bounds.width  = ftsObject.getWidth();
    bounds.height = (ftsObject.getHeight() + 2 * ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT  -
		     ObjectGeometry.INLET_OFFSET - ObjectGeometry.INLET_OVERLAP -
		     ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP);
  }

  // There are two intersect function: one cover the paint needs, and include all
  // the possible space covered by temporary details like highlighted inlets,
  // while the other, coreIntersect, only consider the core objetc, and it is 
  // used for selection

  public final boolean intersects(Rectangle r)
  {
    return !((r.x + r.width <= ftsObject.getX()) ||
	     (r.y + r.height <= (ftsObject.getY() - ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT +
				 ObjectGeometry.INLET_OFFSET + ObjectGeometry.INLET_OVERLAP - 1)) ||
	     (r.x >= ftsObject.getX() + ftsObject.getWidth()) ||
	     (r.y >= (ftsObject.getY() + ftsObject.getHeight() + ObjectGeometry.HIGHLIGHTED_INOUTLET_HEIGHT -
		      ObjectGeometry.OUTLET_OFFSET - ObjectGeometry.OUTLET_OVERLAP + 1)));
  }


  public final boolean coreIntersects(Rectangle r)
  {
    return !((r.x + r.width <= ftsObject.getX()) ||
	     (r.y + r.height <= ftsObject.getY()) ||
	     (r.x >= ftsObject.getX() + ftsObject.getWidth()) ||
	     (r.y >= (ftsObject.getY() + ftsObject.getHeight())));
  }


  public void rectangleUnion(Rectangle r)
  {
    SwingUtilities.computeUnion(ftsObject.getX(), ftsObject.getY(),
				ftsObject.getWidth(), ftsObject.getHeight(), r);
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

	if (annotation != null)
	  {
	    ax = ftsObject.getX() + ftsObject.getWidth() / 2;
	    ay = ftsObject.getY() + ftsObject.getHeight() / 2;
	    aw = itsFontMetrics.stringWidth( annotation);
	    ah = itsFontMetrics.getHeight();

	    g = itsSketchPad.getGraphics();

	    g.setColor( Color.white);
	    g.fillRect( ax - 1 , ay - ah - 1, aw + 2, ah + 2);

	    g.setColor( Color.black);
	    g.drawRect( ax - 1, ay - ah - 1, aw + 2, ah + 2);
	    g.setFont(itsFont);
	    g.drawString( annotation, ax, ay);
	    g.dispose();
	  }
      }
  }

  // Assist code
  // Protected against repetitions of assist messages

  static MaxVector assistArgs = new MaxVector();

  static FtsObject lastAssistedObject;
  static int lastPosition;

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
	assistArgs.removeAllElements();

	assistArgs.addElement("object");
	ftsObject.sendMessage(-1, "assist", assistArgs);
      }
  }

  public void assistOnInlet(int n)
  {
    if (canDoAssist(ASSIST_INLET, n))
      {
	assistArgs.removeAllElements();

	assistArgs.addElement("inlet");
	assistArgs.addElement(new Integer(n));
	ftsObject.sendMessage(-1, "assist", assistArgs);
      }
  }

  public void assistOnOutlet(int n)
  {
    if (canDoAssist(ASSIST_OUTLET, n))
      {
	assistArgs.removeAllElements();

	assistArgs.addElement("outlet");
	assistArgs.addElement(new Integer(n));
	ftsObject.sendMessage(-1, "assist", assistArgs);
      }
  }

  public String toString()
  {
    return "GraphicObject<" + ftsObject.toString() + ">";
  }
}






