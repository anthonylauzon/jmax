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


package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

/**
 * A Monodimensional view for a generic Sequence. 
 * This kind of editor use a MonoDimensionalAdapter
 * to map the y values. The value returned is always half of the panel,
 * and settings of y are simply ignored. */
public class MessageTrackEditor extends MonoTrackEditor
{
    public MessageTrackEditor(Geometry g, Track track)
    {
	super(g, track);
    
	((MonoDimensionalAdapter)gc.getAdapter()).setLabelMapper(MessageMapper.getMapper());
	
	setLayout(null);

	area = new JTextArea();
	area.setCursor( Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR));
	area.setEditable(true);
	area.setLineWrap(true);
	area.setWrapStyleWord(true);
	area.setBackground(Color.white);
	area.setForeground(Color.darkGray);
	area.setFont(MessageEventRenderer.stringFont);
	area.addKeyListener(new KeyListener(){
	    public void keyPressed(KeyEvent e){
		//if(!e.isActionKey())
		if(isEditKey(e))
		   resizeIfNeeded(e.getKeyChar());
	    }
	    public void keyReleased(KeyEvent e){}
	    public void keyTyped(KeyEvent e){}
	});
	add(area);
	validate();
    }
    
    boolean isEditKey(KeyEvent e)
    {
	return ((!e.isActionKey())&&
		(e.getKeyCode()!=KeyEvent.VK_SHIFT)&&(e.getKeyCode()!=KeyEvent.VK_CONTROL)&&
		(e.getKeyCode()!=KeyEvent.VK_DELETE)&&(e.getKeyCode()!=KeyEvent.VK_BACK_SPACE)&&
		(e.getKeyCode()!=KeyEvent.VK_ENTER));
    }

    public JPopupMenu getMenu()
    {
	MessageTrackPopupMenu.getInstance().update(this);
	return MessageTrackPopupMenu.getInstance();
    }

    void resizeIfNeeded(char val)
    {
	String text = area.getText()+val;
	FontMetrics fm = gc.getGraphicDestination().getFontMetrics(MessageEventRenderer.stringFont);
	int width = fm.stringWidth(text);
	Dimension d = area.getSize();
	if(width > d.width)
	    {
		d.width = width;
		area.setSize(d);
		//tempEvent.setProperty("message", text);
		//((TrackEvent)currentEvt).getRenderer().render(tempEvent, gc.getGraphicDestination().getGraphics(), true, gc);

		int lenght = gc.getAdapter().getInvWidth(width+4+MessageEventRenderer.BUTTON_WIDTH);
		currentEvt.setProperty("duration", new Integer(lenght));
	    }
    }

    void doEdit(Event evt)
    {
	isEditing = true;
	currentEvt = evt;
	PartitionAdapter a = ((PartitionAdapter)gc.getAdapter());

	/*tempEvent.setTime(currentEvt.getTime());
	  a.setY(tempEvent, a.getY(currentEvt));*/
	//tempEvent.setLocalProperties(currentEvt);

	FontMetrics fm = gc.getGraphicDestination().getFontMetrics(MessageEventRenderer.stringFont);
	String mess = a.getLabel(evt);

	int evtx = a.getX(evt)+MessageEventRenderer.BUTTON_WIDTH+2+1;
	int evty = a.getY(evt)+1;

	int evtLenght;
	if(!mess.equals(""))
	    evtLenght = fm.stringWidth(mess);
	else
	    evtLenght = MessageEventRenderer.DEFAULT_TEXT_WIDTH-5;
	    
	int evtHeight = fm.getHeight()-1;

	area.setBounds(evtx, evty, evtLenght, evtHeight);
	area.requestFocus();

	area.setText(a.getLabel(evt));
	area.setVisible(true);	
    }

    void setMessage()
    {
	if(currentEvt!=null)
	    ((MessageValue)currentEvt.getValue()).setText(area.getText(), (TrackEvent)currentEvt, gc);
	endEdit();
    }

    void endEdit()
    {
	area.setText("");
	area.setVisible(false);
	requestFocus();
	currentEvt = null;
	isEditing = false;
    }

    protected void processMouseEvent(MouseEvent e)
    {
	if (e.getClickCount() == 1)
	    if(isEditing) setMessage();
	super.processMouseEvent(e);
    }

    JTextArea area;
    boolean isEditing = false;
    Event currentEvt = null;
    //UtilTrackEvent tempEvent = new UtilTrackEvent(new MessageValue());
}

