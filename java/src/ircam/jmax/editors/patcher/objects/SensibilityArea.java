package ircam.jmax.editors.patcher.objects;

import ircam.jmax.editors.patcher.*;

public class SensibilityArea implements DisplayObject
{
  ErmesObject object; // the object this area belong to

  public void setObject(ErmesObject object)
  {
    this.object = object;
  }

  public ErmesObject getObject()
  {
    return  object;
  }
}

