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


##
## Process parameters in the command line 
##

# Get commandline argument

if {[systemProperty "jmaxNoConsole"] != ""} then { set jmaxNoConsole [systemProperty "jmaxNoConsole"]}
if {[systemProperty "jmaxNoRegister"] != ""} then { set jmaxNoRegister [systemProperty "jmaxNoRegister"]}
if {[systemProperty "jmaxHost"] != ""} then { set jmaxHost [systemProperty "jmaxHost"]}
if {[systemProperty "jmaxConnection"] != ""} then { set jmaxConnection [systemProperty "jmaxConnection"]}
if {[systemProperty "jmaxPort"] != ""} then { set jmaxPort [systemProperty "jmaxPort"]}
if {[systemProperty "jmaxArch"] != ""} then { set jmaxArch [systemProperty "jmaxArch"]}
if {[systemProperty "jmaxMode"] != ""} then { set jmaxMode [systemProperty "jmaxMode"]}
if {[systemProperty "jmaxServerName"] != ""} then { set jmaxServerName [systemProperty "jmaxServerName"]}
if {[systemProperty "jmaxPkgPath"] != ""} then { set jmaxPkgPath [systemProperty "jmaxPkgPath"]}
if {[systemProperty "jmaxSplashScreen"] != ""} then { set jmaxSplashScreen [systemProperty jmaxSplashScreen]}
if {[systemProperty "jmaxFastFileBox"] != ""} then { set jmaxFastFileBox [systemProperty "jmaxFastFileBox"]}


# the following is done so properties it can be accessed from Java
#

setSystemProperty "jmaxFastFileBox" $jmaxFastFileBox
setSystemProperty "jmaxNoConsole" $jmaxNoConsole
setSystemProperty "jmaxNoRegister" $jmaxNoRegister


##
## Special handling for host type
##

if {[systemProperty "jmaxHostType"] != ""} then {
    set jmaxHostType [systemProperty "jmaxHostType"]
} else {
    if [catch {set foo $jmaxHostType}] then {
	if [info exists jmaxHostTypeTable($jmaxHost)] then {
	    set jmaxHostType $jmaxHostTypeTable($jmaxHost)
	} else {
	    set jmaxHostType "linuxpc"
	}
    }
}

