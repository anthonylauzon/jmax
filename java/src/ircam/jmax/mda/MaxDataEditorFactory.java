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
package ircam.jmax.mda;

/**
 * A MaxDataEditorFactory is an object able to generate an empty, unbound editor
 * for a given type; each data type have its own default editor 
 * factory, but editors can be also instantiated in other ways.
 */

public interface MaxDataEditorFactory
{
  abstract public boolean       canEdit(MaxData data);
  abstract public MaxDataEditor newEditor(MaxData data);
}

  
