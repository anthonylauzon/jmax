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
package ircam.jmax.editors.explode;

/**
 * interface of the objects that want to be informed when a
 * graphic mapping changes. Used by the score and its setting panel
 */
public interface MappingListener 
{
  /** callback */
  abstract public void mappingChanged(String graphicName, String scoreName);

}
