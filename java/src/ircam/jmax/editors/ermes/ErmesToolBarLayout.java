/*
 * Copyright (c) 1995-1997 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */
/*
 * 1.0 code.
 */

package ircam.jmax.editors.ermes;
import java.awt.*;
import java.util.*;

/**
 * The sticky "toolbar" layout manager. It allows the adding of several
 * components in a container, in a "top down" manner, without changing
 * their size in response to a layout() system call (the typical
 * toolbar behaviour). An analogous horizantal behaviour could be choosen
 * during the creation.
 */
public class ErmesToolBarLayout implements LayoutManager {

  
  private int vgap;
  private int minWidth = 0, minHeight = 0;
  private int preferredWidth = 0, preferredHeight = 0;
  private boolean sizeUnknown = true;
  public static final int VERTICAL = 0;
  public static final int HORIZONTAL = 1;
  int itsType;
  Component itsFill = null;
  Hashtable itsSpecials = new Hashtable();
  int itsInset = 0;

  /**
   * Simplest constructor: just specify the VERTICAL or HORIZONTAL direction
   */
    public ErmesToolBarLayout(int theType) {
      itsType = theType;
    }

  /**
   * Contructor that allows to specify an inset
   */
    public ErmesToolBarLayout(int theType, int theInset) {
      itsType = theType;
      itsInset =theInset;
    }

    /* Required by LayoutManager. */
    public void addLayoutComponent(String name, Component comp) {
      //the name last indicates the "fillObject"
      if (name != null && name.equals("fill")) {
	itsFill = comp;
      }
      itsSpecials.put(comp, name);
      //      System.err.println("********** "+comp.getClass().getName());
    }

    /* Required by LayoutManager. */
    public void removeLayoutComponent(Component comp) {
    }

    /* Required by LayoutManager. */
    public Dimension preferredLayoutSize(Container parent) {
        Dimension dim = new Dimension(0, 0);
        int nComps = parent.countComponents();

        //Always add the container's insets!
        Insets insets = parent.insets();
	//preferredWidth should be the largest component, 
	//preferredHeight should be the sum of the heights
	//-----note: if Horizontal, should be the inverse!
	int preferredWidth = (itsType == VERTICAL)?maxWidth(parent):sumWidths(parent);
	int preferredHeight = (itsType == VERTICAL)?sumHeights(parent):maxHeight(parent);
        dim.width = preferredWidth + insets.left + insets.right;
        dim.height = preferredHeight + insets.top + insets.bottom;

        sizeUnknown = false;

        return dim;
    }
  
  private int maxWidth(Container parent){
    int max = 0;

    Component aComponent;
    Dimension aDimension;

    int nComps = parent.countComponents();
    for (int i=0;i<nComps;i++) {
      aComponent = parent.getComponent(i);
      aDimension = aComponent.getSize();
      if (aDimension.width > max) max = aDimension.width;
    }
    return max+2*itsInset;
  }

  private int maxHeight(Container parent){
    int max = 0;

    Component aComponent;
    Dimension aDimension;

    int nComps = parent.countComponents();
    for (int i=0;i<nComps;i++) {
      aComponent = parent.getComponent(i);
      aDimension = aComponent.getSize();
      if (aDimension.height > max) max = aDimension.height;
    }
    return max+2*itsInset;
  }
  
  //that god send it good (che dio ce la mandi buona)
  private int sumHeights(Container parent){
    int max = 0;
    Component aComponent;
    Dimension aDimension;

    int nComps = parent.countComponents();
    for (int i=0;i<nComps;i++) {
      aComponent = parent.getComponent(i);
      aDimension = aComponent.getSize();
      max += aDimension.height+itsInset;
    }
    return max;
  }

  //that god send it good (che dio ce la mandi buona)
  private int sumWidths(Container parent){
    int max = 0;
    Component aComponent;
    Dimension aDimension;

    int nComps = parent.countComponents();
    for (int i=0;i<nComps;i++) {
      aComponent = parent.getComponent(i);
      aDimension = aComponent.getSize();
      max += aDimension.width+itsInset;
    }
    return max;
  }

    /* Required by LayoutManager. */
    public Dimension minimumLayoutSize(Container parent) {
        Dimension dim = new Dimension(0, 0);
	int minWidth = 0;
	int minHeight = 0;

        //Always add the container's insets!
        Insets insets = parent.insets();
	
	Component aComponent;
	Dimension aDimension;
	
	int nComps = parent.countComponents();
	for (int i=0;i<nComps;i++) {
	  aComponent = parent.getComponent(i);	  
	  aDimension = aComponent.getSize();
	  minWidth = Math.max(aComponent.minimumSize().width, minWidth); 
	  minHeight = Math.max(aComponent.minimumSize().height, minHeight);
	}
        dim.width = minWidth + insets.left + insets.right;
        dim.height = minHeight + insets.top + insets.bottom;

        sizeUnknown = false;
        return dim;
    }

  /**
   * The simplest possible politic: set the position of the components
   * bottom up, sticked to left, one after the other, and as sizes 
   * their respective preferred size, or:
   * left to right, sticked to left, one after the other, and as sizes 
   * their respective preferred sizes (don't change)
   */
  public void layoutContainer(Container parent) {
    Component aComponent = null;
    int currentX = parent.insets().left;
    int currentY = parent.insets().top;
    Dimension aDimension;

    int nComps = parent.countComponents();
    for (int i=0;i<nComps;i++) {
      aComponent = parent.getComponent(i);	        
      if (aComponent == itsFill) continue;
      aDimension = aComponent.getSize();
      
      //      System.err.println("posiziono un "+aComponent.getClass().getName()+" in "+currentX+", "+currentY+"senza cambiare le dimensioni ["+aDimension.width+", "+aDimension.height+"]");
      aComponent.setLocation(currentX, currentY);

      String how = (String) itsSpecials.get(aComponent);   
      if (how != null) {
	if (how.equals("stick_both")){// the component asked to be resized
	  if (itsType == VERTICAL)
	    aComponent.setSize(parent.getSize().width, aComponent.getPreferredSize().height);
	  else aComponent.setSize(aComponent.getPreferredSize().width, parent.getSize().height);
	}
      }
      else
	if (aDimension.height == 0 || aDimension.width == 0) {
	  System.err.println("warning! try to add a zero-height component ("+aComponent.getClass().getName()+") to a "+parent.getClass().getName());
	  aDimension = aComponent.getPreferredSize();
	  aComponent.setSize(aDimension);
	  System.err.println("dimensiono un "+aComponent.getClass().getName()+": "+aDimension.width+", "+aDimension.height);
	}
      //no else here: no specification, no null dimension, don't change dim.
      if (itsType == VERTICAL) currentY+=aComponent.getSize().height+itsInset;//the Y changes, the X not!
      else currentX += aComponent.getSize().width+itsInset;//the X changes, the Y not
    }
    //handle the filler field: stick it at the end, after all the other components have been
    //placed.
    if (itsFill != null){
      itsFill.setLocation(currentX, currentY);
      itsFill.setSize(parent.getSize().width-currentX, parent.getSize().height-currentY);
    }
  }
  
    public String toString() {
        String str = "["+((itsType == VERTICAL)?"vertical":"horizontal")+"]";
        return getClass().getName()+str;
    }
}



