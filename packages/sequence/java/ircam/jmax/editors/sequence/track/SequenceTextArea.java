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

package ircam.jmax.editors.sequence.track;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

import ircam.jmax.fts.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;
import javax.swing.border.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;

//
// The edit field contained in the editable objects (Message, Editable).
//

public class SequenceTextArea extends JTextArea implements FocusListener
{

  SequenceTextArea(TextRenderer renderer, SequenceGraphicContext gc) 
  {
    super();
    
    textRenderer = renderer;
    this.gc = gc;
    setCursor( Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR));
    setEditable(true);
    setLineWrap(true);
    setWrapStyleWord(true);
    setBackground(Color.white);
    setForeground(Color.darkGray);
    setFont(renderer.getFont());
    addKeyListener(new KeyListener(){
	public void keyPressed(KeyEvent e){
	    if(isEditKey(e))
		resizeIfNeeded(e, getCaretPosition());
	}
	public void keyReleased(KeyEvent e){}
	public void keyTyped(KeyEvent e){}
    });

    addFocusListener(this);
  }

  public void doEdit(String text, int x, int y, int width, int height)
  {
      setBounds(x, y, width, height);
      requestFocus();      
      setText(text);
      setVisible(true);
  }

  public static boolean isEditKey(KeyEvent e)
  {
      return ((!e.isActionKey())&&
	      (e.getKeyCode()!=KeyEvent.VK_SHIFT)&&(e.getKeyCode()!=KeyEvent.VK_CONTROL));
  }

  public static boolean isDeleteKey(KeyEvent e)
  {
      return ((e.getKeyCode() == KeyEvent.VK_DELETE)||(e.getKeyCode() == KeyEvent.VK_BACK_SPACE));
  }
  public static boolean isReturnKey(KeyEvent e)
  {
      return (e.getKeyCode() == KeyEvent.VK_ENTER);
  }

  void resizeIfNeeded(KeyEvent e, int pos)
  {
      String text = getText();
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
      int width  = textRenderer.getTextWidth(text, gc) + borderHGap;
      int height = textRenderer.getTextHeight(text, gc) + borderVGap;
      Dimension d = getSize();
      Dimension maxDim = getContainer().getSize();

      if((width<maxDim.width-MessageEventRenderer.BUTTON_WIDTH-ScoreBackground.KEYEND-TrackContainer.BUTTON_WIDTH)
	 &&(height<maxDim.height))
	  {
	      if(width > d.width)
		  {
		      if(width < minWidth) 
			  width  = minWidth;
		      d.width = width;
		      
		      setSize(d);
		      
		      notifyWidthChange(width, height);
		}
	      if((height > d.height)||(height<d.height-10))
		  {
		      if(height < minHeight) 
			  height = minHeight;		
		      d.height = height;
		      
		      setSize(d);
		      
		      notifyHeightChange(width, height);
		  }
	      
	  }
      else
	  notifyEndEdit();
  }

  public void setMinimumSize(Dimension d)
  {
      minWidth = d.width;
      minHeight = d.height;
  }
  public void setBorder(Border border)
  {
      super.setBorder(border);
      Insets ins = border.getBorderInsets(this);
      borderHGap = ins.right+ins.left;
      borderVGap = ins.top+ins.bottom;
  }

  public void setContainer(Component cont)
  {
      container = cont;
  }
  public Component getContainer()
  {
      if(container!=null) return container;
      else return gc.getGraphicDestination();
  }

  public void addSequenceTextAreaListener(SequenceTextAreaListener listener)
  {
      listeners.addElement(listener);
  }
  public void removeSequenceTextAreaListener(SequenceTextAreaListener listener)
  {
      listeners.removeElement(listener);
  }
    void notifyWidthChange(int width, int height)
    {
	for(Enumeration e = listeners.elements(); e.hasMoreElements();)
	    ((SequenceTextAreaListener) e.nextElement()).widthChanged(width);
    }
    void notifyHeightChange(int width, int height)
    {
	for(Enumeration e = listeners.elements(); e.hasMoreElements();)
	    ((SequenceTextAreaListener) e.nextElement()).heightChanged(height);
    }
    void notifyEndEdit()
    {
	for(Enumeration e = listeners.elements(); e.hasMoreElements();)
	    ((SequenceTextAreaListener) e.nextElement()).endEdit();
    }


  //FocusListener interface
  public void focusGained(FocusEvent e){}
  public void focusLost(FocusEvent e){}

  TextRenderer textRenderer;
  SequenceGraphicContext gc;
  Component container = null;

  private Vector listeners = new Vector();  
  private int minWidth = 0;
  private int minHeight = 0;
  private int borderHGap = 0;
  private int borderVGap = 0;
}











