//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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


