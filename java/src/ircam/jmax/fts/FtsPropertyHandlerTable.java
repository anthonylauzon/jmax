


package ircam.jmax.fts;

/**
 *  Property handler table for the FtsObjects.
 */


class FtsPropertyHandlerTable
{
  /** 
   * Entry in the property handler table
   */

  class PropertyEntry
  {
    String name;
    FtsPropertyHandler handler;
    PropertyEntry next;

    PropertyEntry(String name, FtsPropertyHandler handler, PropertyEntry entries)
    {
      super();

      this.name    = name;
      this.handler = handler;
      this.next    = entries;
    }

    void callHandlers(String name, Object value)
    {
      if (name.equals(this.name))
	handler.propertyChanged(name, value);

      if (next != null)
	next.callHandlers(name, value);
    }

    public void removeHandler(String name, FtsPropertyHandler handler)
    {
      if (next != null)
	{
	  if (next.name.equals(name) && next.handler == handler)
	    next = next.next;
	  else
	    next.removeHandler(name, handler);
	}
    }
  }

  private PropertyEntry entries;

  /** Make an empty handler table */

  FtsPropertyHandlerTable()
  {
  }

  void addPropertyHandler(String name, FtsPropertyHandler handler)
  {
    entries = new PropertyEntry(name, handler, entries);
  }


  void removePropertyHandler(String name, FtsPropertyHandler handler)
  {
    if (entries != null)
      {
	if (entries.name.equals(name) && entries.handler == handler)
	  entries = entries.next;
	else
	  entries.removeHandler(name, handler);
      }
  }

  synchronized void callHandlers(String name, Object value)
  {
    if (entries != null)
      entries.callHandlers(name, value);
  }
}
