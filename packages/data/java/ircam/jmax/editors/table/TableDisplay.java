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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.renderers.*;
import ircam.jmax.editors.table.menus.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A JPanel that is able to show the content of a TableDataModel.*/
public class TableDisplay extends PopupToolbarPanel
{

  /**
   * Constructor. */
  public TableDisplay(TablePanel tp)
  {
    super();

    panel = tp;
    setBackground(Color.white);
    popup = new TablePopupMenu(this);

    /******** display labels **********/
    JPanel labelPanel = new JPanel();
    labelPanel.setLayout( new BoxLayout( labelPanel, BoxLayout.X_AXIS));
    labelPanel.setOpaque(false);
	
    displayMouseLabel = new JLabel();
    displayMouseLabel.setFont(displayFont);
    displayMouseLabel.setPreferredSize(new Dimension(102, 15));
    displayMouseLabel.setMaximumSize(new Dimension(102, 15));
    displayMouseLabel.setMinimumSize(new Dimension(102, 15));
    
    infoLabel = new JLabel("", JLabel.RIGHT);
    infoLabel.setFont(displayFont);
    infoLabel.setPreferredSize(new Dimension(190, 15));
    infoLabel.setMaximumSize(new Dimension(190, 15));
    infoLabel.setMinimumSize(new Dimension(190, 15));
	
    labelPanel.add(displayMouseLabel);
    labelPanel.add(Box.createHorizontalGlue());
    labelPanel.add(infoLabel);

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    add(labelPanel);
    add(Box.createVerticalGlue());
    /*********************************/

    addMouseListener(new MouseListener(){
	public void mouseClicked(MouseEvent e){}
	public void mousePressed(MouseEvent e){}
	public void mouseReleased(MouseEvent e){}
	public void mouseEntered(MouseEvent e){}
	public void mouseExited(MouseEvent e){
	  gc.display("");
	}
      });
    addMouseMotionListener(new MouseMotionListener(){
	public void mouseMoved(MouseEvent e)
	{
	  if(getMenu().isVisible()) return;
      
	  if(!gc.getToolManager().getCurrentTool().getName().equals("zoomer"))
	    {
	      int index = gc.getAdapter().getInvX( e.getX());
	      int value = gc.getAdapter().getInvY( e.getY());
	      if( index < 0) index = 0;

	      if (index < gc.getDataModel().getSize() && index >=0)
		gc.display( "( "+index+" , "+value+" )");
	    }
	}
	public void mouseDragged(MouseEvent e)
	{
	  if(getMenu().isVisible()) return;
	  
	  String toolName = gc.getToolManager().getCurrentTool().getName();
	  if(!toolName.equals("zoomer"))
	    {
	      int index = gc.getAdapter().getInvX( e.getX());
	      int value = gc.getAdapter().getInvY( e.getY());
	      if( index < 0) index = 0;
	      
	      if (index < gc.getDataModel().getSize() && index >=0)
		gc.display( "( "+index+" , "+value+" )");
	    }
	  else
	    {
	      /*int start = gc.getAdapter().getInvX(x1);
		int end = gc.getAdapter().getInvX(x2);
	      
		if (end < tgc.getDataModel().getSize() && start>=0)
		gc.display("[ "+start+" -- "+end+" ]");*/

	      /*int start = gc.getLogicalTime();
		int end = start + gc.getTimeWindow();
		gc.display("[ "+start+" -- "+end+" ]");*/	      
	    }
	}
      });
  }

  /**
   * Constructor with a table render */
  public TableDisplay(TableRenderer tr, TableGraphicContext gc, TablePanel tp)
  {
    super();
    this.tr = tr;
    panel = tp;
    this.gc = gc;
    setBackground(Color.white);
    popup = new TablePopupMenu(this);
  }

  public JPopupMenu getMenu()
  {
      return popup;
  }

  /**
   * Specify which render to use for this TableDisplay */
  public void setRenderer(TableRenderer tr)
  {
    this.tr = tr;
  }

  public void setGraphicContext(TableGraphicContext tgc)
  {
      gc = tgc;
      gc.setDisplay( displayMouseLabel, infoLabel);
  }
  public TableGraphicContext getGraphicContext()
  {
      return gc;
  }
  /**
   * The paint routine just delegates to the RenderManager
   * the representation of its data.*/
  public void paintComponent(Graphics g) 
  {
      tr.render(g, g.getClipBounds());
  }
  
  /**
   * paints all the points in a given (closed) range */
  public void rangePaint(int index1, int index2)
  {
     Graphics g = getGraphics();

      for (int i = index1; i<= index2; i++)
	  tr.renderPoint(g, i);
    
      g.dispose();
  }
  
    /**
     * paints a single point */
  public void singlePaint(int index)
  {
      Graphics g = getGraphics();
      
      tr.renderPoint(g, index);
      g.dispose();
  }
  
  //--- Fields
  TableRenderer tr;
  TablePopupMenu popup;
  TableGraphicContext gc;
  public TablePanel panel;
  JLabel displayMouseLabel, infoLabel;

  static public Font displayFont = new Font("SansSerif", Font.PLAIN, 10);
}

