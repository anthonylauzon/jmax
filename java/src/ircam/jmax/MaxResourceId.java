//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax;
import java.util.*;

/**
 * The resource type identification.
 * Every resources has a name and it is associated with an editor 
 * and a set of extensions.
 */
public class MaxResourceId {
  public String resourceName = new String();
  public Vector resourceExtensions = new Vector();
  public String preferred_resource_handler = new String();
  
  public MaxResourceId(String theName) {
    resourceName = theName;
  }
  
  public String GetName(){
    return resourceName;
  }
  
  public String GetHandler(){
    return preferred_resource_handler;
  }
}



