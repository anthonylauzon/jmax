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

package ircam.jmax.editors.sequence.renderers;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.toolkit.*;
import ircam.fts.client.*;

import java.awt.*;

/**
 * The piano-roll event renderer in a Score with an ambitus: the line-based event, 
 * with a lenght , variable width, black color, a label.
 */
public class AmbitusEventRenderer implements SeqObjectRenderer {

  public AmbitusEventRenderer()
    {
    } 

  /**
   * constructor.
   */
  public AmbitusEventRenderer(SequenceGraphicContext theGc) 
  {
    gc = theGc;
  }

  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
  public void render(Object obj, Graphics g, boolean selected)
  {
    render(obj, g, selected, gc);
  } 

  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
  public void render(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
  {
    if(((Event)obj).isHighlighted())
      render(obj, g, Event.HIGHLIGHTED, theGc); 
    else
      if(selected)
	render(obj, g, Event.SELECTED, theGc); 
      else
	render(obj, g, Event.DESELECTED, theGc); 
  }

  public void render(Object obj, Graphics g, int state, GraphicContext theGc) 
  {
    Event e = (Event) obj;
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;
    PartitionAdapter pa = (PartitionAdapter)gc.getAdapter();
    
    int x = pa.getX(e);
    int y = pa.getY(e);
    int lenght = pa.getLenght(e);
    String label = pa.getLabel(e);
    int heigth = pa.getHeigth(e);
    String type = pa.getType(e);

    if (heigth == 0)
      heigth = Adapter.NOTE_DEFAULT_HEIGTH;

    if( type.equals("rest"))
      {
	y = y-heigth/2;

	Color col, bordCol;
	switch(state)
	  {
	  default:
	  case Event.SELECTED:
	    col = restSelColor;
	    bordCol = Color.red;
	    break;
	  case Event.DESELECTED:
	    col = restColor;
	    bordCol = Color.black;
	    break;
	  case Event.HIGHLIGHTED:
	    col = restHighlightColor;
	    bordCol = Color.green;
	    break;
	  }

	g.setColor( col);
	g.fillRect(x, y, lenght, heigth);
	g.setColor( bordCol);
	g.drawLine(x, y, x, y + heigth - 1);
	g.drawLine(x + 1, y, x + 1, y + heigth - 1);
	g.drawLine(x + lenght, y, x + lenght, y + heigth - 1);
	g.drawLine(x + lenght - 1, y, x + lenght - 1, y + heigth - 1);
	g.fillRect(x , y + heigth/2 - 1, lenght, /*2*/ 3);
      }
    else
      {
	y = y-heigth+2;

	switch(state)
	  {
	  case Event.SELECTED:
	    //g.setColor(Color.red);
	    g.setColor( selectedColor);
	    break;
	  case Event.DESELECTED:
	    g.setColor( deselectedColor);
	    break;
	  case Event.HIGHLIGHTED:
	    g.setColor( highlightColor);
	    break;
	  }

	if( type.equals("trill"))
	  {
	    double tw = 0.5 * (double)lenght / Math.ceil((double)lenght / 12.0);
	    double i = 0;
	    while(i < lenght)
	      {
		int d = (int)i;
		i += tw;
		
		g.drawLine(x + d, y + heigth, x + (int)i, y);
		g.drawLine(x + 1 + d, y + heigth, x + 1 + (int)i, y);
		
		d = (int)i;
		i += tw;
		
		g.drawLine( x + d, y, x + (int)i, y + heigth);
		g.drawLine( x + 1 + d, y, x + 1 + (int)i, y + heigth);
	      }
	  }
	else
	  if ( gc.getSelection().getModel() != gc.getDataModel()) 
	    g.drawRect(x, y, lenght, heigth);
	  else 
	    g.fillRect(x, y, lenght, heigth);    
      
	if(pa.getViewMode()==MidiTrackEditor.NMS_VIEW)
	  {
	    int alt = pa.getAlteration(e);
	    g.setFont(altFont);
	    switch(alt)
	      {
	      case PartitionAdapter.ALTERATION_DIESIS:
		g.drawString("#", x-8, y+5);
		break;
	      case PartitionAdapter.ALTERATION_BEMOLLE:
		g.drawString("b", x-8, y+5);
	      }
	  }	
      }

    if( !label.equals(""))
      {
	g.setFont(SequencePanel.rulerFont);
	g.drawString(label, x, y-5);
      }
  }

  public void renderBounds(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
  {
    Event e = (Event) obj;
    PartitionAdapter pa = (PartitionAdapter)((SequenceGraphicContext) theGc).getAdapter();
    
    int x = pa.getX(e);
    int y = pa.getY(e);
    int lenght = pa.getLenght(e);
    int heigth = pa.getHeigth(e);
    String type = pa.getType(e);

    if (heigth == 0)
      heigth = Adapter.NOTE_DEFAULT_HEIGTH;

    if( type.equals("rest"))
      y = y-heigth/2;
    else
      y = y-heigth+2;
    
    if( selected)
      g.setColor(Color.red);
    else
      g.setColor(Color.black);

    g.drawRect( x, y, lenght, heigth);
  }
  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(Object obj, int x, int y) 
  {
    return contains(obj, x, y, gc);
  }

  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(Object obj, int x, int y, GraphicContext theGc) 
  {
    Event e = (Event) obj;
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int evtx = gc.getAdapter().getX(e);
    int evty = gc.getAdapter().getY(e);
    int evtlenght = gc.getAdapter().getLenght(e);
    int evtheigth = gc.getAdapter().getHeigth(e);
    String type = ((PartitionAdapter)gc.getAdapter()).getType(e);

    if( type.equals("rest"))
      return (evtx<=x && (evtx+evtlenght >= x) && evty-evtheigth/2<=y && evty + evtheigth/2 >= y);
    else
      return (evtx<=x && (evtx+evtlenght >= x) && evty-evtheigth+2<=y && evty+2 >= y);
  }


  Rectangle eventRect = new Rectangle();
  Rectangle tempRect = new Rectangle();

  /**
   * returns true if the representation of the given event "touches" the given rectangle
   */
  public boolean touches(Object obj, int x, int y, int w, int h)
  {
    return touches(obj, x, y, w, h, gc);
  } 

  /**
   * returns true if the representation of the given event "touches" the given rectangle
   */
  public boolean touches(Object obj, int x, int y, int w, int h, GraphicContext theGc) 
  {
    Event e = (Event) obj;
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int evtx = gc.getAdapter().getX(e);
    int evty = gc.getAdapter().getY(e);
    int evtlenght = gc.getAdapter().getLenght(e);
    int evtheigth = gc.getAdapter().getHeigth(e);
    String type = ((PartitionAdapter)gc.getAdapter()).getType(e);

    tempRect.setBounds(x, y, w, h);

    if( type.equals("rest"))
      eventRect.setBounds(evtx, evty-evtheigth/2, evtlenght, evtheigth);
    else
      eventRect.setBounds(evtx, evty-evtheigth+2, evtlenght, evtheigth);
    
    return  tempRect.intersects(eventRect);
  }

  public static AmbitusEventRenderer getRenderer()
  {
    if (staticInstance == null)
      staticInstance = new AmbitusEventRenderer();
    
    return staticInstance;
  }

  //------------Fields
  final static int NOTE_DEFAULT_WIDTH = 5;
  public SequenceGraphicContext gc;
  public static AmbitusEventRenderer staticInstance;
  static public Font altFont = new Font("SansSerif", Font.BOLD, 12);
  
  Color restColor = new Color(165, 165, 165, 60);
  Color restSelColor = new Color(255, 0, 0, 60);
  Color restHighlightColor = new Color(0, 255, 0, 60);
  Color deselectedColor = new Color( 0, 0, 0, 180);
  Color selectedColor = new Color(255, 0, 0, 180);
  Color highlightColor = new Color(0, 255, 0, 180);

  int oldX, oldY;
}

