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

package ircam.jmax.toolkit;

import java.awt.*;
import javax.swing.event.*;

/**
 * The context for a generic editing session.
 * it stores the source of graphic events, the graphic destination for 
 * the drawing operations, the RendererManager, the Frame.
 */
public class GraphicContext {

  public GraphicContext()
  {
  }

  /**
   * sets the source of events, that is: the components that generates
   * the awt events.
   */
  public void setGraphicSource(Component theGraphicSource) 
  {
    itsGraphicSource = theGraphicSource;
  }


  /**
   * gets the source
   */
  public Component getGraphicSource() 
  {
    return itsGraphicSource;
  }


  /**
   * set the destination, that is the component that will receive the
   * graphic (drawing) operations
   */
  public void setGraphicDestination(Component theGraphicDestination) 
  {
    itsGraphicDestination = theGraphicDestination;
  }


  /**
   * get the destination
   */
  public Component getGraphicDestination() 
  {
    return itsGraphicDestination;
  }


  /**
   * sets the RenderManager to be used in this context
   */
  public void setRenderManager(RenderManager theManager) 
  {
    itsRenderManager = theManager;
  }
  
  /**
   * gets the RenderManager
   */
  public RenderManager getRenderManager() 
  {
    return itsRenderManager;
  }

  /**
   * set the status bar (should this become standard?)
   */
  public void setStatusBar(StatusBar theStatusBar) 
  {
    itsStatusBar = theStatusBar;
  }

  /**
   * get the status bar
   */
  public StatusBar getStatusBar() 
  {
    return itsStatusBar;
  }
  
  /** sets the awt Frame this context refers to */
  public void setFrame(Frame f) 
  {
    itsFrame = f;
  }

  /** gets the awt Frame this context refers to */
  public Frame getFrame() 
  {
    if (itsFrame != null) 
      return itsFrame;
    else return getFrame(itsGraphicDestination);
  }

  /**
   * Called when this context become the current one.
   * Override to do special actions like change Selection ownerships
   * and so on. */
  public void activate()
  {
  }

    /**
     * Called when the destination for the graphic operation is going to be
     * closed */
    public void destroy()
    {
    }

  /**
   * utility static function:
   * returns the Frame containing a component (why this function is not in AWT?)
   */
  public static Frame getFrame(Component c) {
    for (Container p = c.getParent(); p != null; p = p.getParent()) {
      if (p instanceof Frame) {
	return (Frame) p;
      }
    }
    return null;
  }

  //-------------------------------------

  protected Component itsGraphicSource;
  protected Component itsGraphicDestination;

  StatusBar itsStatusBar;

  protected RenderManager itsRenderManager;
  protected Frame itsFrame;
}












