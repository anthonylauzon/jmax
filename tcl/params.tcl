#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  

##
## Process parameters in the command line 
##

# Get commandline argument

if {[systemProperty "jmaxNoConsole"] != ""} then { set jmaxNoConsole [systemProperty "jmaxNoConsole"]}
if {[systemProperty "jmaxNoRegister"] != ""} then { set jmaxNoConsole [systemProperty "jmaxNoRegister"]}
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
	    set jmaxHostType "octane"
	}
    }
}

