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


package ircam.jmax.toolkit;
import java.util.*;

/**
 * This class centralizes the (graphic) context switching and the 
 * resulting tool activation/desactivation. A generic editor calls the ToolManager
 * to add the set of Tools and ContextSwitchers it needs; a Switcher is an object
 * that implements a GraphicContext switch politic (ex., in response
 * to a WindowActivateEvent, or to a MouseEnter in the editor's Component area). 
 * When such a switch event occurs, the ToolManager takes care of redirecting the activity 
 * of the current Tool on the new GraphicContext.<br>
 * The activity of the ToolManager and of an EditorToolbar are coordinated, in the 
 * sense that the EditorToolbar uses the ToolManager to change tool when the user
 * clicks on the toolbar.
 * Changing tool can also be done programmatically (ex.: by a "one shot tool", that
 * perform an action and then mount the default tool).
 * The ToolManager can't be static, every editor class (not necessarily every instance)
 * have to have its ToolManager.
 * @see ircam.jmax.toolkit.ContextSwitcher
 * @see ircam.jmax.toolkit.Tool
 * */

//-------------------------------------------
// Implementation notes: this class offer a set of functionalities formerly offered
// by the EditorToolbar class, and uses the ContextSwitching logic to break the assumption
// that an editor's GraphicContext is associated to a Window (this a prerequisite
// for the implementation of "tracks" of the Sequence editor. )
// Eventually, also the "ToolSwitching" politic should be isolated into specialized Switchers, 
// making the EditorToolbar just one of such "ToolSwitcher".
//-------------------------------------------


public class ToolManager implements SwitchListener, LockListener{


    /**
     * Constructor without a provider. The tools must be communicated to
     * this Manager via addTool() calls */
    public ToolManager()
    {
    }

    /**
     * Constructor with a provider. It's an alternative to insert all the tools
     * via direct addTool() calls */
  public ToolManager(ToolProvider theProvider)
  {
    Tool aTool;
    for (Enumeration e = theProvider.getTools(); e.hasMoreElements();) 
      {
	aTool = (Tool) e.nextElement();
	if (aTool == null) System.err.println("warning: trying to add a null tool");
	else {
	  addTool(aTool);
	}
      }
  }

    /**
     * Adds a switcher to this manager */
    public void addContextSwitcher(ContextSwitcher switcher)
    {
	switcher.addSwitchListener(this);
    }

    /**
     * Remove a Switcher */
    public void removeContextSwitcher(ContextSwitcher switcher)
    {
	switcher.removeSwitchListener(this);
    }


    /**
     * Add a Tool to the manager */
    public void addTool(Tool tool)
    {
	tools.addElement(tool);
    }


    /**
     * Returns an enumeration of the tools currently handled by this manager */
    public Enumeration getTools()
    {
	return tools.elements();
    }


    /**
     * Returns the tool of the given name, if any */
    // Implementation notes: an Hashtable?
    public Tool getToolByName(String name)
    {
	Tool temp;
	for (Enumeration e = getTools(); e.hasMoreElements();)
	    {
		temp = (Tool) e.nextElement();
		if (temp.getName().equals(name))
		    return temp;
	    }

	return null;
    }

    /**
     * Make the given tool the current one and activates it on the given graphic context */ 
    public void activate(Tool tool, GraphicContext gc)
    {
	currentContext = gc;

	changeTool(tool);
    }

    /**
     * Programmatically make the given tool the current one, on the same
     * graphic context then the old */
    public void changeTool(Tool newTool)
    {
	if (currentTool != null)
	    currentTool.deactivate();
	if (currentContext != null)
	    newTool.reActivate(currentContext);
	
	currentTool = newTool;
	
	if (currentTool != null)
	    {
		toolNotification(newTool);
	    }
    }

    /**
     * utility function. Communicate the tool-change event to the listeners
     */
    private void toolNotification(Tool theTool) 
    {  
	ToolChangeEvent aEvent = new ToolChangeEvent(theTool);
	
	ToolListener aListener;
	for (Enumeration e=listeners.elements(); e.hasMoreElements();) 
	    {
		aListener = (ToolListener) e.nextElement();
		aListener.toolChanged(aEvent);
	    }
    }
    

    /**
     * Called when a switcher triggered a graphic context. This should not
     * be called directly by the user (use ContextSwitchers instead)*/
    public void contextChanged(GraphicContext gc)
    {

	if (InteractionSemaphore.locked()) 
	    {
		InteractionSemaphore.setCaller(this);
		lastContext = gc;
		return;
	    }

	if (gc != null)
	    {

		gc.activate();
		currentContext = gc;
	    }

	
	if (currentTool != null) 
	    {
		
		currentTool.reActivate(gc);
	    }

    }

    /**
     * Called when a GraphicContext is destroyed (ex: the window associated to
     * a WindowContextSwitcher is disposed). This should not be called directly by the user,
     * but indirectly by a ContextSwitcher.*/
    public void contextDestroyed(GraphicContext gc)
    {
	gc.destroy();
    }


    /**
     * Returns the current active tool */
    public Tool getCurrentTool()
    {
	return currentTool;
    }

    /**
     * Returns the current active graphic context */
    public GraphicContext getCurrentGC()
    {
	return currentContext;
    }


    /**
     * Inscribe a new Tool change listener to this Manager
     */
    public void addToolListener(ToolListener tl) 
    {  
	listeners.addElement(tl);
    }
    
    /**
     * remove a tool change listener
     */
    public void removeToolListener(ToolListener tl) 
    {  
	listeners.removeElement(tl);  
    }
  
    public void unlocked()
    {
	if (lastContext != null)
	    contextChanged(lastContext);

	lastContext = null;
    }

    //--- ToolManager fields ---
    Vector tools = new Vector();
    GraphicContext currentContext;
    Tool currentTool;
    Vector listeners = new Vector();
    boolean contextLocked = false;
    GraphicContext lastContext;

}
