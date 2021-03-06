//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.configuration;

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.util.*;

// import javax.swing.*;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultCellEditor;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.UIManager;

// import javax.swing.border.*;
import javax.swing.border.EmptyBorder;

// import javax.swing.table.*;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellEditor;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

public class AudioConfigPanel extends JPanel implements Editor
{
  public AudioConfigPanel( ConfigurationEditor win, FtsAudioConfig config)  
  {
    window = win;
    audioConfig = config;

    if( audioConfig != null)
      audioConfig.setListener( this);

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    setBorder( BorderFactory.createEtchedBorder());

    /*********** buffer size and sampling rate *******************/
    JPanel bSizePanel = new JPanel();
    bSizePanel.setBorder( new EmptyBorder( 15, 15, 15, 15));
    bSizePanel.setLayout( new BoxLayout( bSizePanel, BoxLayout.X_AXIS)); 
    JLabel bSizeLabel = new JLabel("Buffer Size:");
    bSizeLabel.setHorizontalTextPosition(JLabel.RIGHT);
    bSizeLabel.setPreferredSize( new Dimension( 120, 20));
    bSizeLabel.setMinimumSize( new Dimension( 120, 20));
    bSizeCombo = new JComboBox( audioConfig.getBufferSizes());
    bSizeCombo.setFont( ConfigurationEditor.tableFont);
    bSizeCombo.setBackground( Color.white);
    bSizeCombo.addActionListener(new ActionListener() {
	public void actionPerformed(ActionEvent e) {
	  try{	    
	    int bs = Integer.parseInt( (String) bSizeCombo.getSelectedItem());	  
	    if( bs != audioConfig.getBufferSize())
	      audioConfig.requestSetBufferSize( bs);
	  }
	  catch( NumberFormatException nfe){}
	}
      });
    bSizeCombo.setPreferredSize( new Dimension( 160, 20));
    bSizeCombo.setMaximumSize( new Dimension( 160, 20));

    bSizePanel.add( Box.createHorizontalGlue());
    bSizePanel.add( bSizeLabel);
    bSizePanel.add( bSizeCombo);
    bSizePanel.add( Box.createHorizontalGlue());

    add( bSizePanel);

    JPanel sRatePanel = new JPanel();
    sRatePanel.setBorder( new EmptyBorder( 15, 15, 15, 15));
    sRatePanel.setLayout( new BoxLayout( sRatePanel, BoxLayout.X_AXIS)); 
    JLabel sRateLabel = new JLabel("Sampling Rate: ");
    sRateLabel.setHorizontalTextPosition( JLabel.RIGHT);
    sRateLabel.setPreferredSize( new Dimension( 120, 20));
    sRateLabel.setMinimumSize( new Dimension( 120, 20));
    sRateCombo = new JComboBox( audioConfig.getSamplingRates());
    sRateCombo.setFont( ConfigurationEditor.tableFont);
    sRateCombo.setBackground( Color.white);
    sRateCombo.addActionListener(new ActionListener() {
	public void actionPerformed(ActionEvent e) {
	  try{	    
	    int sr = Integer.parseInt( (String) sRateCombo.getSelectedItem());	    	  
	    if( sr != audioConfig.getSamplingRate())
	      audioConfig.requestSetSamplingRate( sr);
	  }
	  catch( NumberFormatException nfe){}
	}
      });
    sRateCombo.setPreferredSize( new Dimension( 160, 20));    
    sRateCombo.setMaximumSize( new Dimension( 160, 20));
    
    sRatePanel.add( Box.createHorizontalGlue());
    sRatePanel.add( sRateLabel);
    sRatePanel.add( sRateCombo);
    sRatePanel.add( Box.createHorizontalGlue());

    add( sRatePanel);

    /*********** Table  ******************************************/
    initDataModel();
    initCellEditors();
    defaultLabelFont = ConfigurationEditor.tableFont.deriveFont( Font.BOLD);

    audioTable = new JTable( audioModel){
	public TableCellEditor getCellEditor(int row, int column)
	{
	  switch( column)
	    {
	    case 0:
	      return super.getCellEditor(row, column);
	    case 1:
	      return inputCellEditor;
	    case 2:
	      updateChannelsCellEditor( row, column);
	      return channelsCellEditor;
	    case 3:
	      return outputCellEditor;
	    case 4:
	      updateChannelsCellEditor( row, column);
	      return channelsCellEditor;
	    default: 
	      return super.getCellEditor(row, column);
	    }
	}
      }; 
    audioTable.setPreferredScrollableViewportSize( new Dimension(TABLE_DEFAULT_WIDTH, TABLE_DEFAULT_HEIGHT));
    audioTable.setRowHeight(17);
    audioTable.setSelectionMode( ListSelectionModel.SINGLE_SELECTION);
    audioTable.getColumnModel().getColumn(2).setPreferredWidth(40);
    audioTable.getColumnModel().getColumn(2).setMaxWidth(40);
    audioTable.getColumnModel().getColumn(4).setPreferredWidth(40);
    audioTable.getColumnModel().getColumn(4).setMaxWidth(40);

    scrollPane = new JScrollPane( audioTable);
    scrollPane.setPreferredSize( new Dimension( TABLE_DEFAULT_WIDTH, TABLE_DEFAULT_HEIGHT));

    scrollPane.addMouseListener( new MouseAdapter(){
	public void mousePressed( MouseEvent e){
	  audioTable.clearSelection();
	  stopEdit();
          audioTable.transferFocus();
	}
      });

    add( scrollPane);

    if( config == null)
      audioTable.setEnabled( false);
  }

