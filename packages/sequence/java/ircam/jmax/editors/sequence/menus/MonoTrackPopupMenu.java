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

package ircam.jmax.editors.sequence.menus;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.track.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class MonoTrackPopupMenu extends JPopupMenu 
{
  static private MonoTrackPopupMenu popup = new MonoTrackPopupMenu();

  int x;
  int y;
  MonoTrackEditor target = null;    
  private boolean added = false;
  JMenuItem displayLabelItem, removeItem, nameItem;
  JMenu moveMenu;
  int trackCount = 0;

  public MonoTrackPopupMenu()
  {
    super();
    JMenuItem item;

    addSeparator();
    moveMenu = new JMenu("Move to Position");
    item = new JMenuItem(""+trackCount);
    item.addActionListener(Actions.moveMonodimensionalTrackToAction);
    moveMenu.add(item);
    
    add(moveMenu);

    addSeparator();
    ////////////////////// Range Menu //////////////////////////////
    item = new JMenuItem("Change Range");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ChangeRangeDialog.changeRange(MonoTrackPopupMenu.getPopupTarget().getTrack(), 
					MonoTrackPopupMenu.getPopupTarget().getGraphicContext().getFrame(),
					SwingUtilities.convertPoint(MonoTrackPopupMenu.getPopupTarget(), 
								    MonoTrackPopupMenu.getPopupX(),
								    MonoTrackPopupMenu.getPopupY(),
								    MonoTrackPopupMenu.getPopupTarget().getGraphicContext().getFrame()));
	}
      });
    add(item);
    
    nameItem = new JMenuItem("Track Name");
    nameItem.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ChangeTrackNameDialog.changeName(MonoTrackPopupMenu.getPopupTarget().getTrack(),  
					   MonoTrackPopupMenu.getPopupTarget().getGraphicContext().getFrame(),
					   SwingUtilities.convertPoint(MonoTrackPopupMenu.getPopupTarget(), 
								       MonoTrackPopupMenu.getPopupX(),
								       MonoTrackPopupMenu.getPopupY(),
								       MonoTrackPopupMenu.getPopupTarget().getGraphicContext().getFrame()));
	}
      });
    
    add( nameItem);

    addSeparator();
    
    ////////////////////// View Menu //////////////////////////////
    JMenu viewMenu = new JMenu("Change View");
    item = new JMenuItem("Peaks view");
    item.addActionListener(new SetViewAction(MonoTrackEditor.PEAKS_VIEW));
    viewMenu.add(item);    
    item = new JMenuItem("Steps view");
    item.addActionListener(new SetViewAction(MonoTrackEditor.STEPS_VIEW));
    viewMenu.add(item);
    item = new JMenuItem("BreakPoints view");
    item.addActionListener(new SetViewAction(MonoTrackEditor.BREAK_POINTS_VIEW));
    viewMenu.add(item);

    add(viewMenu);

    item = new JMenuItem("View as list");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  MonoTrackPopupMenu.getPopupTarget().showListDialog();
	}
    });
    add(item);

    addSeparator();
    ////////////////////// others  //////////////////////////////
    item = new JMenuItem("Select All");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	    MonoTrackPopupMenu.getPopupTarget().getSelection().selectAll();
	    MonoTrackPopupMenu.getPopupTarget().getGraphicContext().getGraphicDestination().requestFocus();
	}
    });
    add(item);

    addSeparator();
     ////////////////////////////////////////////////////////////
    removeItem = new JMenuItem("Remove Track");
    removeItem.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  ((FtsSequenceObject)MonoTrackPopupMenu.getPopupTarget().getGraphicContext().getFtsObject()).
	    removeTrack( MonoTrackPopupMenu.getPopupTarget().getTrack());
	}
    });
    add(removeItem);

    addSeparator();
    item = new JMenuItem("Export Track");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	    MonoTrackPopupMenu.getPopupTarget().getTrack().getFtsTrack().export();
	}
    });
    add(item);

    pack();
  }

  static public MonoTrackPopupMenu getInstance()
  {
    return popup;
  }

  static public MonoTrackEditor getPopupTarget(){
    return popup.target;
  }

  static public void update(MonoTrackEditor editor)
  {
    if( !popup.added) 
      {
	popup.insert( editor.getToolsMenu(), 0);
        popup.target = editor;
	popup.added=true;
	popup.pack();
      }
    else 
      {
	popup.remove( popup.target.getToolsMenu());
	MidiTrackPopupMenu.getInstance().remove( popup.target.getToolsMenu());
	  
	popup.insert(editor.getToolsMenu(), 0);
	popup.target = editor;
	popup.pack();
      }
    
    if( popup.target.getGraphicContext().getFtsObject() instanceof FtsSequenceObject)
      {
	popup.moveMenu.setEnabled( true);
	popup.removeItem.setEnabled( true);
	popup.nameItem.setEnabled( true);
	popup.updateMoveToMenu();	
      }    
    else
      {
	popup.moveMenu.setEnabled( false);
	popup.removeItem.setEnabled( false);
	popup.nameItem.setEnabled( false);
      }
  }

  void updateMoveToMenu()
  {
    JMenuItem item;
    int count =  target.trackCount()-1;
    if(trackCount==count)
      return;
    else
      {
	int dif = count-trackCount;
	
	if(dif>0)
	  for(int i=1; i<=dif; i++)
	    {
	      item = new JMenuItem(""+(trackCount+i));
	      item.addActionListener(Actions.moveMonodimensionalTrackToAction);
	      moveMenu.add(item);			
	    }		
	else
	  for(int i=0; i<-dif; i++)
	    moveMenu.remove(moveMenu.getItemCount()-1);
	trackCount = count;
      }
  }

    class SetViewAction extends AbstractAction {
      SetViewAction(int viewType)
      {
	super("Set View");
	this.viewType = viewType;
      }
    
      public void actionPerformed(ActionEvent e)
      {
	MonoTrackPopupMenu.getPopupTarget().setViewMode(viewType);
      }
	
      int viewType;    
    }
    
    public void show(Component invoker, int x, int y)
    {
	this.x = x;
	this.y = y;
      
	super.show(invoker, x, y);
    }
    static public int getPopupX()
    {
	return popup.x;
    }
    static public int getPopupY()
    {
	return popup.y;
    }
}











