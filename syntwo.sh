sudo pigpiod &
sleep 1

# get connected devices
nanokontrol=$( amidi -l | grep nanoKONTROL2 | awk -F ' ' '{print $2}')
launchpad=$( amidi -l | grep Launchpad | awk -F ' ' '{print $2}')
headphones=$( aplay -L | grep plughw:CARD=Headphones | awk -F ',' '{print $1}')
io2=$( aplay -L | grep plughw:CARD=io2 | awk -F ',' '{print $1}')

echo "nanokontrol : $nanokontrol"
echo "launchpad: $launchpad"
echo "headphones: $headphones"
echo "io2: $io2"

if [ -z "$io2" ];
then
	soundcard=$headphones
	device="hw:Headphones"
else
	soundcard=$io2
	device="hw:io2"
fi

if [ -n "$nanokontrol" ];
then
# launch syntwo
	echo "OK"
	/home/pi/syntwo.a $soundcard $nanokontrol
else
	echo "NOK";
fi
