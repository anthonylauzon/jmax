package ircam.jmax.fts;

/**
 * Handle an FTS property change. <br>
 * This interface should be implemented in order
 * to install an object as property handler for
 * an FTS object.<br>
 * Should go away an be substituted by Java Beans standard
 * methods.
 *
 * @author mdc
 * @see  FtsObject#put
 * @see  FtsObject#get
 */

public interface FtsPropertyHandler
{
  /**
   * Called when a property change.
   *
   * @param name the property name
   * @param value the new property value.
   */

  void propertyChanged(FtsObject object, String name, Object value);
}



