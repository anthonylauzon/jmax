;;
;; jMax
;; Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
;; 
;; This program is free software; you can redistribute it and;or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
;; 
;; See file LICENSE for further informations on licensing terms.
;; 
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;; 
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;; 
;; Based on Max;ISPW by Miller Puckette.
;; 
;; Author: Peter Hanappe
;;

;;
;; System Icons
;;
(declare-icon "_max_patcher_file_"  (file-cat dir "images" "mini_icon_pat.gif"))
(declare-icon "_jmax_patcher_file_" (file-cat dir "images" "mini_icon_jmax.gif"))
(declare-icon "_jmax_tiny_logo_"    (file-cat dir "images" "jmax_logo_tiny.gif"))
(declare-icon "_patcher_"           (file-cat dir "images" "tool_patcher.gif"))
(declare-icon "_object_"            (file-cat dir "images" "tool_ext.gif"))
(declare-icon "_error_object_"      (file-cat dir "images" "tool_err.gif"))
(declare-icon "_comment_"           (file-cat dir "images" "tool_text.gif"))
(declare-icon "_message_box_"       (file-cat dir "images" "tool_mess.gif"))
(declare-icon "_inlet_"             (file-cat dir "images" "tool_in.gif"))
(declare-icon "_outlet_"            (file-cat dir "images" "tool_out.gif"))
(declare-icon "_button_"            (file-cat dir "images" "tool_bang.gif"))
(declare-icon "_toggle_"            (file-cat dir "images" "tool_toggle.gif"))
(declare-icon "_slider_"            (file-cat dir "images" "tool_slider.gif"))
(declare-icon "_intbox_"            (file-cat dir "images" "tool_int.gif"))
(declare-icon "_floatbox_"          (file-cat dir "images" "tool_float.gif"))
(declare-icon "_lock_mode_"         (file-cat dir "images" "tool_lock_mode.gif"))
(declare-icon "_edit_mode_"         (file-cat dir "images" "tool_edit_mode.gif"))
(declare-icon "_up_"                (file-cat dir "images" "tool_up.gif"))

;;
;; Icons usefull for graphic decorations inside comments
;;
(declare-icon "%jmax"               (file-cat dir "images" "jmax_logo_tiny.gif"))
(declare-icon "%new"                (file-cat dir "images" "new.gif"))
(declare-icon "%hot"                (file-cat dir "images" "hot.gif"))
(declare-icon "%stop"               (file-cat dir "images" "stop.gif"))
(declare-icon "%at_work"            (file-cat dir "images" "at_work.gif"))
(declare-icon "%question"           (file-cat dir "images" "question.gif"))
(declare-icon "%warning"            (file-cat dir "images" "warning.gif"))
(declare-icon "%info"               (file-cat dir "images" "info.gif"))
(declare-icon "%linux"              (file-cat dir "images" "linux.gif"))
(declare-icon "%forum"              (file-cat dir "images" "forum.gif"))


;;
;; Define the content of the popup menu on patcher background.
;; The menu can be extended by adding similar commands
;;
(patcher-menu "addAndEdit" "%jmax" "_object_" " " "Adding New Object")
(patcher-menu "addAndEdit" "%jmax" "_message_box_" "messbox" "Adding New Message Box")
(patcher-menu "addAndEdit" "%jmax" "_patcher_" "jpatcher" "Adding New Patcher")
(patcher-menu "add"        "%jmax" "_inlet_" "inlet -1" "Adding New Inlet")
(patcher-menu "add"        "%jmax" "_outlet_" "outlet -1" "Adding New Outlet")
(patcher-menu "addAndEdit" "%jmax" "_comment_" "comment" "Adding New Comment")
(patcher-menu "add"        "%jmax" "_button_" "button" "Adding New Button")
(patcher-menu "add"        "%jmax" "_toggle_" "toggle" "Adding New Toggle")
(patcher-menu "add"        "%jmax" "_slider_" "slider" "Adding New Slider")
(patcher-menu "add"        "%jmax" "_intbox_" "intbox" "Adding New Integer Box")
(patcher-menu "add"        "%jmax" "_floatbox_" "floatbox" "Adding New Float Box")

(patcher-menu "add"  "%hot" "%jmax" "comment %jmax" "Adding Icon JMAX")
(patcher-menu "add"  "%hot" "%new" "comment %new" "Adding Icon NEW")
(patcher-menu "add"  "%hot" "%hot" "comment %hot" "Adding Icon HOT")
(patcher-menu "add"  "%hot" "%stop" "comment %stop" "Adding Icon STOP")
(patcher-menu "add"  "%hot" "%at_work" "comment %at_work" "Adding Icon AT WORK")
(patcher-menu "add"  "%hot" "%question" "comment %question" "Adding Icon Question Mark")
(patcher-menu "add"  "%hot" "%warning" "comment %warning" "Adding Icon WARNING")
(patcher-menu "add"  "%hot" "%info" "comment %info" "Adding Icon INFO")
(patcher-menu "add"  "%hot" "%linux" "comment %linux" "Adding Tux")
(patcher-menu "add"  "%hot" "%forum" "comment %forum" "Adding Forum Logo")

