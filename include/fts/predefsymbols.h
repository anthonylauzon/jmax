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
 */

/**
 * These are the predefined symbols, available in global 
 * variables, so that fts_new_symbol() is not necessary.
 * 
 * Definition of predefined symbol is centralized in this file 
 * with a PREDEF_SYMBOL macro that is redefined before including 
 * this file. See symbols.h and symbols.c for use of this macro.
 * So adding a new predefined symbol, though probably not necessary,
 * can be done by adding:
 * PREDEF_SYMBOL( variable_name, symbol_text)
 * at the end of the list.
 * 
 */

/* Messages and names */
PREDEF_SYMBOL( fts_s_void, "void")
PREDEF_SYMBOL( fts_s_float, "float")
PREDEF_SYMBOL( fts_s_int, "int")
PREDEF_SYMBOL( fts_s_number, "number")
PREDEF_SYMBOL( fts_s_pointer, "pointer")
PREDEF_SYMBOL( fts_s_string, "string")
PREDEF_SYMBOL( fts_s_symbol, "symbol")
PREDEF_SYMBOL( fts_s_class, "class")
PREDEF_SYMBOL( fts_s_array, "array")
PREDEF_SYMBOL( fts_s_tuple, "tuple")
PREDEF_SYMBOL( fts_s_message, "message")
PREDEF_SYMBOL( fts_s_list, "list")
PREDEF_SYMBOL( fts_s_connection, "connection")
PREDEF_SYMBOL( fts_s_yes, "yes")
PREDEF_SYMBOL( fts_s_no, "no")
PREDEF_SYMBOL( fts_s_on, "on")
PREDEF_SYMBOL( fts_s_off, "off")
PREDEF_SYMBOL( fts_s_init, "init")
PREDEF_SYMBOL( fts_s_delete, "delete")
PREDEF_SYMBOL( fts_s_put, "put")
PREDEF_SYMBOL( fts_s_put_prologue, "put_prologue")
PREDEF_SYMBOL( fts_s_put_epilogue, "put_epilogue")
PREDEF_SYMBOL( fts_s_n_inlets, "n_inlets")
PREDEF_SYMBOL( fts_s_n_outlets, "n_outlets")
PREDEF_SYMBOL( fts_s_bang, "bang")
PREDEF_SYMBOL( fts_s_assign, "assign")
PREDEF_SYMBOL( fts_s_compare, "compare")
PREDEF_SYMBOL( fts_s_set, "set")
PREDEF_SYMBOL( fts_s_new, "new")
PREDEF_SYMBOL( fts_s_get, "get")
PREDEF_SYMBOL( fts_s_row, "row")
PREDEF_SYMBOL( fts_s_col, "col")
PREDEF_SYMBOL( fts_s_append, "append")
PREDEF_SYMBOL( fts_s_print, "print")
PREDEF_SYMBOL( fts_s_post, "post")
PREDEF_SYMBOL( fts_s_flush, "flush")
PREDEF_SYMBOL( fts_s_clear, "clear")
PREDEF_SYMBOL( fts_s_fill, "fill")
PREDEF_SYMBOL( fts_s_stop, "stop")
PREDEF_SYMBOL( fts_s_start, "start")
PREDEF_SYMBOL( fts_s_next, "next")
PREDEF_SYMBOL( fts_s_restore, "restore")
PREDEF_SYMBOL( fts_s_open, "open")
PREDEF_SYMBOL( fts_s_openEditor, "openEditor")
PREDEF_SYMBOL( fts_s_closeEditor, "closeEditor")
PREDEF_SYMBOL( fts_s_destroyEditor, "destroyEditor")
PREDEF_SYMBOL( fts_s_upload, "upload")
PREDEF_SYMBOL( fts_s_update, "update")
PREDEF_SYMBOL( fts_s_upload_child, "upload_child")
PREDEF_SYMBOL( fts_s_close, "close")
PREDEF_SYMBOL( fts_s_load, "load")
PREDEF_SYMBOL( fts_s_read, "read")
PREDEF_SYMBOL( fts_s_write, "write")
PREDEF_SYMBOL( fts_s_dump, "dump")
PREDEF_SYMBOL( fts_s_save, "save")
PREDEF_SYMBOL( fts_s_import, "import")
PREDEF_SYMBOL( fts_s_export, "export")
PREDEF_SYMBOL( fts_s_save_dotpat, "save_dotpat")
PREDEF_SYMBOL( fts_s_find, "find")
PREDEF_SYMBOL( fts_s_keep, "keep")
PREDEF_SYMBOL( fts_s_find_errors, "find_errors")
PREDEF_SYMBOL( fts_s_find_friends, "find_friends")
PREDEF_SYMBOL( fts_s_anything, "anything")
PREDEF_SYMBOL( fts_s_update_gui, "update_gui")
PREDEF_SYMBOL( fts_s_update_real_time, "update_real_time")
PREDEF_SYMBOL( fts_s_propagate_input, "propagate_input")
PREDEF_SYMBOL( fts_s_thru, "thru")
PREDEF_SYMBOL( fts_s_sig, "sig")
PREDEF_SYMBOL( fts_s_default, "default")
PREDEF_SYMBOL( fts_s_sched_ready, "sched_ready")
PREDEF_SYMBOL( fts_s_sched_error, "sched_error")
PREDEF_SYMBOL( fts_s_fifo_size, "fifo_size")
PREDEF_SYMBOL( fts_s_get_array, "get_array")
PREDEF_SYMBOL( fts_s_set_from_array, "set_from_array")
PREDEF_SYMBOL( fts_s_set_from_instance, "set_from_instance")
PREDEF_SYMBOL( fts_s_timebase_tick, "timebase_tick")
PREDEF_SYMBOL( fts_s_timebase_locate, "timebase_locate")
PREDEF_SYMBOL( fts_s_paste, "paste")
PREDEF_SYMBOL( fts_s_copy, "copy")
PREDEF_SYMBOL( fts_s_insert, "insert")
PREDEF_SYMBOL( fts_s_remove, "remove")
PREDEF_SYMBOL( fts_s_undefined, "undefined")
PREDEF_SYMBOL( fts_s_internal, "internal")
PREDEF_SYMBOL( fts_s_input, "input")
PREDEF_SYMBOL( fts_s_output, "output")
PREDEF_SYMBOL( fts_s_right, "right")
PREDEF_SYMBOL( fts_s_left, "left")
PREDEF_SYMBOL( fts_s_add_listener, "add_listener")
PREDEF_SYMBOL( fts_s_remove_listener, "remove_listener")
PREDEF_SYMBOL( fts_s_empty_object, "empty object")
PREDEF_SYMBOL( fts_s_get_element, "get_element")

