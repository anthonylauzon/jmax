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


package ircam.jmax.editors.bpf;

import ircam.jmax.editors.bpf.renderers.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.beans.*;
/**
 * A Monodimensional view for a generic Sequence. 
 * This kind of editor use a MonoDimensionalAdapter
 * to map the y values. The value returned is always half of the panel,
 * and settings of y are simply ignored. */
public class BpfEditor extends PopupToolbarPanel implements ListSelectionListener
{
    public BpfEditor(Geometry g, FtsBpfObject model, BpfToolManager manager)
    {
	super();

	this.geometry = g;
	this.model = model;

	model.addBpfListener(new BpfDataListener() {
		public void pointsDeleted(int[] oldIndexs){BpfEditor.this.repaint();}
		public void pointAdded(int index) {
		    BpfEditor.this.repaint();			
		}
		public void pointChanged(int oldndex, int newIndex, float newTime, float newValue) {
		    BpfEditor.this.repaint();
		}
		public void pointsChanged() {
		    BpfEditor.this.repaint();
		}
		public void cleared(){BpfEditor.this.repaint();}
	    });

	geometry.addTranspositionListener(new TranspositionListener() {
		public void transpositionChanged(int newTranspose)
		{
		    repaint();
		}
	    });

	createGraphicContext(geometry, model, manager);

	gc.getSelection().addListSelectionListener(this);

	setBackground(Color.white);

	setOpaque(false);

	bpfPopupMenu = new BpfPopupMenu(this);

	manager.addToolListener(new ToolListener() {
		public void toolChanged(ToolChangeEvent e) 
		{		
		    if (e.getTool() != null) 
			setCursor(e.getTool().getCursor());
		}
	    });
    }

    public void reinit(){}
    
    public JMenu getToolsMenu()
    {
	return gc.getToolManager().getMenu();
    }
    
    public JPopupMenu getMenu()
    {
	return bpfPopupMenu;
    }

    public void paintComponent(Graphics g) 
    {
	Rectangle r = g.getClipBounds();
	renderer.render(g, r);	
    }

    private void createGraphicContext(Geometry geometry, FtsBpfObject model, BpfToolManager manager)
    {
	gc = new BpfGraphicContext(model, new BpfSelection(model)); //loopback?
	gc.setGraphicSource(this);
	gc.setGraphicDestination(this);
	ad = new BpfAdapter(geometry, gc);
	gc.setAdapter(ad);

	renderer = new BpfRenderer(gc);
	gc.setRenderManager(renderer);
	gc.setToolManager(manager);
    }

    public void setAdapter(BpfAdapter adapter)
    {
	gc.setAdapter(adapter);	
	ad = adapter;
    }

    public void setRenderer(BpfRenderer renderer)
    {
	this.renderer = renderer;
	gc.setRenderManager(renderer);
    }

    /**
     * ListSelectionListener interface
     */
    
    public void valueChanged(ListSelectionEvent e)
    {
	repaint();
    }
    
    public void showListDialog()
    {
	if(listDialog==null) 
	    createListDialog();
	else
	    listDialog.relocate();
	listDialog.setVisible(true);
    }

    private void createListDialog()
    {
	listDialog = new BpfTableDialog(gc.getFrame(), gc);
    }

    public void updateNewObject(Object obj){};

    void updateEventProperties(Object whichObject, String propName, Object propValue){}

    void updateRange(Object whichObject){}    
    /**
     * Track editor interface */

    public Component getComponent()
    {
	return this;
    }

    public BpfGraphicContext getGraphicContext()
    {
	return gc;
    }

    public int getDefaultHeight()
    {
	return DEFAULT_HEIGHT;
    }

    public void dispose()
    {
	if(listDialog != null)
	    listDialog.dispose();
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(Bpf.DEFAULT_WIDTH, DEFAULT_HEIGHT);
    }

    public void processKeyEvent(KeyEvent e)
    {
	if(isDeleteKey(e))
	    {
		if(e.getID()==KeyEvent.KEY_PRESSED)
		    {
			((UndoableData)model).beginUpdate();
			gc.getSelection().deleteAll();
		    }
	    }
	else if((e.getKeyCode() == KeyEvent.VK_TAB)&&(e.getID()==KeyEvent.KEY_PRESSED))
	    if(e.isControlDown())
		gc.getSelection().selectPrevious();
	    else
		gc.getSelection().selectNext();
	  
	super.processKeyEvent(e);
	requestFocus();
    }

    public static boolean isDeleteKey(KeyEvent e)
    {
	return ((e.getKeyCode() == KeyEvent.VK_DELETE)||(e.getKeyCode() == KeyEvent.VK_BACK_SPACE));
    }

    //--- BpfEditor fields
    Geometry geometry;
    BpfGraphicContext gc;
    FtsBpfObject model;
    BpfPopupMenu bpfPopupMenu;
    static int MONODIMENSIONAL_TRACK_OFFSET = 0;
    static public int DEFAULT_HEIGHT = /*127*/Bpf.DEFAULT_HEIGHT - 30;
    BpfRenderer renderer;
    BpfAdapter ad;

    MaxVector oldElements = new MaxVector();
    BpfTableDialog listDialog = null;
}





