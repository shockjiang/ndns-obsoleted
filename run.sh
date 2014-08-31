#! /bin/sh
#e.g: ./run -s,   which run commands one by one. Pressing any key to continue
#e.g: ./run -b 5, which run from test 5

ROOT="./build"
CMDOUT="/dev/null"
ZONEINFO="zone-details.txt"
DLGTINFO="delegation-details.txt"
TMP=""
ENABLEFILTER=1
BEGINSTEP=4
CURRENTSTEP=1
ERRORN=0
#############

if [ "$1"x == "-s"x ] ; then
  STEPBYSTEP=$1
  elif [ "$1"x == "-b"x ] ; then
    ENABLEFILTER=1
    if [ $# -gt 1 ] ; then
      BEGINSTEP=$2
    fi
fi


#whether to pause and wait for input when execute command
#0 parameters, use global variable $STEPBYSTEP
function do_pause() {
  if [ "${STEPBYSTEP}"x = "-s"x ] ;
  then
      echo "Press Any Key to run: $1"
      read
  else
      echo $1
  fi
}

#run functionality related commands
#3 parameters at most, 1 optional. $1 is command to be executed
#$2 (optional) is whether to redirect the output to $CMDOUT (0 is no, else is yes-default.)
#$3 (optional) is whether add ROOT as the prefix of command (0 is no, else is yes-default.)
#$4 (optional) is the expected return value (none is default)
function do_run() {
  if [ "$2"x != "0"x ] ;
  then
    cmd="$1 > $CMDOUT"
  fi
  
  #echo "p1=$1, p2(out)=$2, p3(path)=$3, p4(ret)=$4"

  if [ "$3"x != "0"x ] ;
  then
    cmd="${ROOT}/$cmd"
  fi

  do_pause "$cmd"

  eval "$cmd"
  ret=$?
  #echo $ret
  if [ $# -eq 4 ] && [ $4 -lt 0 ] ; then
    expected=$[256+$4]
  else
    expected=$4
  fi
  
  if [ $# -eq 4 ] && [ $ret != $expected ] ; then
    ERRORN=$[$ERRORN+1]
    echo "!!!!!!!  $cmd (returned $ret != expected $4) !!!!!!!!!!"
  fi
  echo 
}

#run functionality related commands
#2 parameters at most, 1 optional. $1 is command to be executed
#$2 (optional) is whether to redirect the output to $CMDOUT (0 is yes, else is no-default.)
function do_run_daemon() {
  cmd="nohup ${ROOT}/$1"
  do_run "$cmd" "$2" 0
}


#will create the keys and register the zone on the local ndns
#1 parameter at most, $1 is the zone name
function func_zone_build() {
  cmd="zone-create $1 -s"
  do_run "$cmd"

  LINE=$(tail -n 1 $ZONEINFO)
  OLD_IFS="$IFS"
  IFS=" "
  arr=($LINE)
  IFS="$OLD_IFS"
  
  CERT=${arr[4]}
  cmd="zone-register $1 $CERT"
  do_run "$cmd"

}

#1 parameter at most, $1 is the site's name
function func_site_create() {
  cmd="site-create $1 -s"
  do_run "$cmd"
}

# parameters at most, $1 is the zone's name
# $2 is the column index (starting from 0)
# return value is stored in global variable TMP
function do_extract_zone_info() {
  fin=$ZONEINFO
  index=$2
  if [ "$2"x == "KSK"x ] ; then
    index=1
  elif [ "$2"x == "KSK-CERT"x ] ; then
    index=2
  elif [ "$2"x == "DSK"x ] ; then
    index=3
  elif [ "$2"x == "DSK-CERT"x ] ; then
    index=4
  elif [ "$2"x == "KSK-PKS"x ] ; then #KSK's parent signed cert
    fin=$DLGTINFO
    index=2
	fi
	
	line=$(cat $fin | grep "^$1 " | tail -n 1)
  OLD_IFS="$IFS"
  IFS=" "
  arr=($line)
  IFS="$OLD_IFS"
  
	#echo $fin, $1, $2, $index
  TMP=${arr[$index]}
}

#delegate a subzone on the parent(current working) zone
#4 parameters at most, 2 optional
#$1 is current working zone, $2 is subzone
#$3 is "--rrtype=..", $4 is "--rrdata=..."
function func_zone_delegate() {
  LINE=$(cat $ZONEINFO | grep "^$1 " | tail -n 1)
  OLD_IFS="$IFS"
  IFS=" "
  arr=($LINE)
  IFS="$OLD_IFS"
  
  p1=${arr[0]}
  p3=${arr[3]}
  
  LINE=$(cat $ZONEINFO | grep "^$2 " | tail -n 1)
  OLD_IFS="$IFS"
  IFS=" "
  arr=($LINE)
  IFS="$OLD_IFS"
  
  p2=${arr[0]}
  p4=${arr[2]}
  
  if [ $# -gt 2 ] ;
  then
      p5=$3  #--rrtype
      p0=site-delegate
  else
      p5=""
      p0=zone-delegate
  fi
  
  if [ $# -gt 3 ] ;
  then
      p6=$4   #--rrdata
  else
      p6=""
  fi

  cmd="$p0 $p1 $p2 $p3 $p4 $p5 $p6 -s"
  
  if [ "$1"x = "$p1"x -a "$2"x = "$p2"x ];
  then
      do_run "$cmd"
  else
      echo "CMD: $cmd"
      echo "ERROR: p1=$p1 p2=$p2 while intented p1=$1 p2=$2"
  fi
}

#delegate a site on the parent(current working) zone
#4 parameters at most, 2 optional
#$1 is current working zone, $2 is subzone
#$3 is "--rrtype=..", $4 is "--rrdata=..."
function func_site_delegate() {
    if [ $# -lt 3 ] ; then
      rrtype="--rrtype=TXT"
    else
      rrtype="$3"
    fi
    func_zone_delegate "$1" "$2" "$rrtype" "$4"
}


function func_reset_settings() {
  sqlite3 src/db/ndns-local.db < src/db/ndns-db-tables.sql
  echo "reset DB"

  ndnsec delete -n /
  ndnsec delete -n /net
  ndnsec delete -n /net/ndnsim
  ndnsec delete -n /net/ndnsim/www
  ndnsec delete -n /net/ndnsim
  ndnsec delete -n /net/ndnsim/doc/www
  ndnsec delete -n /net/rtc
  ndnsec delete -n /net/rtc/shock
  echo "reset local KeyChain"

  rm $ZONEINFO
  rm $DLGTINFO
  rm *.cert
}

function test_add_zone_and_site() {
  func_zone_build "/"
  
  func_zone_build "/net"
  
  func_zone_delegate "/" "/net"

  func_site_create "/net/ndnsim/doc/www"
  
  func_zone_build "/net/ndnsim"
  
  func_zone_delegate "/net" "/net/ndnsim"
  
  #if not specified the rrtype (by$3), will create a TXT
  func_site_delegate "/net/ndnsim" "/net/ndnsim/doc/www"
  
  func_site_create "/net/ndnsim/www"
  
  func_site_delegate "/net/ndnsim" "/net/ndnsim/www" "--rrtype=TXT"
  
  func_zone_build "/net/rtc"
  
  func_zone_delegate "/net" "/net/rtc"
  
  func_site_create "/net/rtc/shock"
  
  func_site_delegate "/net/rtc" "/net/rtc/shock" "--rrtype=FH" "--rrdata=/att"
}

function func_start_daemon() {
  pkill server-daemon
  pkill resolver-daemon

  cmd="server-daemon / / > root.log &"
  do_run_daemon "$cmd" 0
  
  cmd="server-daemon /net /net > net.log &"
  do_run_daemon "$cmd" 0
  
  cmd="server-daemon /net/ndnsim /net/ndnsim > net-ndnsim.log &"
  do_run_daemon "$cmd" 0
  
  cmd="server-daemon /net/rtc /net/rtc > net-rtc.log &"
  do_run_daemon "$cmd" 0
  
  cmd="resolver-daemon /lo > resolver.log &"
  do_run_daemon "$cmd" 0

  sleep 5
}



function test_dig() {
  cmd="dig /net/ndnsim/www /lo"
  do_run "$cmd" 1 1 1
  # $1=cmd $2=out-redirect   $3=root-path-append  $4=expected-ret
  cmd="dig /net/ndnsim/www /lo --rrtype=TXT"
  do_run "$cmd" 1 1 1

  cmd="dig /net/ndnsim /lo --rrtype=NS"
  do_run "$cmd" 1 1 1
  
  cmd="dig /net/ndnsim2 /lo --rrtype=NS"
  do_run "$cmd" 1 1 0

  cmd="dig /net/ndnsim/doc/www /lo --rrtype=TXT"
  do_run "$cmd" 1 1 1

  cmd="dig /net/ndnsim/doc/www /lo --rrtype=FH"
  do_run "$cmd" 1 1 0
  
  
  #note: get none Data back, if the rrtype is illegal
  cmd="dig /net/rtc/shock /lo --rrtype=FH2"
  do_run "$cmd" 1 1 -1

  #warning: returned rrst's rrset and zone info is not right
  cmd="dig /net/rtc/shock /lo --rrtype=FH"
  do_run "$cmd" 0 1 1
  
  do_extract_zone_info "/net" "DSK"
  key=$TMP
  cmd="dig $key /lo --rrtype=ID-CERT"
  do_run "$cmd" 1 1 1
  
  do_extract_zone_info "/net" "KSK-PKS"
  key=$TMP  
  cmd="dig $key /lo --rrtype=ID-CERT"
  do_run "$cmd" 1 1 1
  
}


function test_update() {
  cmd="update /net/rtc /shock FH /ucla ADD"
  do_run "$cmd" 1 1 1
  
  cmd="dig /net/rtc/shock /lo --rrtype=FH"
  do_run "$cmd" 1 1 2
  
  cmd="update /net/rtc /shock FH /ucla REMOVE"
  do_run "$cmd" 1 1 1
  cmd="dig /net/rtc/shock /lo --rrtype=FH"
  do_run "$cmd" 0 1 1
  
  cmd="update /net/rtc /shock FH /ucla ADD"
  do_run "$cmd" 1 1 1
  cmd="dig /net/rtc/shock /lo --rrtype=FH"
  do_run "$cmd" 0 1 2
  
  cmd="update /net/rtc /shock FH /t-mobile REPLACE-ALL"
  do_run "$cmd" 1 1 1
  cmd="dig /net/rtc/shock /lo --rrtype=FH"
  do_run "$cmd" 0 1 1
  
  #add non-existing rrset
  cmd="update /net/rtc /alex FH /sprint ADD"
  do_run "$cmd" 1 1 1
  #duplicate ADD ($cmd is changed in do_run)
  cmd="update /net/rtc /alex FH /sprint ADD"
  do_run "$cmd" 1 1 1
  cmd="dig /net/rtc/alex /lo --rrtype=FH"
  do_run "$cmd" 0 1 1
  
  
  #remove non-existing rr
  cmd="update /net/rtc /alex FH /sprint2 REMOVE"
  do_run "$cmd" 1 1 1
  cmd="dig /net/rtc/alex /lo --rrtype=FH"
  do_run "$cmd" 1 1 1
  
  #remove non-existing rrset
  cmd="update /net/rtc /yingdi FH /sprint2 REMOVE"
  do_run "$cmd" 1 1 1
  cmd="dig /net/rtc/yingdi /lo --rrtype=FH"
  do_run "$cmd" 1 1 0
  
  
}

function filter() {
  if [ $ENABLEFILTER -gt 0 ]  && [ $CURRENTSTEP -lt $BEGINSTEP ] ; then
    echo "pass function $1"
  else
    echo "CURRENTSTEP=$CURRENTSTEP BEGINSTEP=$BEGINSTEP"
    $1
  fi
  CURRENTSTEP=$[$CURRENTSTEP+1]
}


filter func_reset_settings

filter test_add_zone_and_site

#daemon needs to get the id from database
filter func_start_daemon

filter test_dig

filter test_update


echo "----------------    Errors: ${ERRORN}    --------------------"