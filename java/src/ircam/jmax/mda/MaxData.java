package ircam.jmax.mda;

import java.util.*;

/**
 * Superclass for all the Max Data
 * provide basic services, like tracing 
 * the data source and the data handler,
 * registering and disposing
 */

abstract public class MaxData
{
  private MaxDataHandler handler = null;
  private MaxDataSource  source  = null;
  private MaxDataType    type    = null;
  private Vector editors = new Vector();

  /** The constructor get only the type (mandatory) */

  public MaxData(MaxDataType type)
  {
    this.type = type;
  }

  /**
   * Adding an editor; this method 
   * bind the editor on the data.
   */

  public void addEditor(MaxDataEditor editor)
  {
    // Do first the add, so that editData can assume
    // the housekeeping is already consistent.

    editors.addElement(editor);

    try
      {
	editor.editData(this);
      }
    catch (MaxDataException e)
      {
	editors.removeElement(editor);
	return;
      }
  }

  /** Removing the editor */

  public void removeEditor(MaxDataEditor editor)
  {
    // Do first the quitEdit, so that it can assume
    // the housekeeping is still consistent.

    editor.quitEdit();
    editors.removeElement(editor);
  }

  /** Getting the handler */

  public MaxDataHandler getDataHandler()
  {
    return handler;
  }

  /** Setting the handler */

  public void setDataHandler(MaxDataHandler handler)
  {
    this.handler = handler;
  }

  /** Getting the data source */

  public MaxDataSource getDataSource()
  {
    return source;
  }

  /** Setting the source; implicitly get a new Data Handler
   */

  public void setDataSource(MaxDataSource source)
  {
    this.source = source;
    this.handler = MaxDataHandler.findDataHandlerFor(source);
  }

  /** Getting the type */

  public MaxDataType getDataType()
  {
    return type;
  }

  /** Setting the type */

  public void setDataType(MaxDataType type)
  {
    this.type = type;
  }

  /** edit: start an instance of the default editor for this type and
   * bind it to this data; it is not the only permitted way to start
   * an editor on a data instance, but it may be convenient.
   */

  public MaxDataEditor edit() throws MaxDataException
  {
    if (type.defaultEditorClass != null)
      {
	try
	  {
	    MaxDataEditor editor = (MaxDataEditor) type.defaultEditorClass.newInstance();
	
	    addEditor(editor);

	    return editor;
	  }
	catch (InstantiationException e)
	  {
	    throw new MaxDataException("Cannot instantiate default editor for " + this);
	  }
	catch (IllegalAccessException e)
	  {
	    throw new MaxDataException("Illegal Access instantiating default editor for " + this);
	  }
      }
    else
      throw new MaxDataException("No default editor for " + this);
  }

  /** return true if the instance can be saved to its current
   * data source
   */

  public boolean canSave()
  {
    if (handler != null)
      return handler.canSaveTo(source);
    else
      return false;
  }
    
  /** Save the instance to its data source */

  public void save() throws MaxDataException
  {
    if (! canSave())
      throw new MaxDataException("Cannot save to " + source);

    for (int i = 0; i < editors.size() ; i++)
      {
	MaxDataEditor editor;

	editor = (MaxDataEditor) editors.elementAt(i);

	editor.syncData();
      }

    if (handler == null)
      throw new MaxDataException("No data handler for " + source);

    handler.saveInstance(this);
  }

  /** Disposing this instance */

  public void dispose()
  {
    for (int i = 0; i < editors.size() ; i++)
      {
	MaxDataEditor editor;

	editor = (MaxDataEditor) editors.elementAt(i);

	editor.quitEdit();
      }

    type.disposeInstance(this);
    handler = null;
    type = null;
    source = null;
  }

  /** Signal that the data is changed to all the editors */

  void dataChanged(Object reason)
  {
    for (int i = 0; i < editors.size() ; i++)
      {
	MaxDataEditor editor;

	editor = (MaxDataEditor) editors.elementAt(i);

	editor.dataChanged(reason);
      }
  }


  /** Get the MaxData content; the content is the real thing to
   * Edit, and its type is not specified
   */

  abstract public Object getContent();
}
