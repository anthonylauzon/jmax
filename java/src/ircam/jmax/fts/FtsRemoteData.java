package ircam.jmax.fts;

import java.lang.*;

public abstract class FtsRemoteData {

  public int getId()
    {
      return id;
    }

  public void setId( int id)
    {
      this.id = id;
    }

  public String toString()
    {
      return this.getClass().getName() + "." + id;
    }

  abstract public void call( int key, Object args[]);

  public void remoteCall( int key, Object args[])
  {
    Fts.getServer().remoteCall( this, key, args);
  }

  protected int id;
};

