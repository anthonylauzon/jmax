 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Peter Hanappe.
// 

package ircam.jmax.script.pkg;
import java.util.*;
import java.io.*;

public interface SettingsTable
{
    /**
     *  getSetting
     *
     *  Get the value of the setting specified by the name. The first
     *  encountered value of the setting is returned. All variables
     *  used in the setting definition will be expanded.
     *
     *  @param name The name of the setting
     *  @returns The value of the setting or "" if not defined.  */
    public String getSetting(String name);

    /** 
     *  getSettingExpression
     * 
     *  Get the value of the setting specified by the name. The first
     *  encountered value of the setting is returned. All variables
     *  used in the setting definition are not expanded but displayed
     *  in their $() form.  
     *
     *  @param name The name of the setting
     *  @returns The value of the setting or "" if not defined.  */
    public String getSettingExpression(String name);

    /**
     *  setSetting
     *
     *  Set a setting. This method destructs the previous setting
     *  values. If the setting name does not exist, it is defined. If
     *  it already exists, the existing chain of values is replaced
     *  with the new setting value.  
     *
     *  @param name The name of the setting
     *  @param value The value of the setting
     */
    public void setSetting(String name, Object value);

    /**
     *  appendSetting
     *
     *  Appends a setting. This method does not destruct the previous
     *  setting values. If the setting name does not exist, it is
     *  defined. If it already exists, the new setting value is
     *  appended to the end of the existing chain of values.  
     *
     *  @param name The name of the setting
     *  @param value The value of the setting
     */
    public void appendSetting(String name, Object value);

    /**
     *  enumerateSetting
     *
     *  Returns an enumaration to inspect all the values defined for a
     *  given setting. All the values are expanded.
     *
     *  @param name The name of the setting
     *  @returns An enumaration of the values.
     */
    public Enumeration enumerateSetting(String name);

    /**
     *  enumerateSetting
     *
     *  Returns an enumaration to inspect all the values defined for a
     *  given setting. The values are not expanded.
     *
     *  @param name The name of the setting
     *  @returns An enumaration of the values.
     */
    public Enumeration enumerateSettingExpressions(String name);

    /**
     *  settings
     *
     *  Enumerate all the defined settings.
     *
     *  @returns An enumaration of the defined settings.
     */
    public Enumeration settings();

    /**
     *  loadSettings
     *
     *  Load the settings from a file.
     */
    public void loadSettings(String file) throws Exception;

    /**
     *  loadSettings
     *
     *  Load the settings from a file.
     */
    public void loadSettings(File file) throws Exception;
}