  public void stopEdit()
  {
    if( audioTable.getCellEditor() != null)
      audioTable.getCellEditor().stopCellEditing();
  }

  void initDataModel()
  {
    audioModel = new AudioTableModel( audioConfig);
    
    if( audioConfig == null) return;

    bSizeCombo.setSelectedItem(""+audioConfig.getBufferSize());
    sRateCombo.setSelectedItem(""+audioConfig.getSamplingRate());

    for( Enumeration e = audioConfig.getLabels(); e.hasMoreElements();)
      audioModel.addRow( (FtsAudioLabel) e.nextElement());    
  }

  void initCellEditors()
  {
    if( audioConfig == null) return;

    channelsCombo = new JComboBox();
    channelsCombo.setBackground( Color.white);
    channelsCombo.setFont( ConfigurationEditor.tableFont);
    channelsCellEditor = new DefaultCellEditor( channelsCombo);

    sourceCombo = new JComboBox( audioConfig.getSources());
    sourceCombo.setBackground( Color.white);
    sourceCombo.setFont( ConfigurationEditor.tableFont);
    inputCellEditor = new DefaultCellEditor( sourceCombo);

    destCombo = new JComboBox( audioConfig.getDestinations());
    destCombo.setBackground( Color.white);
    destCombo.setFont( ConfigurationEditor.tableFont);
    outputCellEditor = new DefaultCellEditor( destCombo);
  }

  void updateChannelsCellEditor( int rowId, int colId)
  {
    channelsCombo.removeAllItems();
    
    Object val = audioModel.getValueAt( rowId, colId-1);
    if( val != null)
      {
	int ch;
	if( colId == 2)
	  ch = audioConfig.getInDeviceChannels( (String)val);
	else
	  ch = audioConfig.getOutDeviceChannels( (String)val);
	
	for( int i = 0; i < ch; i++)
	  channelsCombo.addItem( ""+i);
      }
    channelsCombo.validate();
    channelsCellEditor = new DefaultCellEditor( channelsCombo);
    revalidate(); 
  }

  void update( FtsAudioConfig ac)
  {    
    audioConfig = ac;
    initDataModel();
    audioTable.setModel( audioModel);
    audioTable.revalidate();
    revalidate();    
    window.pack();
  }

