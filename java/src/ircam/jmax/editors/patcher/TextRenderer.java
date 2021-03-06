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

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;

import ircam.jmax.editors.patcher.objects.*;

// import javax.swing.*;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SwingUtilities;

// import javax.swing.text.*;
import javax.swing.text.BadLocationException;

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

  private Editable owner;
  private RenderTextArea area;
  private int defaultWidth;
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public TextRenderer(Editable object)
  {
    super();

    owner = object;
    area = new RenderTextArea();
    area.setEditable(false);
    area.setLineWrap(true);
    area.setWrapStyleWord(true);
    area.setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    area.setBounds(0, 0, 100, 30);
    //defaultWidth = SwingUtilities.computeStringWidth(owner.getFontMetrics(), "pack 1 2 3");
  }

  public void update()
  {
    if (owner.getHeight() > 0){
      area.setBounds(owner.getX() + owner.getTextXOffset(),
		     owner.getY() + owner.getTextYOffset(),
		     owner.getWidth() - owner.getVariableWidth() - owner.getTextWidthOffset(),
		     owner.getHeight() - owner.getTextHeightOffset());
    }    
    else
      area.setBounds(owner.getX() + owner.getTextXOffset(),
		     owner.getY() + owner.getTextYOffset(), 
		     owner.getWidth() - owner.getVariableWidth() - owner.getTextWidthOffset(),
		     5);
    area.setFont(owner.getFont());    
    area.setText(owner.getArgs()); 
  }

  public int getTextWidth(){
    int ww;
    int w = 0;
    int lines = area.getLineCount();

    if(lines==0)
      w = owner.getWidth() - owner.getVariableWidth() - owner.getTextWidthOffset();
    else 
      if(lines==1){
	w = SwingUtilities.computeStringWidth(owner.getFontMetrics(), owner.getArgs());
	//if(w<defaultWidth) w = defaultWidth;
      }
      else{
	for(int i=0;i<lines;i++){
	  ww = SwingUtilities.computeStringWidth(owner.getFontMetrics(), area.getTextLine(i));
	  if(ww>w) w = ww;
	}
	if(w==0) w = owner.getWidth()  - owner.getVariableWidth() - owner.getTextWidthOffset();
      }
    return w;
  }
    
  public int getTextHeight(){
    int h = 0;
    int lines = area.getLineCount();
    if(lines==0)
      h = owner.getHeight() - owner.getTextHeightOffset();
    else 
      h = area.getRHeight()*lines;
    return h;
  }

  public boolean canResizeWidthTo(int width)
  {
    return area.canResizeWidthTo(width);
  }

  public boolean isMultiLine(){
    return (area.getLineCount()>1);
  }

  public boolean isTextLonger(){
    if( owner.getArgs() != null)
      return (SwingUtilities.computeStringWidth(owner.getFontMetrics(), owner.getArgs()) >= 
	      owner.getWidth()  - owner.getVariableWidth() - owner.getTextWidthOffset());
    else
      return true;
  }

  public int getRHeight(){
    return area.getRHeight();
  }

  public int getHeight()
  {
    if(isTextLonger()||isMultiLine())
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

  static Container ic = new JPanel();

  public void render(Graphics g, int x, int y, int w, int h)
  { 
    area.setSize( w, h);
    try{
        SwingUtilities.paintComponent(g, area, ic, x, y, w, h);
    }
    catch( java.lang.NullPointerException e){
        e.printStackTrace();
    }
  }
}


