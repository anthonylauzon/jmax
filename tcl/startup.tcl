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
#  jMax startup file
#

# open the console

openConsole

# run the official jMax splash screen 
# if not supressed by the user in .jmaxrc

#if {$jmaxSplashScreen != "hide"} then {
#  splash $jmaxRootDir/images/Splash.gif [getMaxVersion]
#}

# start jMax server

if {[systemProperty "jmaxServerDir"] != ""} then { 
    set jmaxServerDir [systemProperty "jmaxServerDir"]
} else { 
    if [catch {set foo $jmaxServerDir}] then {
	## set directory for jMax server executable
	set jmaxServerDir "$jmaxRootDir/bin"
    }
}

puts "jMax copyright (C) 1994, 1995, 1998, 1999 IRCAM - Centre Georges Pompidou"
puts "jMax is free software with ABSOLUTELY NO WARRANTY."
puts "(see file LICENSE for more details)"

if {$jmaxConnection == "tcl"} {
  puts "jMax starting server on $jmaxHost ($jmaxHostType) via TCP/IP"
} elseif {$jmaxConnection == "udp"} {
  puts "jMax starting server on $jmaxHost ($jmaxHostType) via UDP"
} elseif {$jmaxConnection == "socket"} {
  puts "jMax connecting to server on $jmaxHost ($jmaxHostType) via TCP/IP port $jmaxPort"
}

ftsconnect $jmaxServerDir $jmaxServerName $jmaxConnection $jmaxHost $jmaxServerOptions $jmaxPort

# hello server
sync

# load gui server side objects
package require system
package require guiobj

# load tutorials
sourceFile $jmaxRootDir/tutorials/basics/project.env

# load installation default packages
# Use sourceFile as a protection against user errors
#
#sourceFile $jmaxRootDir/config/packages.tcl
sourceFile $jmaxRootDir/tcl/require.tcl

jmaxSetSampleRate $jmaxSampleRate
jmaxSetAudioBuffer $jmaxAudioBuffer

# run the start Hooks, if there
# if not there, run the defaultStart hooks
# that should be defined in the machine type declaration files
# so we are sure the correct configuration is there ...

runHooks "platformStart"

if {[runHooks "start"] != "true"} {
    runHooks "defaultStart"
}
