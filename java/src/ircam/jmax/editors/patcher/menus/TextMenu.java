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

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

/** Implement the patcher editor File Menu */

public class TextMenu extends EditorMenu
{
  JMenuItem biggerItem;
  JMenuItem smallerItem;
  JMenuItem fitItem;
  JMenuItem boldItem, italicItem;
  JRadioButtonMenuItem automaticFitItem; 
  private JMenu itsSizesMenu;
  private ButtonGroup itsSizesMenuGroup;
  JRadioButtonMenuItem fakeSizeButton;

  private JMenu itsFontsMenu;
  private ButtonGroup itsFontMenuGroup;
  JRadioButtonMenuItem fakeFontButton;

    Vector fontNameItems = new Vector();
    Vector fontSizeItems = new Vector();

    ErmesSketchPad sketch;

    class TextMenuListener implements MenuListener
    {
	public void menuSelected(MenuEvent e)
	{
	    updateMenu();
	}
	
	public void menuDeselected(MenuEvent e)
	{
	}

	public void menuCanceled(MenuEvent e)
	{
	}
    }

  public TextMenu(ErmesSketchPad sketch)
  {
    super("Text");
    setHorizontalTextPosition(AbstractButton.LEFT);

    JMenuItem item;
    JRadioButtonMenuItem radioItem;
    this.sketch = sketch;

    itsSizesMenuGroup = new ButtonGroup();
    fakeSizeButton = new JRadioButtonMenuItem( "fake");
    itsSizesMenuGroup.add(fakeSizeButton);
    itsFontMenuGroup = new ButtonGroup();
    fakeFontButton = new JRadioButtonMenuItem( "fake");
    itsFontMenuGroup.add(fakeFontButton);

    //add(new JLabel(" Font Sizes"));
    item = new JMenuItem(" Font Sizes");
    item.setEnabled(false);
    add(item);

    for(int i = 0; i < PatcherFontManager.getInstance().getJMaxFontSizes().length; i++)
    {
	radioItem = new JRadioButtonMenuItem(PatcherFontManager.getInstance().getJMaxFontSizes()[i]);
	add(radioItem);
	radioItem.addActionListener(Actions.jmaxFontSizesAction);
	itsSizesMenuGroup.add(radioItem);
	fontSizeItems.addElement(radioItem);
    }

    /*itsBaseSizesMenu = new JMenu("Change Base Size");
      FillBaseSizesMenu( itsBaseSizesMenu);    
      add(itsBaseSizesMenu);

      addSeparator();*/

    itsSizesMenu = new JMenu("Java Sizes");

    FillSizesMenu( itsSizesMenu);    
    add(itsSizesMenu);

    addSeparator();

    //add(new JLabel(" Font Styles"));
    item = new JMenuItem(" Font Styles");
    item.setEnabled(false);
    add(item);

    boldItem = new JCheckBoxMenuItem("Bold");
    boldItem.addActionListener(Actions.fontStylesAction);
    add(boldItem);
    italicItem = new JCheckBoxMenuItem("Italic");
    italicItem.addActionListener(Actions.fontStylesAction);
    add(italicItem);

    addSeparator();

    //add(new JLabel(" Fonts"));
    item = new JMenuItem(" Fonts");
    item.setEnabled(false);
    add(item);

    for(int i = 0; i < PatcherFontManager.getInstance().getJMaxFontNames().length; i++)
    {
	radioItem = new JRadioButtonMenuItem(PatcherFontManager.getInstance().getJMaxFontNames()[i]);
	add(radioItem);
	radioItem.addActionListener(Actions.jmaxFontAction);
	itsFontMenuGroup.add(radioItem);
	fontNameItems.addElement(radioItem);
    }

    itsFontsMenu = new JMenu("Java Fonts");

    FillFontMenu(itsFontsMenu);
    add(itsFontsMenu);

    addSeparator();

    biggerItem  = add(Actions.fontBiggerAction, "Bigger", Event.CTRL_MASK, KeyEvent.VK_ADD);
    smallerItem = add(Actions.fontSmallerAction, "Smaller", Event.CTRL_MASK, KeyEvent.VK_SUBTRACT);

    addSeparator();

    automaticFitItem = new JRadioButtonMenuItem("Automatic Fit To Text");
      
    add(automaticFitItem);
    automaticFitItem.addActionListener(Actions.setAutomaticFitAction);

    fitItem = add(Actions.fitToTextAction, "Fit To Text", Event.CTRL_MASK, KeyEvent.VK_T);

    addMenuListener(new TextMenuListener());
  }