  public void sourcesChanged()
  {  
    initCellEditors();
    audioTable.revalidate();
    revalidate(); 
  }

  public void destinationsChanged()
  {
    initCellEditors();
    audioTable.revalidate();
    revalidate(); 
  }

  public void samplingRatesChanged()
  {  
    String[] sr = audioConfig.getSamplingRates();
    sRateCombo.removeAllItems();
    for( int i = 0; i < sr.length; i++)
      sRateCombo.addItem( sr[i]);
    
    sRateCombo.validate();
    revalidate(); 
  }

  public void bufferSizesChanged()
  {  
    String[] bs = audioConfig.getBufferSizes();
    bSizeCombo.removeAllItems();
    for( int i = 0; i < bs.length; i++)
      bSizeCombo.addItem( bs[i]);

    bSizeCombo.validate();
    revalidate(); 
  }

  public void bufferSizeChanged()
  {  
    bSizeCombo.setSelectedItem(""+audioConfig.getBufferSize());
  }

  public void samplingRateChanged()
  {  
    sRateCombo.setSelectedItem(""+audioConfig.getSamplingRate());
  }

  public void labelInserted( int index, FtsAudioLabel label)
  {
    if(!label.getLabel().equals( audioModel.getValueAt( index, 0)))
      audioModel.forceValue( index, 0, label.getLabel());
    audioModel.forceValue( index, 1, label.getInput());
    audioModel.forceValue( index, 3, label.getOutput());
  }

  public void labelInputChanged( FtsAudioLabel label)
  {
    int index = audioConfig.getLabelIndex( label);
    if( index == -1) return;

    int numCh = audioConfig.getInDeviceChannels( label.getInput());
    int currentCh = numCh;
    try{	    
      currentCh = Integer.parseInt(  (String)audioModel.getValueAt( index, 2));
    }
    catch( NumberFormatException nfe){} 

    if( currentCh >= numCh)
      audioModel.forceValue( index, 2, "0");

    validateTable();
  }

  public void labelInChannelChanged( FtsAudioLabel label)
  {
    int index = audioConfig.getLabelIndex( label);
    if( index == -1) return;
    
    int ch = label.getInputChannel();
    if( !((String)audioModel.getValueAt( index, 2)).equals( ""+ch))
      audioModel.forceValue( index, 2, ""+ch);

    validateTable();
  }

  public void labelOutputChanged( FtsAudioLabel label)
  {
    int index = audioConfig.getLabelIndex( label);
    if( index == -1) return;
    
    int numCh = audioConfig.getOutDeviceChannels( label.getOutput());
    int currentCh = numCh;
    try{	    
      currentCh = Integer.parseInt(  (String)audioModel.getValueAt( index, 4));
    }
    catch( NumberFormatException nfe){} 
    if( currentCh >= numCh)
      audioModel.forceValue( index, 4, "0");

    validateTable();
  }

  public void labelOutChannelChanged( FtsAudioLabel label)
  {
    int index = audioConfig.getLabelIndex( label);
    if( index == -1) return;
    
    int ch = label.getOutputChannel();
    if( !((String)audioModel.getValueAt( index, 4)).equals( ""+ch))
      audioModel.forceValue( index, 4, ""+ch);

    validateTable();
  }

  public void labelNameChanged( FtsAudioLabel label)
  {
    int index = audioConfig.getLabelIndex( label);
    if( index == -1) return;
    
    audioModel.forceValue( index, 0, label.getLabel());
    validateTable();
  }

  void validateTable()
  {
    audioTable.revalidate();

    revalidate();

    if( audioTable.getCellEditor() != null)
      audioTable.getCellEditor().stopCellEditing();

    audioTable.transferFocus();
  }

