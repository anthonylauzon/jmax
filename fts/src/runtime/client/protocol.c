/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Francois Dechelle
 *
 */

#include <fts/runtime/client/protocol.h>

const char *protocol_printable_cmd( int cmd)
{
  switch( cmd) {
  case CLIENTMESS_CODE:
    return "CLIENTMESS_CODE";
  case CLIENTPROP_CODE:
    return "CLIENTPROP_CODE";
  case CONNECTION_RELEASE_CODE:
    return "CONNECTION_RELEASE_CODE";
  case DECLARE_ABSTRACTION_CODE:
    return "DECLARE_ABSTRACTION_CODE";
  case DECLARE_ABSTRACTION_PATH_CODE:
    return "DECLARE_ABSTRACTION_PATH_CODE";
  case DECLARE_TEMPLATE_CODE:
    return "DECLARE_TEMPLATE_CODE";
  case DECLARE_TEMPLATE_PATH_CODE:
    return "DECLARE_TEMPLATE_PATH_CODE";
  case DELETE_CONNECTION_CODE:
    return "DELETE_CONNECTION_CODE";
  case DELETE_OBJECT_CODE:
    return "DELETE_OBJECT_CODE";
  case DOWNLOAD_CONNECTION_CODE:
    return "DOWNLOAD_CONNECTION_CODE";
  case DOWNLOAD_OBJECT_CODE:
    return "DOWNLOAD_OBJECT_CODE";
  case FTS_SHUTDOWN_CODE:
    return "FTS_SHUTDOWN_CODE";
  case GETALLPROP_CODE:
    return "GETALLPROP_CODE";
  case GETPROP_CODE:
    return "GETPROP_CODE";
  case LOAD_PATCHER_BMAX_CODE:
    return "LOAD_PATCHER_BMAX_CODE";
  case LOAD_PATCHER_DPAT_CODE:
    return "LOAD_PATCHER_DPAT_CODE";
  case MESSAGE_CODE:
    return "MESSAGE_CODE";
  case NEW_CONNECTION_CODE:
    return "NEW_CONNECTION_CODE";
  case NEW_OBJECT_CODE:
    return "NEW_OBJECT_CODE";
  case NEW_OBJECT_VAR_CODE:
    return "NEW_OBJECT_VAR_CODE";
  case OBJECT_RELEASE_CODE:
    return "OBJECT_RELEASE_CODE";
  case OBJECT_RELEASE_DATA_CODE:
    return "OBJECT_RELEASE_DATA_CODE";
  case POST_CODE:
    return "POST_CODE";
  case POST_LINE_CODE:
    return "POST_LINE_CODE";
  case PUTPROP_CODE:
    return "PUTPROP_CODE";
  case RECOMPUTE_ERRORS_CODE:
    return "RECOMPUTE_ERRORS_CODE";
  case REDEFINE_CONNECTION_CODE:
    return "REDEFINE_CONNECTION_CODE";
  case REDEFINE_OBJECT_CODE:
    return "REDEFINE_OBJECT_CODE";
  case REDEFINE_PATCHER_CODE:
    return "REDEFINE_PATCHER_CODE";
  case REMOTE_CALL_CODE:
    return "REMOTE_CALL_CODE";
  case SAVE_PATCHER_BMAX_CODE:
    return "SAVE_PATCHER_BMAX_CODE";
  case SYNC_CODE:
    return "SYNC_CODE";
  case SYNC_DONE_CODE:
    return "SYNC_DONE_CODE";
  case UCS_CODE:
    return "UCS_CODE";
  case UPDATE_GROUP_END_CODE:
    return "UPDATE_GROUP_END_CODE";
  case UPDATE_GROUP_START_CODE:
    return "UPDATE_GROUP_START_CODE";
  case VOID_COMMAND_CODE:
    return "VOID_COMMAND_CODE";
  }

  return "???";
}

