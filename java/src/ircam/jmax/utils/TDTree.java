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

package ircam.jmax.utils;

import java.awt.*;
import java.util.*;

/**
 * A generic 2D tree of objects that have 
 * two comparison criterias (example: segments).
 * This kind of trees have most of the caracteristics of the binary
 * search trees (log(n) search, but also balancing problems). 
 * @see TwoOrderObject
 */
public class TDTree {

  /**
   * constructor */
  public TDTree()
  {
    root = null;
    testType = FIRST;
  }

  /**
   * private constructor used in the recursive insert */
  protected TDTree(TwoOrderObject s, int theTestType)
  {
    root = s;
    testType = theTestType;
  }

  /**
   * returns the subtree whose root is the given argument */
  public TDTree find( TwoOrderObject obj)
  {
    if (obj == root) return this;
    else 
      if ( testType == FIRST)
	if (obj.firstLessOrEqual(root))
	  if (left != null) 
	    return left.find(obj);
	  else return null;
	else 
	  if (right != null)
	    return right.find(obj);
	  else return null;
      else
	if (obj.secondLessOrEqual(root))
	  if (left != null) 
	    return left.find(obj);
	  else return null;
	else 
	  if (right != null)
	    return right.find(obj);
	  else return null;
  }
  

  /**
   * remove an object form the tree */
  public void remove(TwoOrderObject obj)
  {
    //1. find it
    TDTree t = find(obj);
    //2. t contains the subtree with root=obj. Swap its root
    if (t == null) 
      {
	System.err.println("excuse me but the element seems not to be in the 2DTree");
	return;
      }
    else 
      {
	TDTree candidate = null; 
	if (t.left != null) candidate = rightMost(t.left);
	 else if (t.right != null) candidate = leftMost(t.right);
	
	if (candidate != null) 
	  {
	    if (candidate.father.left == candidate)
	      candidate.father.left = null;
	    else if (candidate.father.right == candidate)
	      candidate.father.right = null;
	    candidate.left = left;
	    candidate.right = right;
	    t = candidate;
	  }
	else 
	  {
	    if (t.father == null) return; //one-element tree...
	    if (t.father.left == t)
	      t.father.left = null;
	    else if (t.father.right == t)
	      t.father.right = null;
	  }
	
      }
  }

  /** find the rightmost leaf of the tree */
  private TDTree rightMost(TDTree t)
  {
    TDTree tl = null;
    TDTree tr = null;
    
    if (t == null)
      return null;

    if (t.right != null)
      tr = rightMost(t.right);
    else if (t.left != null)
      tl = rightMost(t.left);

    if (tr != null) return tr;
    else if (tl != null)
      return tl;
    else return t;
  }

  /** find the leftmost leaf of the tree */
  private TDTree leftMost(TDTree t)
  {
    TDTree tl = null;
    TDTree tr = null;
    
    if (t == null)
      return null;

    if (t.left != null)
      tl = leftMost(t.left);
    else if (t.right != null)
      tr = leftMost(t.right);

    if (tl != null) return tl;
    else if (tr != null)
      return tr;
    else return t;
  }

  /**
   * insert a TwoOrderObject in the tree */
  public TDTree insert(TwoOrderObject obj)
  {
    if (root == null) 
      {
	root = obj;
	return this;
      }

    if (testType == FIRST)
      {
	if (obj.firstLessOrEqual(root))
	  {
	    if (left == null)
	      {
		left = new TDTree(obj, SECOND);
		left.father = this;
		return left;
	      }
	    else 
	      {
		return left.insert(obj);
	      }
	  }
	else
	  {
	    if (right == null)
	      {
		right = new TDTree(obj, SECOND);
		right.father = this;
		return right;
	      }
	    else 
	      {
		return right.insert(obj);
	      }
	  }
      }
    else 
      if (obj.secondLessOrEqual(root))
	{
	  if (left == null)
	    {
	      left = new TDTree(obj, FIRST);
	      left.father = this;
	      return left;
	    }
	  else 
	    {
	      return left.insert(obj);
	    }
	}
      else
	{
	  if (right == null)
	    {
	      right = new TDTree(obj, FIRST);
	      right.father = this;
	      return right;
	    }
	  else 
	    {
	      return right.insert(obj);
	    }
	}
  }


