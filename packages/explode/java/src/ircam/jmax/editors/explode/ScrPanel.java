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

package ircam.jmax.editors.explode;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

  /**
   * The piano roll panel of an explode editor. 
   * The graphic representation is handled by 
   * a RendererManager that has the responsability to actually draw the score.
   * The simplest renderer is "ScoreRenderer", a piano-roll component.
   * The user interaction is handled by the tools.
   * The panel builds also the Graphic context to be used during edit, 
   * and the Toolbar. 
   */
public class ScrPanel extends JPanel implements ExplodeDataListener, ToolProvider, ToolListener, ListSelectionListener, StatusBarClient{
  
  /**
   * Constructor based on a ExplodeDataModel and a selection model
   */
  public ScrPanel(ExplodePanel panel, ExplodeDataModel ep, ExplodeSelection s) 
  {  
    explodePanel = panel;
    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());
    setBackground(Color.white);
    setDoubleBuffered(false);

    //-- prepares the NORTH Status bar
    
    JPanel northSection = new JPanel();
    northSection.setLayout(new BoxLayout(northSection, BoxLayout.Y_AXIS));
    
    itsStatusBar = new InfoPanel();
    itsStatusBar.setSize(300, 20);

    northSection.add(itsStatusBar);

    add(northSection, BorderLayout.NORTH);

    //-- prepares the CENTER panel (the score)
    // a simple panel, that just uses a Renderer as its paint method...
    // it takes care of the popup showing.
    // NOTE: the popup cannot be shown from a simple listener, since
    // being a MouseListener for this panel does not prevent the panel
    // itself (and then the current tool) to receive the same events.
    itsScore = new JPanel() {

      public void paint(Graphics g) 
	{
	  Rectangle r = g.getClipBounds();
	  gc.getRenderManager().render(g, r); //et c'est tout	
	}

       protected void processMouseEvent(MouseEvent e)
	{
	  if (e.isPopupTrigger()) 
	    {
	      tb.itsPopupMenu.show (e.getComponent(), e.getX()-10, e.getY()-10);
	    }
	  else 
	    super.processMouseEvent(e);
	  
	}
    };

    itsScore.setBounds(0, itsStatusBar.getSize().height, PANEL_WIDTH, PANEL_HEIGHT);
    itsScore.setDoubleBuffered(false);
    add(itsScore, BorderLayout.CENTER);

    { //-- prepares the graphic context
      gc = new ExplodeGraphicContext(ep, s);
      gc.setGraphicSource(itsScore);
      gc.setGraphicDestination(itsScore);

      gc.setRenderManager(new ScoreRenderer(gc));

      gc.setLogicalTime(0);
      gc.setStatusBar(itsStatusBar);
      
    }

    // prepare the toolbar and insert it in the status bar
    prepareToolbar();

    // make this panel repaint when the selection status change
    // either in content or in ownership.
    s.addListSelectionListener(this);
    s.setOwner(new SelectionOwner() {
      public void selectionDisactivated()
	{
	  itsScore.repaint();
	}
      public void selectionActivated()
	{
	  itsScore.repaint();
	}
    });

    gc.getDataModel().addListener(this);

    itsStatusBar.addWidget(new ScrEventWidget(gc));

    //-- prepares the zoom scrollbar (time stretching) and its listeners
    itsTimeZoom = new Scrollbar(Scrollbar.HORIZONTAL, INITIAL_ZOOM, 5, 1, 1000);

    itsZoomLabel = new JLabel("Zoom: "+INITIAL_ZOOM+"%");
    
    gc.getAdapter().addZoomListener( new ZoomListener() {
      public void zoomChanged(float zoom, float oldZoom)
	{
	  itsZoomLabel.setText("Zoom: "+((int)(zoom*100))+"%"); 
	  repaint();
	}
    });


    itsTimeZoom.addAdjustmentListener(new AdjustmentListener() {
      
      public void adjustmentValueChanged(AdjustmentEvent e) {
	
	gc.getAdapter().setXZoom(e.getValue());
	itsTimeScrollbar.setVisibleAmount(windowTimeWidth()/2);
	itsScore.repaint();
      }
      
    });


    //-- prepares the SOUTH scrollbar (time scrolling) and its listener
    int totalTime = 0;
    
