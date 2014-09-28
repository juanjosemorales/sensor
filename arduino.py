import serial
import MySQLdb

arduino = serial.Serial('/dev/ttyACM0', 9600)

db = MySQLdb.connect(host="192.168.0.5", port=3306,
		     user="root", passwd="root", db="arduino")
cursor = db.cursor()

while 1 : 
	val = arduino.readline()
	val = str(val).rstrip()
	print("The value is {} ".format(val))
	sql = "INSERT INTO sensor_info_test1 (data) VALUES ('"+val+"')"
	print(sql)
	try:
		cursor.execute(sql)
		db.commit()
	except:
		db.rollback()
		
	
	
