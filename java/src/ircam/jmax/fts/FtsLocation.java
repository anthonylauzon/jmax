package ircam.jmax.fts;

/** An abstract location for data inside FTS.
 *To be really implemented 
 */

public class FtsLocation
{
  FtsObject object = null;
  String name;

  public FtsLocation(String name)
  {
    this.name = name;
  }


  public FtsLocation(FtsObject object)
  {
    this.object = object;
  }


  protected void fetchObject()
  {
    object = FtsServer.getServer().getRootObject().getObjectByName(name);
  }

  public FtsObject getObject()
  {
    if (object == null)
      fetchObject();

    return object;
  }
  
  public boolean exists()
  {
    if (object == null)
      fetchObject();

    return object != null;
  }
  
  public String getName()
  {
    return name;
  }
}
