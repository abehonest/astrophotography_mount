import serial
from time import sleep

import cv2
import numpy as np


iWTotal = 0
jWTotal = 0

iTotal = 0
jTotal = 0

#PI constants
K_Pi = 0
K_Ii = 0
K_Pj = 0
K_Ij = 0

#List to store terms for the integral amount
offsetsSize = 5
offsetsI = [0] * offsetsSize
offsetsJ = [0] * offsetsSize

#Format output to be readable by Arduino
def padZeros(padStr):
    padStrSplit = padStr.split(" ")
   
    if(len(padStrSplit[0]) == 1): padStrSplit[0] = "0000" + padStrSplit[0]
    elif(len(padStrSplit[0]) == 2): padStrSplit[0] = "000" + padStrSplit[0]
    elif(len(padStrSplit[0]) == 3): padStrSplit[0] = "00" + padStrSplit[0]
    elif(len(padStrSplit[0]) == 4): padStrSplit[0] = "0" + padStrSplit[0]

    if(len(padStrSplit[1]) == 1): padStrSplit[1] = "0000" + padStrSplit[1]
    elif(len(padStrSplit[1]) == 2): padStrSplit[1] = "000" + padStrSplit[1]
    elif(len(padStrSplit[1]) == 3): padStrSplit[1] = "00" + padStrSplit[1]
    elif(len(padStrSplit[1]) == 4): padStrSplit[1] = "0" + padStrSplit[1]

    output = padStrSplit[0] + " " + padStrSplit[1]
    return output

def main():
    ts = load.timescale()

    ser = serial.Serial('COM3', 115200, timeout=20)
   
    sleep(5)

    # tracking loop
    while(True):
        #Read current frame using the cv2 library
        #Returns a 2D Array with light intensities for each pixel
        currentFrame = cv2.imread('',0)

        #Assign frame centers
        frameCenterI = len(currentFrame) / 2
        frameCenterJ = len(currentFrame[0]) / 2

        #Calculate the center of mass of light
        for i in range(len(currentFrame)):
            for j in range(len(currentFrame[i])):
                iWTotal = iWTotal + ((i+1) * currentFrame[i][j])
                jWTotal = jWTotal + ((j+1) * currentFrame[i][j])
                iTotal = iTotal + currentFrame[i][j]
                jTotal = jTotal + currentFrame[i][j]

        lightCenterI = iWTotal / iTotal
        lightCenterJ = jWTotal / jTotal

        #Reset variables
        iWTotal = 0
        jWTotal = 0

        iTotal = 0
        jTotal = 0

        #Calculate the displacement of the center of light from the center of frame
        currentOffsetI = frameCenterI - lightcenterI
        currentOffsetJ = frameCenterJ - lightcenterJ

        #Store current offset in array of recent offsets for integral term
        offsetsI[count] = currentOffsetI
        offsetsJ[count] = currentOffsetJ

        #calculate the amount
        Alt = sum(offsetsI) * K_Ii + currentOffsetI * K_Pi
        Az = sum(offsetsJ) * K_Ij + currentOffsetJ * K_Pj

        count = count + 1

        #reset index in offsets list
        if(count = offsetsSize):
            count = 0

        #Write movement amounts to the Serial port for the Arduino
        outString = padZeros(Alt + " " + Az + "\n")
        ser.write(outString.encode(encoding="ascii"))

        sleep(0.1)



if __name__ == "__main__":
    main()
