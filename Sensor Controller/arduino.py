import serial
import datetime
import requests


url = "http://localsprout.pythonanywhere.com/monitoring/get_readings"
i = 0
try:
	arduino = serial.Serial('/dev/ttyACM0', 9600)
	while i == 0:
		arduino_reading = str(arduino.readline()).strip()	
		value = arduino_reading	
		date = str(datetime.datetime.now())	
		reading_type = ""
		if "pH" in arduino_reading:
			reading_type = "pH"
		elif "EC" in arduino_reading:
			reading_type = "EC"
		else:
			reading_type = "Bad"	
		reading = { "type":reading_type, "date":date, "reading":value }
		r = requests.post(url, data=reading) 
		print(r.text)
except Exception, e:
	print "Unexpected error: " + str(e)
