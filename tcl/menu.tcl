# 
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
# 

##
## Define the content of the popup menu on patcher background.
## The menu can be extended by adding similar commands
##

# Syntax for adding items to the patcherMenu 
#
#  patcherMenu add <name> <description>  <message>
#  patcherMenu add <submenus> <name> <description>  <message>
#  patcherMenu addAndEdit <name> <description>  <message>
#  patcherMenu addAndEdit <submenus> <name> <description>  <message>


patcherMenu addAndEdit "%jmax" "_object_" " " "Adding New Object"
patcherMenu addAndEdit "%jmax" "_message_box_" "messbox" "Adding New Message Box"
patcherMenu addAndEdit "%jmax" "_patcher_" "jpatcher" "Adding New Patcher"
patcherMenu add  "%jmax" "_inlet_" "inlet -1" "Adding New Inlet"
patcherMenu add  "%jmax" "_outlet_" "outlet -1" "Adding New Outlet"
patcherMenu addAndEdit "%jmax" "_comment_" "comment" "Adding New Comment"
patcherMenu add  "%jmax" "_button_" "button" "Adding New Button"
patcherMenu add  "%jmax" "_toggle_" "toggle" "Adding New Toggle"
patcherMenu add  "%jmax" "_slider_" "slider" "Adding New Slider"
patcherMenu add  "%jmax" "_intbox_" "intbox" "Adding New Integer Box"
patcherMenu add  "%jmax" "_floatbox_" "floatbox" "Adding New Float Box"

patcherMenu add  "%hot" "%jmax" "comment %jmax" "Adding Icon JMAX"
patcherMenu add  "%hot" "%new" "comment %new" "Adding Icon NEW"
patcherMenu add  "%hot" "%hot" "comment %hot" "Adding Icon HOT"
patcherMenu add  "%hot" "%stop" "comment %stop" "Adding Icon STOP"
patcherMenu add  "%hot" "%at_work" "comment %at_work" "Adding Icon AT WORK"
patcherMenu add  "%hot" "%question" "comment %question" "Adding Icon Question Mark"
patcherMenu add  "%hot" "%warning" "comment %warning" "Adding Icon WARNING"
patcherMenu add  "%hot" "%info" "comment %info" "Adding Icon INFO"
patcherMenu add  "%hot" "%linux" "comment %linux" "Adding Tux"
patcherMenu add  "%hot" "%forum" "comment %forum" "Adding Forum Logo"
