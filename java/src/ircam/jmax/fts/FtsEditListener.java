//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts;

/**
  This interface define a listener of the changes
  at the global level; for now, only objectRemoved is actually
  called.
  */

public interface FtsEditListener
{
  public void objectAdded(FtsObject object);
  public void objectRemoved(FtsObject object);

  public void connectionAdded(FtsConnection connection);
  public void connectionRemoved(FtsConnection connection);
}
