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

package ircam.jmax.editors.sequence.renderers;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.toolkit.*;

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

      if (heigth == 0)
	  heigth = Adapter.NOTE_DEFAULT_HEIGTH;

      y = y-heigth/2;

      switch(state)
      {
      case Event.SELECTED:
	  g.setColor(Color.red);
	  if (SequenceSelection.getCurrent().getModel() != gc.getDataModel()) g.drawRect(x, y, lenght, heigth);
	  else g.fillRect(x, y, lenght, heigth);
	  break;
      case Event.DESELECTED:
	  g.setColor(Color.black);
	  g.fillRect(x, y, lenght, heigth);
	  break;
      case Event.HIGHLIGHTED:
	  g.setColor(Color.green);
	  g.fillRect(x, y, lenght, heigth);
	  break;
      }

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
      if(pa.isDisplayLabels())
      {
	  g.setFont(SequencePanel.rulerFont);
	  g.drawString(label, x, y-5);
      }
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

    return  (evtx<=x && (evtx+evtlenght >= x) && evty-evtheigth/2<=y && (evty+evtheigth/2) >= y);
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

    tempRect.setBounds(x, y, w, h);
    eventRect.setBounds(evtx, evty, evtlenght, evtheigth);
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
    
    int oldX, oldY;
    //static public Color selectionColor = new Color(150,90,/*153*/253);
}