    if (gc.getDataModel().length() != 0)
      {
	ScrEvent e = gc.getDataModel().getEventAt(gc.getDataModel().length()-1);
	totalTime = ((e.getTime()+e.getDuration())*3)/2;
      }

    if (totalTime < MINIMUM_TIME) totalTime = MINIMUM_TIME;

    itsTimeScrollbar = new Scrollbar(Scrollbar.HORIZONTAL, 0, 1000, 0, totalTime);
    itsTimeScrollbar.setUnitIncrement(windowTimeWidth()/10);//WARN: setUnitIncrement seems not to work
    itsTimeScrollbar.setBlockIncrement(windowTimeWidth());


    itsTimeScrollbar.addAdjustmentListener(new AdjustmentListener() {
    
      public void adjustmentValueChanged(AdjustmentEvent e) {
	
	int currentTime = e.getValue();
	gc.setLogicalTime(currentTime);
	
	gc.getAdapter().setXTransposition(-currentTime);
	itsScore.repaint();

	itsStatusBar.post(ScrPanel.this, "starting time: "+currentTime+"msec"+" zoomfactor"+itsTimeZoom.getValue()+"%");
	
      }
    });

    JPanel aSliderPanel = new JPanel();
    aSliderPanel.setLayout(new ProportionalLayout(ProportionalLayout.X_AXIS, (float) 0.75));
    aSliderPanel.add("", itsTimeScrollbar);
    Box aZoomBox = new Box(BoxLayout.X_AXIS);
    aZoomBox.add(itsZoomLabel);
    aZoomBox.add(itsTimeZoom);

    aSliderPanel.add("", aZoomBox);

    add(aSliderPanel, BorderLayout.SOUTH);
    

