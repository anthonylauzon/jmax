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
package ircam.jmax.editors.ermes;

// All the ErmesObj that can be edited should implement this interface

public interface ErmesObjEditable
{
  public abstract void startEditing();
  public abstract void restartEditing();
}
    