  /**
   * returns an enumeration of all the objects in the tree
   * that have an intersection with the given range */
  public Enumeration rangeSearch(int first, int second)
  {
    temp.removeAllElements();
    privateRangeSearch(first, second, temp);
    return temp.elements();
  }

  /** internal use only... */
  protected void privateRangeSearch(int first, int second, MaxVector v)
  {
    if (testType == FIRST)
      {
	if (root.getFirst() <= second)
	  {
	    if (right != null) right.privateRangeSearch(first, second, v); 
	    if (root.getSecond() > first)
	      v.addElement(root);
	    if (left != null) left.privateRangeSearch(first, second, v);
	  }
	else if (left != null) left.privateRangeSearch(first, second, v);
      }
    else
      {
	if (root.getSecond() > first)
	  {
	    if (left != null) left.privateRangeSearch(first, second, v); 
	    if (root.getFirst() <= second)
	      v.addElement(root);
	    if (right != null) right.privateRangeSearch(first, second, v);
	  }
	else if (right != null)right.privateRangeSearch(first, second, v);
      }
  }

   /**
   * builds a (balanced) TDTree starting from an ordered vector of TwoOrderObjects 
   * @param first index of the first object to insert
   * @param last index of the last object to indert
   * @param elements the -ordered- vector of TwoOrderObjects 
   */
  public static TDTree buildTDTree(int first, int last, TwoOrderObject[] elements)
  {

     return buildTDTree(first, last, elements, FIRST);   
    
  }
  
  private static TDTree buildTDTree(int first, int last, TwoOrderObject[] elements, int comparison)
  {
    if (first> last) 
      {
	return null;
      }
    else if (first == last) 
      {
	return new TDTree(elements[first], comparison);
      }
    else
      {
	int half = (first+last)/2;
	TDTree t = new TDTree(elements[half], comparison);
	t.left = buildTDTree(first, half-1, elements, 1-comparison);
	if (t.left != null) t.left.father = t;

	t.right = buildTDTree(half+1, last, elements, 1-comparison);
	if (t.right != null) t.right.father = t;

	return t;
      }
  }

  /**
   * returns the root of this tree */
  public TwoOrderObject getRoot()
  {
    return root;
  }

  /**
   * An utility function to verbosely visit the tree */
  public static void visit(TDTree t)
  {
    if (t == null) 
      {
	System.err.println("empty");
	return;
      }
    else 
      {
	if (t.testType == FIRST) System.err.println("root: "+t.root.toString()+" (FIRST)");
	else if (t.testType == SECOND)  System.err.println("root: "+t.root.toString()+" (SECOND)");
	else System.err.println("WARNING - root: "+t.root.toString()+" (NO TEST TYPE)");
	System.err.println("left:");
	visit(t.left);
	System.err.println("right:");
	visit(t.right);
      }
  }

  /** utility debug function */
  public void checkConsistency()
  {
    if (root == null) 
      {
	System.err.println("null root!");
      }
    else 
      {
	if (testType == FIRST)
	  {
	    if (left != null)
	      {
		if (left.root.firstLessOrEqual(root))
		  left.checkConsistency();
		else System.err.println("inconsistent tree at node "+root.toString());
	      }
	    else if (right != null)
	      {
		if (root.firstLessOrEqual(right.root))
		  right.checkConsistency();
		else System.err.println("inconsistent tree at node "+root.toString());
	      }
	      
	  }
	else {
	  if (left != null)
	    {
	      if (left.root.secondLessOrEqual(root))
		left.checkConsistency();
	      else System.err.println("inconsistent tree at node "+root.toString());
	    }
	  else if (right != null)
	    {
	      if (root.secondLessOrEqual(right.root))
		right.checkConsistency();
	      else System.err.println("inconsistent tree at node "+root.toString());
	    }
	  
	}
	
      }
  }


  //--- Fields 
  protected TwoOrderObject root;
  int testType;
  public TDTree left;
  public TDTree right;
  public TDTree father;

  private MaxVector temp = new MaxVector();
  static protected int FIRST = 0;
  static protected int SECOND = 1;

}