    initTools();
    
  }
  
  /**
   * note: can't create the toolbar in the constructor of this class,
   * because the Frame is not available yet.
   * The EditorToolbar class uses the Frame information
   * in order to correctly map graphic contexts on windows.
   * This function must be delayed to the moment in which the window
   * will be really visible.
   */
  public EditorToolbar prepareToolbar() 
  {
      initTools();
      /*if (toolManager == null)*/ toolManager = new /*ExplodeToolManager*/ToolManager(this);

      
      tb = new EditorToolbar(toolManager, EditorToolbar.HORIZONTAL);
      tb.setFloatable(false);
      
      gc.setToolManager(toolManager);
      
      // prepare the Panel containing the toolbar when anchored
      JPanel c = new JPanel() {
	  
	  // this callback is called when the user anchors the tooolbar
	  protected void addImpl(Component comp,
				 Object constraints,
				 int index)
	  {
	      if (! toolbarAnchored && GraphicContext.getFrame(tb) != null)
	      {
		  GraphicContext.getFrame(tb).setVisible(false);
		  GraphicContext.getFrame(tb).dispose();
	      }
	      
	      super.addImpl(comp, constraints, index);
	      ScrPanel.toolbarAnchored = true;
	  }
	  
	  // the tolbar is going to be unanchored
	  public void remove(Component c1)
	  {
	      super.remove(c1);
	      ScrPanel.toolbarAnchored = false;
	      repaint();
	  }
	  
      };

      c.setLayout(new BorderLayout());
      c.setOpaque(false);
      
      c.setSize(200, 30);
      
      itsStatusBar.addWidgetAt(c, 2); // the toolbar of explode is added as WIDGET in the statusBar!
      
      if ( toolbarAnchored)
	  {
	      tb.setSize(200, 30);
	      c.add(tb, BorderLayout.CENTER);
	  }

      //itsStatusBar.post(toolManager.getCurrentTool(), "");
      return tb;
  }



    void frameAvailable()
    {
	gc.setFrame(explodePanel.getFrame());
	toolManager.addContextSwitcher(new WindowContextSwitcher(gc.getFrame(), gc));
	toolManager.addToolListener(this);
	toolManager.activate(itsDefaultTool, gc);
    }


  /**
   * prepares the tools that will be used with this editor.
   * Tools are static (ex.: there's just one arrow tool) and are reused.
   */
  static void initTools() 
  {

    if (tools != null) return; //tools have already been created...

    tools = new Vector();
    String path = null;
    String fs = File.separator;
    /*
      WARNING:
      Waiting for a method to get the packagePath from the package name
    */
    /*try
      {
      path  = MaxApplication.getPackageHandler().locatePackage("explode").getPath()+fs+"images"+fs;
      }
      catch(FileNotFoundException e){
      //System.err.println("Can't locate explode images");
      path = MaxApplication.getProperty("explodePackageDir")+fs+"images" +fs;
      }*/
    path = MaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"explode"+fs+"images"+fs;//??????????????????	 
    /*************************************************************/

    itsDefaultTool = new ArrowTool(new ImageIcon(path+"selecter.gif"));
    tools.addElement( itsDefaultTool);
    tools.addElement(new ScrAddingTool(new ImageIcon(path+"adder.gif")));
    tools.addElement(new DeleteTool(new ImageIcon(path+"eraser.gif")));
    tools.addElement(new MoverTool(new ImageIcon(path+"vmover.gif"), 
				   MoverTool.VERTICAL_MOVEMENT));
    tools.addElement(new MoverTool(new ImageIcon(path+"hmover.gif"), 
				   MoverTool.HORIZONTAL_MOVEMENT));
    tools.addElement(new ResizerTool(new ImageIcon(path+"resizer.gif")));
    tools.addElement(new ZoomTool(new ImageIcon(path+"zoomer.gif")));

  }



  /**
   * ToolProvider interface
   */
  public Enumeration getTools() 
  {  
    return tools.elements();
  }

  /**
   * ToolbarProvider interface
   */
  public GraphicContext getGraphicContext() 
  {
    return gc;
  }

  /**
   * ToolbarProvider interface
   */
  public Tool getDefaultTool() 
  {
    return itsDefaultTool;
  }

  /**
   * Callback from the toolbar when a new tool have been
   * selected by the user
   */ 
  public void toolChanged(ToolChangeEvent e) 
  {
    if (e.getTool() != null) 
      {
	itsStatusBar.post(e.getTool(), "");
      }
    
  }

  /**
   * called when the database is changed
   */

  public void objectChanged(Object spec) 
  {
    repaint();
  }

  public void objectAdded(Object spec, int index) 
  {
    repaint();
  }

  public void objectDeleted(Object whichObject, int index) 
  {
    repaint();
  }

  public void objectMoved(Object whichObject, int oldIndex, int newIndex) 
  {
    repaint();
  }

  /**
   * ListSelectionListener interface
   */

  public void valueChanged(ListSelectionEvent e)
  {
    repaint();
  }


  /* avoid to paint the white background twice*/   
  public void update(Graphics g) {}


  /**
   * The paint method.
   * Delegated to the current Renderer
   */
  /*public void paint(Graphics g) 
  {
    super.paint(g);
  }*/

  /**
   * get the lenght (in milliseconds) of the window
   */
  int windowTimeWidth() 
  {
    return gc.getAdapter().getInvX(itsScore.getSize().width) - gc.getAdapter().getInvX(ScoreBackground.KEYEND) - 1;

  }


  /**
   * from the StatusBarClient interface
   */
  public String getName() 
  {
    return "";
  }


  /**
   * from the StatusBarClient interface
   */
  public ImageIcon getIcon() 
  {
    return null;
  }


  /**
   * invoke the Adapter's editor
   */
  public void settings() {
    gc.getAdapter().edit(gc.getFrame());
  }


  /**
   * from Panel class...
   */
  public Dimension getMinimumSize() 
  {  
    return size;
  }    
  
  /**
   * from Panel class...
   */
  public Dimension getPreferredSize() 
  {  
    return getMinimumSize();	  
  }
  

  //------------- Fields
  public static final int PANEL_WIDTH = 800;
  public static final int PANEL_HEIGHT = 450;
  
  ExplodeGraphicContext gc;

  // the tools are static: this means that there will be, say, just one arrow
  // tool instance in the system
  EditorToolbar itsToolbar;

  static Vector tools;
  static Tool itsDefaultTool;

  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
  
  public final int INITIAL_ZOOM = 20;
  public static final int MINIMUM_TIME = 10000;
  Scrollbar itsTimeScrollbar;
  Scrollbar itsTimeZoom;
  JLabel itsZoomLabel;

  EditorToolbar tb;
  /*static*/ ToolManager toolManager;
  static boolean toolbarAnchored = true;
  
  InfoPanel itsStatusBar;
  JPanel itsScore;
  ExplodePanel explodePanel;
}






