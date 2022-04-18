import serial
import skyfield
import ephem
from skyfield.api import load, wgs84, EarthSatellite
from time import sleep


#Format strings to be of the exact form: XXX:XX:XX.X
def padSat(padStr):
    strs = padStr.split(" ")
    degrees = str(int(strs[0].replace("deg", "")))
    minutes = str(int(strs[1].replace("'", "")))
    seconds = str(float(strs[2].replace('"', "")))

    return degrees + ":" + minutes + ":" + seconds

def padZeros(padStr):
    padStrSplit = padStr.split(":")
   
    if(len(padStrSplit[0]) == 1): padStrSplit[0] = "00" + padStrSplit[0]
    elif(len(padStrSplit[0]) == 2): padStrSplit[0] = "0" + padStrSplit[0]

    if(len(padStrSplit[1]) == 1): padStrSplit[1] = "0" + padStrSplit[1]
    if(len(padStrSplit[2]) == 1): padStrSplit[2] = "0" + padStrSplit[2]

    output = padStrSplit[0] + ":" + padStrSplit[1] + ":" + padStrSplit[2]
    return output


def main():
    ts = load.timescale()

    #Obtain satellite information from online library
    stations_url = 'http://celestrak.com/NORAD/elements/stations.txt'
    satellites = load.tle_file(stations_url, reload=True)
    print('Loaded', len(satellites), 'satellites')

    ISS = satellites[0]

    print(ISS)

    #Assign current location as Princeton
    princeton = wgs84.latlon(+40.350701806262855, -74.65177232410804)

    #Obtain the North Star's location
    pton = ephem.Observer()
    pton.lat, pton.lon = '40.35065784387554', '-74.65177973757405'

    polaris = ephem.readdb("Polaris,f|M|F7,2:31:48.704,89:15:50.72,2.02,2000")

    polaris.compute(pton)
   
    print("%s %s" % (polaris.alt, polaris.az))

    polarisAltStr = padZeros(str(polaris.alt))
    polarisAzStr = padZeros(str(polaris.az))

    #Setup the serial port for communication with Arduino
    ser = serial.Serial('COM3', 115200, timeout=20)
    outString = polarisAltStr + " " + polarisAzStr + "\n"

    sleep(5)

    #Write to the serial port
    ser.write(outString.encode(encoding="ascii"))
    print(outString)

    # ISS Loop
    # while(True):
    #     tNow = ts.now()
    #     difference = ISS - princeton
    #     topocentric = difference.at(tNow)
    #     alt, az, distance = topocentric.altaz()
       
    #     # print(alt)
    #     padAlt = padSat(str(alt))
    #     padAz = padSat(str(az))

    #     padZeroAlt = padZeros(padAlt.replace("-", ""))
    #     padZeroAz = padZeros(padAz)

    #     print("Alt: " + padZeroAlt + "   Az:  " + padZeroAz)

    #     outString = padZeroAlt + " " + padZeroAz + "\n"


    #     ser.write(outString.encode(encoding="ascii"))

    #     sleep(0.1)


# Moon/Sun Loop
    while(True):
        pton = ephem.Observer()
        pton.lat, pton.lon = '40.35065784387554', '-74.65177973757405'
        moon = ephem.Moon()
        # moon = ephem.star("Dubhe")
        moon.compute(pton)

        moonAltStr = padZeros(str(moon.alt))
        moonAzStr = padZeros(str(moon.az))
       
        outString = moonAltStr + " " + moonAzStr + "\n"

        print("Writing moon alt/az: " + outString)


        ser.write(outString.encode(encoding="ascii"))

        sleep(0.1)

if __name__ == "__main__":
    main()