    /*private void FillBaseSizesMenu( JMenu menu)
      {
      int sizes[] = {8, 10, 12, 14, 18, 24};
      JRadioButtonMenuItem item; 
      itsBaseSizesMenuGroup = new ButtonGroup();

      fakeBaseSizeButton = new JRadioButtonMenuItem( "fake");
      itsBaseSizesMenuGroup.add(fakeBaseSizeButton);

      for (int i = 0; i < sizes.length; i++)
      {
      item = new JRadioButtonMenuItem(Integer.toString(sizes[i]));
      menu.add(item);
      item.addActionListener(Actions.fontBaseSizesAction);
	  itsBaseSizesMenuGroup.add(item);
	  }
	  }*/

  private void FillSizesMenu( JMenu menu)
  {
    int sizes[] = {8, 9, 10, 12, 14, 18, 24, 36, 48};
    JRadioButtonMenuItem item; 
    for (int i = 0; i < sizes.length; i++)
      {
	  item = new JRadioButtonMenuItem(Integer.toString(sizes[i]));
	  menu.add(item);
	  item.addActionListener(Actions.fontSizesAction);
	  itsSizesMenuGroup.add(item);
      }
  }

    private final int MAX_MENU_SIZE = 20;
    private int numFontFloor = 1;
    private void FillFontMenu( JMenu theFontMenu)
    {
	/*****************/
	//jdk117-->jdk1.3//
	//String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
	String[] itsFontList = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();
	/*****************/

	JRadioButtonMenuItem item;
	
	int num = 0;
	JMenu currentMenu = theFontMenu;
	for ( int i = 0; i < itsFontList.length; i++)
	    {
		if(num<MAX_MENU_SIZE)
		    /*item = new JRadioButtonMenuItem(itsFontList[i]);
		      theFontMenu.add(item);
		      itsFontMenuGroup.add(item);
		      item.addActionListener(Actions.fontAction);
		      num++;*/
		    num++;
		else
		    {
			//qui crea un sotto menu
			JMenu subMenu = new JMenu("more...");
			currentMenu.add(subMenu);
			currentMenu = subMenu;
			num=1; numFontFloor++;		      
		    }
		item = new JRadioButtonMenuItem(itsFontList[i]);
		currentMenu.add(item);
		itsFontMenuGroup.add(item);
		item.addActionListener(Actions.fontAction);
	    }
    }

  /** Set the font and size menu settings like this:
    If the selection is empty, set the default font.
    If the selection is a singleton, set the object font.
    Otherwise reset everything.

    For the moment, still compare the font names.
    */

