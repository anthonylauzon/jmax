package ircam.jmax.editors.patcher.objects;

import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.interactions.*;

public class SensibilityArea 
{
  // Sensibility area are stored in a pool.

  // Stack of availables instances

  static private MaxVector pool = new MaxVector();

 // top of stack, pointer to next avaible free place

  static int top = 0; 

  static public SensibilityArea get(Object target, int squeack)
  {
    if (top == 0)
      return new SensibilityArea(target, squeack);
    else
      {
	SensibilityArea area;

	top = top - 1;
	area = (SensibilityArea) pool.elementAt(top);
	area.target = target;
	area.squeack = squeack;
	area.cost = 0;
	area.transparent = false;
	area.number = 0;

	return area;
      }
  }

  private boolean transparent;
  private int     cost;
  private int     squeack;
  private Object  target;
  private int     number;

  public String toString()
  {
    return ("SensibilityArea<" + (transparent ? "transparent, " : "nonTransparent, ") +
	    target + ", " + Squeack.describe(squeack) + "," + number + "," + cost + ">");
	    
  }

  SensibilityArea(Object target, int squeack)
  {
    this.target = target;
    this.squeack = squeack;
    cost = 0;
    transparent = false;
    number = 0;
  }


  // Sensibility area should be disposed to be reused

  public void dispose()
  {
    pool.setSize(top + 1);
    pool.setElementAt(this, top);
    top = top + 1;
  }
  
  public boolean isTransparent()
  {
    return transparent;
  }

  public void setTransparent(boolean v)
  {
    transparent = v;
  }

  public int getCost()
  {
    return cost;
  }

  public void setCost(int v)
  {
    cost = v;
  }

  public Object getTarget()
  {
    return target;
  }

  public void setTarget(Object v)
  {
    target = v;
  }

  public int getNumber()
  {
    return number;
  }

  public void setNumber(int v)
  {
    number = v;
  }

  public int getSqueack()
  {
    return squeack;
  }

  public void setSqueack(int v)
  {
    squeack = v;
  }

}