/* client messages */
PREDEF_SYMBOL( fts_s_openFileDialog, "openFileDialog")

/* syntax symbols */
PREDEF_SYMBOL( fts_s_comma, ",")
PREDEF_SYMBOL( fts_s_quote, "'")
PREDEF_SYMBOL( fts_s_dollar, "$")
PREDEF_SYMBOL( fts_s_semi, ";")

/* Object properties */;
PREDEF_SYMBOL( fts_s_value, "value")
PREDEF_SYMBOL( fts_s_orientation, "orientation")
PREDEF_SYMBOL( fts_s_max_value, "maxValue")
PREDEF_SYMBOL( fts_s_min_value, "minValue")
PREDEF_SYMBOL( fts_s_name, "name")
PREDEF_SYMBOL( fts_s_x, "x")
PREDEF_SYMBOL( fts_s_wx, "wx")
PREDEF_SYMBOL( fts_s_y, "y")
PREDEF_SYMBOL( fts_s_wy, "wy")
PREDEF_SYMBOL( fts_s_width, "w")
PREDEF_SYMBOL( fts_s_ww, "ww")
PREDEF_SYMBOL( fts_s_height, "h")
PREDEF_SYMBOL( fts_s_wh, "wh")
PREDEF_SYMBOL( fts_s_range, "range")
PREDEF_SYMBOL( fts_s_font, "font")
PREDEF_SYMBOL( fts_s_fontSize, "fs")
PREDEF_SYMBOL( fts_s_fontStyle, "fst")
PREDEF_SYMBOL( fts_s_setFont, "setFont")
PREDEF_SYMBOL( fts_s_error, "error")
PREDEF_SYMBOL( fts_s_error_description, "errdesc")
PREDEF_SYMBOL( fts_s_size, "size")
PREDEF_SYMBOL( fts_s_comment, "comment")
PREDEF_SYMBOL( fts_s_layer, "layer")
PREDEF_SYMBOL( fts_s_color, "color")
PREDEF_SYMBOL( fts_s_flash, "flash")
PREDEF_SYMBOL( fts_s_state, "state")
PREDEF_SYMBOL( fts_s_abstraction, "abstraction")
PREDEF_SYMBOL( fts_s_abstraction_path, "abstraction_path")
PREDEF_SYMBOL( fts_s_template, "template")
PREDEF_SYMBOL( fts_s_template_path, "template_path")
PREDEF_SYMBOL( fts_s_data_path, "data_path")
PREDEF_SYMBOL( fts_s_require, "require")
PREDEF_SYMBOL( fts_s_package, "package")
PREDEF_SYMBOL( fts_s_project, "project")
PREDEF_SYMBOL( fts_s_help, "help")
PREDEF_SYMBOL( fts_s_summary, "summary")
PREDEF_SYMBOL( fts_s_patcher_type, "type")
PREDEF_SYMBOL( fts_s_named_defaults, "named_defaults")
PREDEF_SYMBOL( fts_s_spost_description, "spost_description")
PREDEF_SYMBOL( fts_s_set_dirty, "setDirty")
PREDEF_SYMBOL( fts_s_midi_config, "midi_config")
PREDEF_SYMBOL( fts_s_audio_config, "audio_config")

