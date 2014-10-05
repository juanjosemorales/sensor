import serial
import MySQLdb
from datetime import datetime

arduino = serial.Serial('/dev/ttyACM0', 9600)

db = MySQLdb.connect(host="192.168.0.5", port=3306,
		     user="root", passwd="root", db="arduino")
cursor = db.cursor()

while 1 : 
	val = arduino.readline()
	val = str(val).rstrip()
	sensor="PH"
	time = str(datetime.now())
	print("The value is {} ".format(val))
	sql = "INSERT INTO monitoring_sensor (sensor_type, sensor_data, date) VALUES (%s, %s, %s)"
	params = sensor, val, time
	print(sql, params)
	try:
		cursor.execute(sql, params)
		db.commit()
	except:
		db.rollback()
		
	
	