  private void updateMenu()
  {
    JRadioButtonMenuItem item;
    boolean sizeExist;
    String fontName;
    int    fontSize;
    int    fontStyle;

    if (ErmesSelection.patcherSelection.isEmpty())
      {
	biggerItem.setEnabled(false);
	smallerItem.setEnabled(false);
	fitItem.setEnabled(false);

	fontName = sketch.getDefaultFontName();
	fontSize = sketch.getDefaultFontSize();
	fontStyle = sketch.getDefaultFontStyle();
      }
    else if (ErmesSelection.patcherSelection.isSingleton())
      {
	biggerItem.setEnabled(true);
	smallerItem.setEnabled(true);
	fitItem.setEnabled(true);
	
	GraphicObject object = ErmesSelection.patcherSelection.getSingleton();

	fontName = object.getFontName();
	fontSize = object.getFontSize();
	fontStyle = object.getFontStyle();
      }
    else
      {
	biggerItem.setEnabled(true);
	smallerItem.setEnabled(true); 
	fitItem.setEnabled(true); 
	fakeFontButton.setSelected(true);
	fakeSizeButton.setSelected(true);
	return;
      }

    if(PatcherFontManager.getInstance().isDefaultFontName(fontName))
	{
	    for(int i = 0; i<fontNameItems.size(); i++)
	    {
	      item = (JRadioButtonMenuItem)fontNameItems.elementAt(i);
	      if(fontName.equals(PatcherFontManager.getInstance().getFontName(item.getText()))) 
	      {
		  item.setSelected(true);
		  break;
	      }	   
	    } 
	    sizeExist = false;
	    for(int i = 0; i<fontSizeItems.size(); i++)
	    {
		item = (JRadioButtonMenuItem)fontSizeItems.elementAt(i);
		if(fontSize == PatcherFontManager.getInstance().getFontSize(item.getText())) 
		{
		    item.setSelected(true);
		    sizeExist = true;
		    break;
		}	 
	    }
	    if(!sizeExist) fakeSizeButton.setSelected(true); 
	}
    else
    {
	if(numFontFloor>1)
	{
	    JMenu currentMenu = itsFontsMenu;
	    for(int i = 0; i<numFontFloor; i++)
		{
		    for(int j = 0; j < currentMenu.getItemCount()-1; j++)
			{
			    item = (JRadioButtonMenuItem)currentMenu.getItem(j);

			    if (item.getText().equals(fontName))
				{
				    item.setSelected(true);
				    break;
				}
			}
		    if(i!=numFontFloor-1)
			currentMenu = (JMenu) (currentMenu.getItem(currentMenu.getItemCount()-1));
		    else
			{
			    item = (JRadioButtonMenuItem)currentMenu.getItem(currentMenu.getItemCount()-1);
			    
			    if (item.getText().equals(fontName))
				{
				    item.setSelected(true);
				    break;
				}
			}
		}
	}
	else
	    for( int i = 0; i < itsFontsMenu.getItemCount(); i++)
		{
		    item = (JRadioButtonMenuItem)itsFontsMenu.getItem( i);

		    if (item.getText().equals(fontName))
			{
			    item.setSelected(true);
			    break;
			}
		}

	sizeExist = false;
	for ( int i = 0; i < itsSizesMenu.getItemCount(); i++)
	    {
		item = (JRadioButtonMenuItem)itsSizesMenu.getItem( i);

		if (Integer.parseInt(item.getText()) == fontSize)
		    {
			item.setSelected(true);
			sizeExist = true;
			break;
		    }
	    }
    
	if(!sizeExist) fakeSizeButton.setSelected(true);
    }

    switch(fontStyle)
	{
	case Font.PLAIN:
	    boldItem.setSelected(false);
	    italicItem.setSelected(false);
	    break;
	case (Font.BOLD + Font.ITALIC):
	    boldItem.setSelected(true);
	    italicItem.setSelected(true);
	    break;
	case Font.BOLD:
	    boldItem.setSelected(true);
	    italicItem.setSelected(false);
	    break;
	case Font.ITALIC:
	    boldItem.setSelected(false);
	    italicItem.setSelected(true);
	    break;
	default:
	    boldItem.setSelected(false);
	    italicItem.setSelected(false);
	}

    /*for ( int i = 0; i < itsBaseSizesMenu.getItemCount(); i++)
      {
      item = (JRadioButtonMenuItem)itsBaseSizesMenu.getItem( i);
      
      if (Integer.parseInt(item.getText()) == ScaleTransform.getInstance().baseSize)
      {
      item.setSelected(true);
      break;
      }
      }*/

  }
}