  void add()
  {
    int sel = audioTable.getSelectedRow();

    audioModel.addRow( sel);

    if( sel==-1)
      sel = audioTable.getRowCount()-2;

    audioTable.getSelectionModel().setSelectionInterval( sel+1, sel+1);
  }

  void delete()
  {
    audioModel.removeRow( audioTable.getSelectedRow());
  }

  /*********************************************************
   ***   Table model for the Labels JTable             ***
   *********************************************************/
  class AudioTableModel extends AbstractTableModel 
  {
    AudioTableModel( FtsAudioConfig ac)
    {
      super();
      audioConfig = ac;
    }

    public int getColumnCount() 
    { 
      return 5;
    }
  
    public Class getColumnClass(int col)
    {
      return String.class;
    }

    public boolean isCellEditable(int row, int col)
    {
      /*if((row == 0) && ( col == 0)) return false;
	else return true;*/
      return true;
    }

    public String getColumnName(int col)
    {
      switch(col)
	{
	case 0: 
	  return "label";
	case 1: 
	  return "in device";
	case 2: 
	  return "in #";
	case 3: 
	  return "out device";
	case 4: 
	  return "out #";
	default:
	  return "";
	}
    }

    public int getRowCount() { 
      return size; 
    }

    public void addRow(int index)
    {
      size++;
      Object[][] temp;
      
      if( size > rows)
      {
        temp = new Object[size+5][5];
        rows = size+5;
      }
      else
        temp = new Object[size][5];

      if(index == -1)
        {
          for(int i = 0; i < size-1; i++)
          {
            temp[i][0] = data[i][0];
            temp[i][1] = data[i][1];
            temp[i][2] = data[i][2];
            temp[i][3] = data[i][3];
            temp[i][4] = data[i][4];
          }
          temp[size-1][0] = "unnamed";
          temp[size-1][1] = null;
          temp[size-1][2] = null;
          temp[size-1][3] = null;
          temp[size-1][4] = null;
          index = size-2;
        }
      else
        {
          for(int i = 0; i < index+1; i++)
          {
            temp[i][0] = data[i][0];
            temp[i][1] = data[i][1];
            temp[i][2] = data[i][2];
	    temp[i][3] = data[i][3];
	    temp[i][4] = data[i][4];
          }

          temp[index+1][0] = "unnamed";
          temp[index+1][1] = null;
          temp[index+1][2] = null;
          temp[index+1][3] = null;
          temp[index+1][4] = null;

          for(int j = index+2; j < size; j++)
          {
            temp[j][0] = data[j-1][0];
            temp[j][1] = data[j-1][1];
            temp[j][2] = data[j-1][2];
	    temp[j][3] = data[j-1][3];
	    temp[j][4] = data[j-1][4];
          }
        }

      data = temp;

      if( audioConfig != null)
        audioConfig.requestInsertLabel( index+1, "unnamed");

      fireTableDataChanged();      
    }

    public void addRow( FtsAudioLabel label)
    {
      size++;
      Object[][] temp;

      if( size > rows)
        {
          temp = new Object[size+5][5];
          rows = size+5;
        }
      else
        temp = new Object[size][5];

      for(int i = 0; i < size-1; i++)
        {
          temp[i][0] = data[i][0];
          temp[i][1] = data[i][1];
          temp[i][2] = data[i][2];
          temp[i][3] = data[i][3];
          temp[i][4] = data[i][4];
        }
      
      temp[size-1][0] = label.getLabel();
      temp[size-1][1] = label.getInput();
      temp[size-1][2] = (label.getInputChannel() == -1) ? null : ""+label.getInputChannel();
      temp[size-1][3] = label.getOutput();
      temp[size-1][4] = (label.getOutputChannel() == -1) ? null : ""+label.getOutputChannel();
      
      data = temp;
    }

