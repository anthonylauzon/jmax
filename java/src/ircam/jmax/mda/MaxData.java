package ircam.jmax.mda;

import java.util.*;
import com.sun.java.swing.*;

/**
 * Superclass for all the Max Data
 * provide basic services, like tracing 
 * the data source and the data handler,
 * registering and disposing
 */

abstract public class MaxData
{
  protected MaxDataHandler handler = null;
  protected MaxDataSource  source  = null;
  protected MaxDataType    type    = null;
  private DefaultListModel editors = new DefaultListModel();
  protected String name = null; // name of the instance, for UI purposes
  protected String info = null; // comment field; store and get back, but don't use for semantic purpose

  /** A constructor that get only the type */

  public MaxData(MaxDataType type)
  {
    type.registerInstance(this);
    this.type = type;
  }

  /** A constructor that get the type and the instance name */

  public MaxData(MaxDataType type, String name)
  {
    type.registerInstance(this);
    this.type = type;
    this.name = name;
  }

  /**
   * Adding an editor; this method 
   * do not bind the editor on the data, and it is private
   */

  public void addEditor(MaxDataEditor editor)
  {
    editors.addElement(editor);
  }

  /** Removing the editor */

  public void removeEditor(MaxDataEditor editor)
  {
    editors.removeElement(editor);
  }


  /** Getting the list of editors, as a ListModel/DefaultListModel */

  public DefaultListModel getEditors()
  {
    return editors;
  }

  /** Getting the handler */

  public MaxDataHandler getDataHandler()
  {
    return handler;
  }

  /** Setting the handler */

  public  void setDataHandler(MaxDataHandler handler)
  {
    this.handler = handler;
  }

  /** Getting the data source */

  public MaxDataSource getDataSource()
  {
    return source;
  }

  /**
   * Bind this data to a new data the source; implicitly get a new Data Handler
   * it is the public method to call to set a data source.
   */

  public void bindToDataSource(MaxDataSource source)
  {
    setDataSource(source);
    setDataHandler(MaxDataHandler.findDataHandlerFor(source, this));
  }

  /** To set both the handler and the source at the same time;
    used in initialization
    */

  public void setDataSource(MaxDataSource source)
  {
    this.source = source;
    this.name = source.getName();
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


  /** Getting the name */

  public String getName()
  {
    return name;
  }

  /** Setting the name */

  public void setName(String name)
  {
    this.name = name;
  }

  /** Getting the info */

  public String getInfo()
  {
    return info;
  }

  /** Setting the info */

  public void setInfo(String info)
  {
    this.info = info;
  }

  /** edit: start an instance of the default editor for this type and
   * register it; it is not the only permitted way to start
   * an editor on a data instance, but it may be convenient.
   * the other way is to manually start an editor, and register it
   * with addEditor.
   */

  public MaxDataEditor edit() throws MaxDataException
  {
    if (type.getDefaultEditorFactory() != null)
      {
	MaxDataEditor editor = (MaxDataEditor) type.getDefaultEditorFactory().newEditor(this);
	
	addEditor(editor);

	return editor;
      }
    else
      throw new MaxDataException("No default editor for " + this);
  }


  /** return true if the instance can be saved to its current
   * data source
   */

  public boolean canSave()
  {
    if (source == null)
      return false;
    else if (handler == null)
      return false;
    else
      return handler.canSaveTo(source, this);
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


  /** Save the instance to a given source, without changing the
   *  original binding of the data 
   */

  public void saveTo(MaxDataSource source) throws MaxDataException
  {
    MaxDataHandler handler = MaxDataHandler.findDataHandlerFor(source, this);

    if (handler == null)
      throw new MaxDataException("Cannot save to " + source);

    for (int i = 0; i < editors.size() ; i++)
      {
	MaxDataEditor editor;

	editor = (MaxDataEditor) editors.elementAt(i);

	editor.syncData();
      }

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



