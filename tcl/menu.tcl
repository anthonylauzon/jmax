# 
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# See file LICENSE for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# Based on Max/ISPW by Miller Puckette.
#
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
