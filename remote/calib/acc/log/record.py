import serial
import time
import numpy as np

def get_samples(ser, label):
	input(f"Press Enter to start recording data for {label}...")
	samples = []
	while len(samples) < 30:
		line = ser.readline().decode('utf-8').strip()
		try:
			x, y, z = map(float, line.split(','))
			samples.append((x, y, z))
			print(f"Recorded: {x}, {y}, {z} ({len(samples)}/30)")
		except ValueError:
			print(f"Invalid data: {line}")
	return np.mean(samples, axis=0)

def main():
	ser = serial.Serial('/dev/cu.usbmodem142301', 115200, timeout=1)
	time.sleep(2)  # Allow time for connection to establish

	positions = [
		('X+', 'X-'),
		('Y+', 'Y-'),
		('Z+', 'Z-')
	]
	averages = {}
	
	for pos1, pos2 in positions:
		avg1 = get_samples(ser, pos1)
		avg2 = get_samples(ser, pos2)
		averages[pos1] = avg1
		averages[pos2] = avg2
	
	ser.close()
	
	with open("acceldata.txt", "w") as f:
		for pos, avg in averages.items():
			f.write(f"{pos}: {avg[0]}, {avg[1]}, {avg[2]}\n")
	
	r = 9.80665  # Assuming Earth's gravity (m/s^2) as reference magnitude
	for axis, (pos1, pos2) in zip(['X', 'Y', 'Z'], positions):
		u1, u2 = averages[pos1], averages[pos2]
		bias = (u1 + u2) / 2
		sensitivity = (np.abs(u1) + np.abs(u2)) / (2 * r)
		print(f"{axis} Bias: {bias}")
		print(f"{axis} Sensitivity: {sensitivity}")

if __name__ == "__main__":
	main()
