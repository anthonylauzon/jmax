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

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

// import javax.swing.*;
import javax.swing.ButtonGroup;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JRadioButtonMenuItem;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

/** Implement the patcher editor File Menu */

public class TextPopUpMenu extends JMenu
{
  static private TextPopUpMenu textPopup = new TextPopUpMenu();

  private JMenu itsSizesMenu;
  private ButtonGroup itsSizesMenuGroup;
  JRadioButtonMenuItem fakeSizeButton;

  private JMenu itsFontsMenu;
  private ButtonGroup itsFontMenuGroup;
  
  private JMenu itsStylesMenu;
  private ButtonGroup itsStylesMenuGroup;
  JMenuItem boldItem, italicItem;

  Vector fontNameItems = new Vector();
  Vector fontSizeItems = new Vector();
  
  public TextPopUpMenu()
  {
    super("Text");
    JMenuItem item;
    JRadioButtonMenuItem radioItem;

    itsSizesMenuGroup = new ButtonGroup();
    fakeSizeButton = new JRadioButtonMenuItem( "fake");
    itsSizesMenuGroup.add(fakeSizeButton);

    itsFontMenuGroup = new ButtonGroup();
    itsStylesMenuGroup = new ButtonGroup();

    add(new JLabel(" Font Sizes"));

    for(int i = 0; i < PatcherFontManager.getInstance().getJMaxFontSizes().length; i++)
    {
	radioItem = new JRadioButtonMenuItem(PatcherFontManager.getInstance().getJMaxFontSizes()[i]);
	add(radioItem);
	radioItem.addActionListener(Actions.jmaxFontSizesPopUpAction);
	itsSizesMenuGroup.add(radioItem);
	fontSizeItems.addElement(radioItem);
    }

    itsSizesMenu = new JMenu("Java Sizes");
    FillSizesMenu( itsSizesMenu);
    add(itsSizesMenu);
    
    addSeparator();

    add(new JLabel(" Font Styles"));

    boldItem = new JCheckBoxMenuItem("Bold");
    boldItem.addActionListener(Actions.fontStylesPopUpAction);
    add(boldItem);
    italicItem = new JCheckBoxMenuItem("Italic");
    italicItem.addActionListener(Actions.fontStylesPopUpAction);
    add(italicItem);

    addSeparator();

    add(new JLabel(" Font Names"));

    for(int i = 0; i < PatcherFontManager.getInstance().getJMaxFontNames().length; i++)
    {
	radioItem = new JRadioButtonMenuItem(PatcherFontManager.getInstance().getJMaxFontNames()[i]);
	add(radioItem);
	radioItem.addActionListener(Actions.jmaxFontPopUpAction);
	itsFontMenuGroup.add(radioItem);
	fontNameItems.addElement(radioItem);
    }

    itsFontsMenu = new JMenu("Java Fonts");
    FillFontMenu(itsFontsMenu);
    add(itsFontsMenu);

    addSeparator();

    item = new JMenuItem("Fit To Text");
    item.addActionListener(Actions.fitToTextPopUpAction);
    add(item);
    
    addSeparator();

    item = new JMenuItem("Smaller");
    item.addActionListener(Actions.fontSmallerPopUpAction);
    add(item);

    item = new JMenuItem("Bigger");
    item.addActionListener(Actions.fontBiggerPopUpAction);
    add(item);
  }

  private void FillSizesMenu( JMenu menu)
  {
    int sizes[] = {8, 9, 10, 12, 14, 18, 24, 36, 48};
    JRadioButtonMenuItem item; 

    for (int i = 0; i < sizes.length; i++)
      {
	item = new JRadioButtonMenuItem(Integer.toString(sizes[i]));
	menu.add(item);
	item.addActionListener(Actions.fontSizesPopUpAction);
	itsSizesMenuGroup.add(item);
      }
  }

    private final int MAX_MENU_SIZE = 20;
    private int numFontFloor = 1;
    private void FillFontMenu( JMenu theFontMenu)
    {
	String[] itsFontList = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();

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
			JMenu subMenu = new JMenu("more...");
			currentMenu.add(subMenu);
			currentMenu = subMenu;
			num=1; numFontFloor++;		      
		    }
		item = new JRadioButtonMenuItem(itsFontList[i]);
		currentMenu.add(item);
		itsFontMenuGroup.add(item);
		item.addActionListener(Actions.fontPopUpAction);
	    }
    }

  public static TextPopUpMenu getInstance()
  {
    return textPopup;
  }

  public static void update(GraphicObject obj)
  {
    JRadioButtonMenuItem item;
    String fontName = obj.getFontName();
    int fontSize = obj.getFontSize();
    int fontStyle = obj.getFontStyle();

    if(PatcherFontManager.getInstance().isDefaultFontName(fontName))
    {
	for(int i = 0; i<textPopup.fontNameItems.size(); i++)
	{
	    item = (JRadioButtonMenuItem)textPopup.fontNameItems.elementAt(i);
	    if(fontName.equals(PatcherFontManager.getInstance().getFontName(item.getText()))) 
	    {
		item.setSelected(true);
		break;
	    }	   
	}
	boolean sizeExist = false;
	for(int i = 0; i<textPopup.fontSizeItems.size(); i++)
	{
	    item = (JRadioButtonMenuItem)textPopup.fontSizeItems.elementAt(i);
	    if(fontSize == PatcherFontManager.getInstance().getFontSize(item.getText())) 
	    {
		item.setSelected(true);
		sizeExist = true;
		break;
	    }	 
	}
	if(!sizeExist) textPopup.fakeSizeButton.setSelected(true); 
    }
    else
    {
	if(textPopup.numFontFloor>1)
	{
	    JMenu currentMenu = textPopup.itsFontsMenu;
	    for(int i = 0; i<textPopup.numFontFloor; i++)
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
		    if(i!=textPopup.numFontFloor-1)
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
	    for( int i = 0; i < textPopup.itsFontsMenu.getItemCount(); i++)
		{
		    item = (JRadioButtonMenuItem)textPopup.itsFontsMenu.getItem( i);

		    if (item.getText().equals(fontName))
			{
			    item.setSelected(true);
			    break;
			}
		}
    
	for ( int i = 0; i < textPopup.itsSizesMenu.getItemCount(); i++)
	{
	    item = (JRadioButtonMenuItem)textPopup.itsSizesMenu.getItem( i);
	    
	    if (Integer.parseInt(item.getText()) == fontSize)
		{
		    item.setSelected(true);
		    break;
		}
	}
    }

    switch(fontStyle)
	{
	case Font.PLAIN:
	    textPopup.boldItem.setSelected(false);
	    textPopup.italicItem.setSelected(false);
	    break;
	case (Font.BOLD + Font.ITALIC):
	    textPopup.boldItem.setSelected(true);
	    textPopup.italicItem.setSelected(true);
	    break;
	case Font.BOLD:
	    textPopup.boldItem.setSelected(true);
	    textPopup.italicItem.setSelected(false);
	    break;
	case Font.ITALIC:
	    textPopup.boldItem.setSelected(false);
	    textPopup.italicItem.setSelected(true);
	    break;
	default:
	    textPopup.boldItem.setSelected(false);
	    textPopup.italicItem.setSelected(false);
	}
  }
}








