##
## app.tcl --
##
##     Internet version check module.   
##
## Copyright (C) 2002 Stacho Mudrak
## 
## $Date: $
## $RCSfile: $
## $Revision: $
##
## -------------------------------------------------------------------- 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
## -------------------------------------------------------------------- 

package require msgcat
namespace import ::msgcat::mc

set xth(about,nvr) {}
set xth(about,session) [list]


proc xth_ivc {} {
  global xth
  set newver {}
  catch {
    
    package require http 2.0
    
    # proxy configuration
    set httpproxy {}
    global tcl_platform env
    set winregkey [join {
         HKEY_CURRENT_USER
         Software Microsoft Windows
         CurrentVersion "Internet Settings"
    } \\]
    if {[info exists env(http_proxy)]} {
        set httpproxy $env(http_proxy)
    } else {
        if {$tcl_platform(platform) == "windows"} {
            package require registry 1.0
            array set reg {ProxyEnable 0 ProxyServer "" ProxyOverride {}}
            catch {
                set reg(ProxyEnable) [registry get $winregkey "ProxyEnable"]
                set reg(ProxyServer) [registry get $winregkey "ProxyServer"]
            }
            if {![string is bool $reg(ProxyEnable)]} {
                set reg(ProxyEnable) 0
            }
            if {$reg(ProxyEnable)} {
               if { [string first ";" $reg(ProxyServer)] == -1 } {
                   set httpproxy $reg(ProxyServer)
               } else {
                  foreach tmp [split $reg(ProxyServer) ";"] {
                      if { [string match "http=*" $tmp] } {
                          set httpproxy [string range $tmp 5 end]
                          break
                      }
                  }
                  unset tmp
               }
            }
		if {[string length $httpproxy] == 0} {
			set reg_auto {}
			catch {
				set reg_auto [registry get $winregkey "AutoConfigURL"]
				if {[string length $reg_auto] > 0} {
    					set token [::http::geturl $reg_auto]
				      upvar #0 $token pxs
					regexp -all -nocase {proxy\s+([^\:]+\:\d+)} $pxs(body) dum httpproxy
				}
			}
		}
        }
    }

    if {[string length $httpproxy] > 0} {
        if {![regexp {\w://.*} $httpproxy]} {
            set httpproxy "http://$httpproxy"
        }
        if {[regexp {\w://(.*)\:(\d*)} $httpproxy dum proxyhost proxyport]} {
          ::http::config -proxyhost $proxyhost -proxyport $proxyport
          set xth(proxy) "$proxyhost:$proxyport"
        }
    }
    
    set token [::http::geturl "http://therion.speleo.sk/update.php" -query [::http::formatQuery version $xth(about,ver) therion 1]]
    upvar #0 $token state
    if {[regexp {^(\d+)\.(\d+)\.(\d+)} $xth(about,ver) dump mv1 mv2 mv3]} {
      if {[regexp {th\_version\s+(\d+)\.(\d+)\.(\d+)} $state(body) dump nv1 nv2 nv3]} {
        if {($mv1 < $nv1) || (($mv1 == $nv1) && ($mv2 < $nv2)) || (($mv1 == $nv1) && ($mv2 == $nv2) && ($mv3 < $nv3))} {
          set newver "$nv1.$nv2.$nv3"
        }
      }
    }
  }
  set xth(about,nvr) $newver
}


