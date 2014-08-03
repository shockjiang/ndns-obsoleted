sudo nfd-stop
sudo nohup nfd-start>nfd.log &
sleep 5

nohup ./build/name-server-daemon / / -H /att >root.log &
nohup ./build/name-server-daemon /com /com -H /bt >com.log &
nohup ./build/name-server-daemon /com/skype /com/skype -H  /verizon >skype.log &

nohup ./build/name-server-daemon /net /net -H /sprint >net.log &
nohup ./build/name-server-daemon /net/ndnsim /net/ndnsim -H /ucla >ndnsim.log &
nohup ./build/name-server-daemon /net/ndnsim/git/doc /net/ndnsim/git/doc -H /ucla >git.doc.log &


./build/caching-resolver-daemon -H /att