/* Symbols related to builtin classes */
PREDEF_SYMBOL( fts_s_old_patcher, "patcher")
PREDEF_SYMBOL( fts_s_patcher, "jpatcher")
PREDEF_SYMBOL( fts_s_inlet, "inlet")
PREDEF_SYMBOL( fts_s_outlet, "outlet")
PREDEF_SYMBOL( fts_s_label, "label")
PREDEF_SYMBOL( fts_s_param, "param")
PREDEF_SYMBOL( fts_s_preset, "preset")
PREDEF_SYMBOL( fts_s_receive, "receive")
PREDEF_SYMBOL( fts_s_send, "send")
PREDEF_SYMBOL( fts_s_audiofile, "audiofile")
PREDEF_SYMBOL( fts_s_define, "define")

/* Global variable definition */
PREDEF_SYMBOL( fts_s_double_colon, "::")

/* Expression operators */
PREDEF_SYMBOL( fts_s_plus, "+")
PREDEF_SYMBOL( fts_s_minus, "-")
PREDEF_SYMBOL( fts_s_times, "*")
PREDEF_SYMBOL( fts_s_div, "/")
PREDEF_SYMBOL( fts_s_power, "**")
PREDEF_SYMBOL( fts_s_open_par, "(")
PREDEF_SYMBOL( fts_s_closed_par, ")")
PREDEF_SYMBOL( fts_s_open_sqpar, "[")
PREDEF_SYMBOL( fts_s_closed_sqpar, "]");
PREDEF_SYMBOL( fts_s_open_cpar, "{");
PREDEF_SYMBOL( fts_s_closed_cpar, "}");
PREDEF_SYMBOL( fts_s_dot, ".");
PREDEF_SYMBOL( fts_s_percent, "%");
PREDEF_SYMBOL( fts_s_shift_left, "<<");
PREDEF_SYMBOL( fts_s_shift_right, ">>");
PREDEF_SYMBOL( fts_s_bit_and, "&");
PREDEF_SYMBOL( fts_s_bit_or, "|");
PREDEF_SYMBOL( fts_s_bit_xor, "^");
PREDEF_SYMBOL( fts_s_bit_not, "~");
PREDEF_SYMBOL( fts_s_logical_and, "&&");
PREDEF_SYMBOL( fts_s_logical_or, "||");
PREDEF_SYMBOL( fts_s_logical_not, "!")
PREDEF_SYMBOL( fts_s_equal_equal, "==")
PREDEF_SYMBOL( fts_s_not_equal, "!=")
PREDEF_SYMBOL( fts_s_greater, ">")
PREDEF_SYMBOL( fts_s_greater_equal, ">=")
PREDEF_SYMBOL( fts_s_smaller, "<")
PREDEF_SYMBOL( fts_s_smaller_equal, "<=")
PREDEF_SYMBOL( fts_s_conditional, "?")
PREDEF_SYMBOL( fts_s_colon, ":")
PREDEF_SYMBOL( fts_s_equal, "=")

/* Predefined variable names */
PREDEF_SYMBOL( fts_s_args, "args")
