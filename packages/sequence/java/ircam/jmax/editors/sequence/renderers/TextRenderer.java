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

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

//
// The edit field contained in the editable objects (Message, Standard).
// Must subclass JTextArea in order to  use it, because the getColumnWidth is
// protected in JTextArea, and we *need* to know.

public class TextRenderer implements ObjectRenderer
{
  class RenderTextArea extends JTextArea
  {
    public boolean canResizeWidthTo(int width)
    {
      return width > getColumnWidth();
    }
    
    public int getColWidth(){
      return super.getColumnWidth();
    }
    public int getRHeight(){
      return super.getRowHeight();
    }
    public String getTextLine(int line){
      String text = "";
      if(line<getLineCount()){
	try{
	  text = getText().substring(getLineStartOffset(line), getLineEndOffset(line)-1);
	}catch(BadLocationException be){
	  text = "";
	}catch(StringIndexOutOfBoundsException se){
	  text = "";
	}
      }
      return text;
    }
  }

    public static TextRenderer staticInstance;
    private RenderTextArea area;
    private Font currentFont;
    //--------------------------------------------------------
    // CONSTRUCTOR
    //--------------------------------------------------------

    public TextRenderer()
    {
	super();

	currentFont = MessageEventRenderer.stringFont;
	area = new RenderTextArea();
	area.setEditable(false);
	area.setLineWrap(true);
	area.setWrapStyleWord(true);
	area.setBackground(Color.white);
	area.setForeground(Color.darkGray);
	area.setFont(currentFont);
    }

    public TextRenderer(Font font, Color back, Color fore)
    {
	super();

	currentFont = font;
	area = new RenderTextArea();
	area.setEditable(false);
	area.setLineWrap(true);
	area.setWrapStyleWord(true);
	area.setBackground(back);
	area.setForeground(fore);
	area.setFont(currentFont);
    }

    public static TextRenderer getRenderer()
    {
	if (staticInstance == null)
	    staticInstance = new TextRenderer();

	return staticInstance;
    }

    public int getTextWidth(String text, SequenceGraphicContext gc){
	int ww;
	int w = 0;

	if(!text.equals(""))
	    {
		area.setText(text);
		
		//FontMetrics fm = gc.getGraphicDestination().getFontMetrics(currentFont);
		FontMetrics fm = area.getFontMetrics(currentFont);
		
		int lines = area.getLineCount();
	    
		if(lines==1)
		    w = SwingUtilities.computeStringWidth(fm, text);
		else{
		    for(int i=0;i<lines;i++){
			ww = SwingUtilities.computeStringWidth(fm, area.getTextLine(i));
			if(ww>w) w = ww;
		    }
		}
	    }
	return w;
    }

  public int getTextHeight(String text, SequenceGraphicContext gc){
    int h = 0;
    if(!text.equals(""))
	{
	    area.setText(text);

	    int lines = area.getLineCount();
	    h = area.getRHeight()*lines;
	}
    return h;
  }

  public boolean canResizeWidthTo(int width)
  {
    return area.canResizeWidthTo(width);
  }

    public boolean isMultiLine(){
	return (area.getLineCount()>1);
    }

    public int getRHeight(){
	return area.getRHeight();
    }

    public int getHeight()
    {
	if(isMultiLine())
	    return area.getPreferredSize().height;
	else
	    return area.getRHeight();
    }

    public int getColWidth(){
	return area.getColWidth();
    }

    public int getWidth()
    {
	return area.getPreferredSize().width;
    }

    public void setBackground(Color color)
    {
	area.setBackground(color);
    }

    public void setForeground(Color color)
    {
	area.setForeground(color);
    }

    public void setFont(Font font)
    {
	currentFont = font;
	area.setFont(currentFont);
    }
    public Font getFont()
    {
	return currentFont;
    }

    static Container ic = new Panel();
    
    public void render(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
    {}

    public void render(Graphics g, String text, int x, int y, int w, int h)
    {
	area.setText(text);
	render(g, x, y, w, h);
    }
    public void render(Graphics g, int x, int y, int w, int h)
    {
	SwingUtilities.paintComponent(g, area, ic, x, y, w, h);
    }
    public boolean touches(Object obj, int x, int y, int w, int h, GraphicContext theGc)
    {return false;}
    public boolean contains(Object obj, int x, int y, GraphicContext theGc) 
    {return false;}
}










