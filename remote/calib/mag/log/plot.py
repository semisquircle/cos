import numpy as np
import matplotlib.pyplot as plt


# Define calibration parameters
A = np.array([
	[1.936717, -0.008358, -0.024280],
	[0.008358, 1.821100, 0.027199],
	[-0.024280, 0.027199, 1.662221]
])
b = np.array([-24.305482, 40.410017, -38.012254])


# Read raw data and apply calibration
rawData = np.genfromtxt('magdata.txt', delimiter='\t')  # Read raw measurements

N = len(rawData)
calibData = np.zeros((N, 3), dtype='float')
for i in range(N):
	currMeas = np.array([rawData[i, 0], rawData[i, 1], rawData[i, 2]])
	calibData[i, :] = A @ (currMeas - b)


# Plot XY data
plt.figure()
plt.plot(rawData[:, 0], rawData[:, 1], 'b*', label='Raw Meas.')
plt.plot(calibData[:, 0], calibData[:, 1], 'r*', label='Calibrated Meas.')
plt.title('XY Magnetometer Data')
plt.xlabel('X [uT]')
plt.ylabel('Y [uT]')
plt.legend()
plt.grid()
plt.axis('equal')

# Plot YZ data
plt.figure()
plt.plot(rawData[:, 1], rawData[:, 2], 'b*', label='Raw Meas.')
plt.plot(calibData[:, 1], calibData[:, 2], 'r*', label='Calibrated Meas.')
plt.title('YZ Magnetometer Data')
plt.xlabel('Y [uT]')
plt.ylabel('Z [uT]')
plt.legend()
plt.grid()
plt.axis('equal')

# Plot XZ data
plt.figure()
plt.plot(rawData[:, 0], rawData[:, 2], 'b*', label='Raw Meas.')
plt.plot(calibData[:, 0], calibData[:, 2], 'r*', label='Calibrated Meas.')
plt.title('XZ Magnetometer Data')
plt.xlabel('X [uT]')
plt.ylabel('Z [uT]')
plt.legend()
plt.grid()
plt.axis('equal')


# Plot 3D scatter
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

for i in range(N):
	xraw = rawData[i, 0]
	yraw = rawData[i, 1]
	zraw = rawData[i, 2]

	xcalib = calibData[i, 0]
	ycalib = calibData[i, 1]
	zcalib = calibData[i, 2]
	ax.scatter(xraw, yraw, zraw, color='r')
	ax.scatter(xcalib, ycalib, zcalib, color='b')

ax.set_title('3D Scatter Plot of Magnetometer Data')
ax.set_xlabel('X [uT]')
ax.set_ylabel('Y [uT]')
ax.set_zlabel('Z [uT]')


plt.show()