    public void removeRow(int rowId)
    {
      if(( rowId == 0) || (( rowId == -1)&&( size == 1))) return;
      if(size > 0)
	{
	  size--;    
	  if(rowId >= 0)
	    {
	      for(int i = rowId; i < size; i++)
		{
		  data[i][0] = data[i+1][0];
		  data[i][1] = data[i+1][1];
		  data[i][2] = data[i+1][2];
		  data[i][3] = data[i+1][3];
		  data[i][4] = data[i+1][4];
		}
	    }
          else rowId = size;
          
	  data[size][0] = null;
	  data[size][1] = null;
	  data[size][2] = null;
	  data[size][3] = null;
	  data[size][4] = null;

	  fireTableDataChanged();
	  audioConfig.requestRemoveLabel( rowId);
	}
    }

    public Object getValueAt(int row, int col) 
    { 
      if(row > size) return null;
      else
	return data[row][col];
    }

    public void forceValue(int row, int col, Object value)
    {
       data[row][col] = value;
       fireTableCellUpdated(row, col);
    }

    public void setRowValues(int row, Object v1, Object v2, Object v3, Object v4, Object v5)
    {
      if( v1 != null)
      {
        data[row][0] = v1;
        fireTableCellUpdated(row, 0);
      }
      if( v2 != null)
      {
        data[row][1] = v2;
        fireTableCellUpdated(row, 1);
      }
      if( v3 != null)
      {
        data[row][2] = v3;
        fireTableCellUpdated(row, 2);
      }
      if( v4 != null)
      {
        data[row][3] = v4;
        fireTableCellUpdated(row, 3);
      }
      if( v5 != null)
      {
        data[row][4] = v5;
        fireTableCellUpdated(row, 4);
      }
    }
    
    public void setValueAt(Object value, int row, int col) 
    {
      if(row > size) return;
      
      Object oldValue = data[row][col];
      
      if((((String)oldValue) != null) && ( ((String)oldValue).equals( value)))
	return;
      
      data[row][col] = value;
      
      fireTableCellUpdated(row, col);
    
      if( audioConfig != null)
	{
	  FtsAudioLabel label = audioConfig.getLabelAt( row);
	  int chn = 0;
	  switch( col)
	    {	      
	    case 0:
	      ((String)value).trim();
              if( ((String)value).equals("")) value = "unnamed";
	      
	      if( oldValue == null)
		audioConfig.requestInsertLabel( row, (String)value);	      
	      else
		label.requestChangeLabel( (String)value);
	      break;
	    case 1:
	      label.requestSetInput( (String)value);
	      break;
	    case 2:
	      chn = channelsCombo.getSelectedIndex();
	      if( chn > -1)
		label.requestSetInChannel( chn);
	      break;
	    case 3:
	      label.requestSetOutput( (String)value);
	      break;
	    case 4:
	      chn = channelsCombo.getSelectedIndex();
	      if( chn > -1)
		label.requestSetOutChannel( chn);
	      break;
	    default:
	      return;
	    }
	}
    }
    int size = 0;
    int rows = 0;
    Object data[][];
    FtsAudioConfig audioConfig;
  }

  /************* interface Editor ************************/
  public EditorContainer getEditorContainer()
  {
    return (EditorContainer)window;
  }

  public void close(boolean doCancel)
  {
    window.close();
  }
  public void save()
  {
    window.save();
  }
  public void saveAs()
  {
    window.saveAs();
  }
  public void print(){} 

  /********************************/
  private JTable audioTable;
  private DefaultCellEditor inputCellEditor;
  private DefaultCellEditor outputCellEditor;
  private DefaultCellEditor channelsCellEditor;

  private JScrollPane scrollPane;
  private AudioTableModel audioModel;
  private ConfigurationEditor window;
  private FtsAudioConfig audioConfig;
  private final int TABLE_DEFAULT_WIDTH = 450;
  private final int TABLE_DEFAULT_HEIGHT = 160;
  private Font defaultLabelFont;

  JComboBox channelsCombo, bSizeCombo, sRateCombo, sourceCombo, destCombo;
}




