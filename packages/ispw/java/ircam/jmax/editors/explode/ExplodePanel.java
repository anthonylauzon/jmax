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

package ircam.jmax.editors.explode;

import javax.swing.*;
import javax.swing.undo.*;

import java.awt.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

  /**
   */
public class ExplodePanel extends JPanel implements Editor{
  
  /**
   * Constructor based on a ExplodeDataModel and a selection model
   */
  ExplodeTablePanel itsTablePanel;
  ScrPanel itsScrPanel;
  FtsExplodeObject explodeObject;
  ExplodeDataModel explodeData;
  public int defaultView = PIANOROLL_VIEW;
  public int currentView = PIANOROLL_VIEW;
  public static final int PIANOROLL_VIEW = 0;
  public static final int TABLE_VIEW = 1;
  private JPanel itsView;
  EditorContainer itsContainer;

  public ExplodePanel(EditorContainer container, ExplodeDataModel data) 
  {  
    itsContainer = container;
    explodeData = data;
    explodeObject = (FtsExplodeObject)data;
    ExplodeTableModel model = new ExplodeTableModel(explodeObject);

    ExplodeSelection s = new ExplodeSelection(explodeObject);
    itsTablePanel = new ExplodeTablePanel(model, s);
    // creates the Piano roll view
    itsScrPanel = new ScrPanel(this, explodeObject, s);
     // make the newly created selection the current one
    ExplodeSelection.setCurrent(s);

    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    setView(defaultView);
  }

  public void setView(){
    if(currentView==PIANOROLL_VIEW)
      setView(TABLE_VIEW);
    else
      setView(PIANOROLL_VIEW);
  }

  public int getCurrentView(){
    return currentView;
  }
  /** 
   * Set the kind of panel corresponding to the given view (PIANOROLL, TABLE).
   * Removes also the old panel from the container, if needed */
  private void setView(int view)
  {

    if (itsView != null)
      {
	itsContainer.getFrame().setVisible(false);
	remove(itsView);
      }

    if (view == PIANOROLL_VIEW)
      itsView = itsScrPanel;
    else 
      itsView = itsTablePanel;

    add(itsView);
    currentView = view;

    if (view == PIANOROLL_VIEW)
      {
	itsScrPanel.frameAvailable();
      }
    
    itsContainer.getFrame().validate();
    itsContainer.getFrame().pack();

    itsContainer.getFrame().setVisible(true);
  }

  ////////////////////////////////////////////////////////////
  public void Copy()
  {
    ((ClipableData) explodeData).copy();
  }

  public void Cut()
  {
    ((ClipableData) explodeData).cut();
  }

  public void Paste()
  {
    ((ClipableData) explodeData).paste();
  }

  public void Duplicate()
  {
    Copy();
    Paste();
  }

  public void Undo()
  {
    try 
      {
	((UndoableData) explodeData).undo();
      } catch (CannotUndoException e1) {
	System.out.println("can't undo");
      }
  }

  public void Redo()
  {
    try 
      {
	((UndoableData) explodeData).redo();
      } catch (CannotRedoException e1) {
	System.out.println("can't redo");
      }
  }

  public void Settings(){
    itsScrPanel.settings();
  }

  //per ora
  public Frame getFrame(){
    return itsContainer.getFrame();
  }
  //------------------- Editor interface ---------------
  public EditorContainer getEditorContainer(){
    return itsContainer;
  }

  public void close(boolean doCancel){
    itsContainer.getFrame().setVisible(false);
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsContainer);
  }
  public void save(){}
  public void saveAs(){}
  public void print(){}
}





