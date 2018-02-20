import serial

led_offset=10
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0.5)

# https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
# answer from "amro"
def getColour(v, vmin, vmax):
    c = [1.0,1.0,1.0]

    if v < vmin:
        v = vmin
    if v > vmax:
        v = vmax
    dv = vmax - vmin

    if v < (vmin + 0.25 * dv):
        c[0] = 0
        c[1] = 4 * (v - vmin) / dv
    elif v < (vmin + 0.5 * dv):
        c[0] = 0
        c[2] = 1 + 4 * (vmin + 0.25 * dv - v) / dv
    elif v < (vmin + 0.75 * dv):
        c[0] = 4 * (v - vmin - 0.5 * dv) / dv
        c[2] = 0
    else:
        c[1] = 1 + 4 * (vmin + 0.75 * dv - v) / dv
        c[2] = 0
    return c


nLeds = 50
offset = 0

while True:
	for i in range(0,nLeds/2):
		c = getColour(float(i), float(0), float(nLeds)/2.0)
		cmd = 'LEDF %d %.2f %.2f %.2f\r\n' % ( ((i+offset) % nLeds), c[0], c[1], c[2] )
#		print(cmd)
		ser.write(cmd)

	for i in range(0,nLeds/2):
		c = getColour(float(i), float(0), float(nLeds)/2.0)
		cmd = 'LEDF %d %.2f %.2f %.2f\r\n' % ( ((-i+offset+nLeds) % nLeds), c[0], c[1], c[2] )
#		print(cmd)
		ser.write(cmd)
        
	offset = offset + 1
	#raw_input('Press enter to continue: ')
	
ser.close()
