; 
;  jMax
;  
;  Copyright (C) 1999 by IRCAM
;  All rights reserved.
;  
;  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
;
;  This program may be used and distributed under the terms of the 
;  accompanying LICENSE.
; 
;  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
;  for DISCLAIMER OF WARRANTY.
;  
; 

;;
;; Define the content of the popup menu on patcher background.
;; The menu can be extended by adding similar commands
;;

; Syntax for adding items to the patcherMenu 
;
;  (patcher-menu "add" () <name> <description> <message>)
;  (patcher-menu "add" <submenus> <name> <description> <message>)
;  (patcher-menu "addAndEdit" () <name> <description> <message>)
;  (patcher-menu "addAndEdit" <submenus> <name> <description> <message>)

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
