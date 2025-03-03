import os
import math
import pandas
import serial


MAX_MEAS = 200
AVG_MEAS = 25
SER_PORT = "/dev/cu.usbmodem142301"
SER_BAUD = 115200
FILENAME = os.path.join(os.getcwd(), 'gyrodata.txt')


class SerialPort:
	def __init__(self, port, baud=115200):
		if isinstance(port, str) == False:
			raise TypeError('port must be a string.')

		if isinstance(baud, int) == False:
			raise TypeError('Baud rate must be an integer.')

		self.port = port
		self.baud = baud

		self.ser = serial.Serial(self.port, self.baud, timeout=None, xonxoff=False, rtscts=False, dsrdtr=False)
		self.ser.reset_input_buffer()
		self.ser.reset_output_buffer()

	def Read(self, clean_end=True) -> str:
		self.ser.reset_input_buffer()
		bytesToRead = self.ser.readline()
		decodedMsg = bytesToRead.decode('utf-8')

		if clean_end == True:
			decodedMsg = decodedMsg.rstrip()

		return decodedMsg

	def Close(self) -> None:
		self.ser.close()


def RecordDataPt(ser: SerialPort) -> tuple:
	ax = ay = az = 0.0

	actualSamples = 0;
	while actualSamples < AVG_MEAS:
		data = ser.Read().split(',')
		if len(data) == 3 and all(len(axis) >= 8 and '.' in axis for axis in data):
			print(str(actualSamples) + "\t" + str(data))
			ax += float(data[0])
			ay += float(data[1])
			az += float(data[2])
			actualSamples += 1

	return (ax / actualSamples, ay / actualSamples, az / actualSamples)


def List2DelimFile(mylist: list, filename: str, delimiter: str = ',', f_mode='a') -> None:
	df = pandas.DataFrame(mylist)
	df.to_csv(
		filename,
		sep=delimiter,
		mode=f_mode,
		header=False,
		index=False
	)


def main():
	ser = SerialPort(SER_PORT, baud=SER_BAUD)
	data = []

	print('[INFO]: Place sensor level and stationary on desk.')
	input('[INPUT]: Press any key to continue...')

	for _ in range(MAX_MEAS):
		user = input('[INPUT]: Ready for measurement? Type \'m\' to measure or \'q\' to save and quit: ').lower()
		if user == 'm':
			ax, ay, az = RecordDataPt(ser)
			magn = math.sqrt(ax**2 + ay**2 + az**2)
			print('[INFO]: Avgd Readings: {:.4f}, {:.4f}, {:.4f} Magnitude: {:.4f}'.format(ax, ay, az, magn))
			data.append([ax, ay, az])
		elif user == 'q':
			print('[INFO]: Saving data and exiting...')
			List2DelimFile(data, FILENAME, delimiter='\t')
			ser.Close()
			print('[INFO]: Done!')
			return
		else:
			print('[ERROR]: \'{}\' is an unknown input. Terminating!'.format(user))
			List2DelimFile(data, FILENAME, delimiter='\t')
			ser.Close()
			return

	print('[WARNING]: Reached max. number of datapoints, saving file...')
	List2DelimFile(data, FILENAME, delimiter='\t')
	ser.Close()
	print('[INFO]: Done!')


if __name__ == '__main__':
	main()
