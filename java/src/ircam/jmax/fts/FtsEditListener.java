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
  at the server level; for now, only objectRemoved is actually
  implemented.
  */

public interface FtsEditListener
{
  /** Called when an object is added in the listened server */

  public void objectAdded(FtsObject object);

  /** Called when an object is removed in the listened server */

  public void objectRemoved(FtsObject object);

  /** Called when a connection is added in the listened server */

  public void connectionAdded(FtsConnection connection);

  /** Called when a connection is removed in the listened server */

  public void connectionRemoved(FtsConnection connection);
}
