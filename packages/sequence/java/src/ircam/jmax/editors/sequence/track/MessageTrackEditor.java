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
		if(isEditKey(e))
		    resizeIfNeeded(e, area.getCaretPosition());
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
		(e.getKeyCode()!=KeyEvent.VK_SHIFT)&&(e.getKeyCode()!=KeyEvent.VK_CONTROL));
    }

    boolean isDeleteKey(KeyEvent e)
    {
	return ((e.getKeyCode() == KeyEvent.VK_DELETE)||(e.getKeyCode() == KeyEvent.VK_BACK_SPACE));
    }
    boolean isReturnKey(KeyEvent e)
    {
	return (e.getKeyCode() == KeyEvent.VK_ENTER);
    }

    public JPopupMenu getMenu()
    {
	MessageTrackPopupMenu.getInstance().update(this);
	return MessageTrackPopupMenu.getInstance();
    }

    void resizeIfNeeded(KeyEvent e, int pos)
    {
	String text = area.getText();
	char newChar = e.getKeyChar();

	String str1, str2;

	if(isDeleteKey(e)&&(pos>0))
	    {
		str1 = text.substring(0, pos-1);  
		str2 = text.substring(pos);  
		text = str1+str2;
	    }
	else
	    {
		str1 = text.substring(0, pos);  
		str2 = text.substring(pos);  
		if(isReturnKey(e))
		    text = str1+newChar+" "+str2;
		else
		    text = str1+newChar+str2;
	    }
	int width = TextRenderer.getRenderer().getTextWidth(text, gc);
	int height = TextRenderer.getRenderer().getTextHeight(text, gc);
	Dimension d = area.getSize();
	  
	if((width > d.width)||(height > d.height)||(height<d.height-10))
	    {
		if(width < MessageEventRenderer.MINIMUM_WIDTH) 
		   width  = MessageEventRenderer.MINIMUM_WIDTH;
		d.width = width;

		if(height < MessageValue.DEFAULT_HEIGHT) 
		    height = MessageValue.DEFAULT_HEIGHT;		
		d.height = height;

		area.setSize(d);

		int lenght = gc.getAdapter().getInvWidth(width+4+MessageEventRenderer.BUTTON_WIDTH);		
		currentEvt.setProperty("duration", new Integer(lenght));
		currentEvt.setProperty("height", new Integer(height));
	    }
    }

    void doEdit(Event evt, int x, int y)
    {
	isEditing = true;
	currentEvt = evt;
	PartitionAdapter a = ((PartitionAdapter)gc.getAdapter());

	String text = a.getLabel(evt);

	int evtx = a.getX(evt)+MessageEventRenderer.BUTTON_WIDTH+2+1;
	int evty = a.getY(evt)+1;

	int evtLenght, evtHeight;

	if(!text.equals(""))
	    {
		evtLenght = TextRenderer.getRenderer().getTextWidth(text, gc);
		evtHeight = TextRenderer.getRenderer().getTextHeight(text, gc);
	    }	
	else
	    {
		evtLenght = MessageEventRenderer.DEFAULT_TEXT_WIDTH-5;
		evtHeight = MessageValue.DEFAULT_HEIGHT;
	    }

	area.setBounds(evtx, evty, evtLenght, evtHeight);
	area.requestFocus();

	area.setText(text);
	area.setVisible(true);

	setCaretPosition(text, x-evtx, y-evty);
    }

    void setCaretPosition(String text, int x, int y)
    {
	int pos = area.viewToModel(new Point(x, y));
	
	if ((pos >= 0) && (pos <= text.length()))
	    area.setCaretPosition(pos);
	else
	    area.setCaretPosition(text.length());
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

    int pressX, pressY;
    protected void processMouseEvent(MouseEvent e)
    {
	if (e.getClickCount() == 1)
	    if(isEditing) setMessage();
	
	super.processMouseEvent(e);

	int id = e.getID();
	if(id==MouseEvent.MOUSE_PRESSED)
	    {
		pressX = e.getX();
		pressY = e.getY();
	    }
	else 
	    if(id==MouseEvent.MOUSE_RELEASED)
		startEditIfNeeded(e);
    }

    TrackEvent editEvt;
    void startEditIfNeeded(MouseEvent e)
    {
	int x = e.getX();
	int y = e.getY();
	if((pressX <= x+2)&&(pressX >= x-2)&&(pressY <= y+2)&&(pressY >= y-2))
	    {
		editEvt = (TrackEvent) gc.getRenderManager().
		    firstObjectContaining(pressX, pressY);
		
		if (editEvt != null) 
		    { 
			if(!((MessageValue)editEvt.getValue()).isOnTheButton(editEvt, pressX, gc))
			    SwingUtilities.invokeLater(new Runnable(){
				public void run()
				    {
					doEdit(editEvt, pressX, pressY);
				    }
			    });
		    }
	    }
    }

    void updateEventProperties(Object whichObject, String propName, Object propValue)
    {
	if(propName.equals("message"))
	{
	    String text = (String)propValue;
	    TrackEvent evt = (TrackEvent)whichObject;
	    int width = TextRenderer.getRenderer().getTextWidth(text, gc);
	    int height = TextRenderer.getRenderer().getTextHeight(text, gc);
	  
	    if(width < MessageEventRenderer.MINIMUM_WIDTH) 
		width  = MessageEventRenderer.MINIMUM_WIDTH;

	    if(height < MessageValue.DEFAULT_HEIGHT) 
		height = MessageValue.DEFAULT_HEIGHT;		
		    
	    int lenght = gc.getAdapter().getInvWidth(width+4+MessageEventRenderer.BUTTON_WIDTH);		
	    evt.setProperty("duration", new Integer(lenght));
	    evt.setProperty("height", new Integer(height));
	}
    }

    JTextArea area;
    boolean isEditing = false;
    Event currentEvt = null;
}


