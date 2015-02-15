import serial
import datetime
import requests


url = "http://localsprout.pythonanywhere.com/monitoring/get_readings"
i = 0
while i == 0:
	date = str(datetime.datetime.now())	
	try:
		arduino = serial.Serial('/dev/ttyACM0', 9600)
		arduino_reading = str(arduino.readline()).strip()	
		value = arduino_reading	
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
		reading = { "type":"fatal_error", "date":date, "reading":str(e) }
		r = requests.post(url, data=reading) 
		print "Unexpected error: " + str(e)
