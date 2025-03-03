#include <Arduino_BMI270_BMM150.h>
#include <NewMadgwick.h>

using namespace std;
#include <vector>

float gx, gy, gz;
float ax, ay, az;
float mx, my, mz;
float gxOffset = 0, gyOffset = 0, gzOffset = 0;
float q1, q2, q3, q4;
float velX, velY, velZ;
float posX, posY, posZ;

NewMadgwick filter;

// Acceleration of gravity vector in m/s^2 (actually a 1D matrix for convenience)
vector<vector<float>> g = {
	{0},
	{0},
	{9.80665}
};

// Integration timing
unsigned long previousTime = 0;
unsigned long currentTime;
float dt;


void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}

	filter.setSamplingRate(IMU.gyroscopeSampleRate());
	filter.setGyroMeasError(0.010 * DEG_TO_RAD);
	filter.setGyroDriftError(0.02 * DEG_TO_RAD);

	calibrateGyro();
}


void calibrateGyro() {
	Serial.println("Calibrating gyroscope...");

	const int numSamples = 3000;
	float sumX = 0, sumY = 0, sumZ = 0;

	int actualSample = 0;
	for (int i = 0; i < numSamples; i++) {
		IMU.readGyroscope(gx, gy, gz);
		sumX += gx;
		sumY += gy;
		sumZ += gz;
		actualSample++;
	}

	gxOffset = sumX / actualSample;
	gyOffset = sumY / actualSample;
	gzOffset = sumZ / actualSample;

	Serial.println("Finished calibration.");
	Serial.println();
}


// Matrix multiplication
vector<vector<float>> multiplyMat(vector<vector<float>> &mat1, vector<vector<float>> &mat2) {
	int rows1 = mat1.size();
	int cols1 = mat1[0].size();
	int rows2 = mat2.size();
	int cols2 = mat2[0].size();

	// Initialize the result matrix with appropriate dimensions
	vector<vector<float>> result(rows1, vector<float>(cols2, 0));

	// Perform matrix multiplication
	for (int i = 0; i < rows1; ++i) {
		for (int j = 0; j < cols2; ++j) {
			for (int k = 0; k < cols1; ++k) {
				result[i][j] += mat1[i][k] * mat2[k][j];
			}
		}
	}

	return result;
}

// Matrix addition
vector<vector<float>> addMat(vector<vector<float>> &mat1, vector<vector<float>> &mat2) {
	int rows = mat1.size();
	int cols = mat1[0].size();

	vector<vector<float>> result(rows, vector<float>(cols, 0));

	// Add corresponding elements
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result[i][j] = mat1[i][j] + mat2[i][j];
		}
	}

	return result;
}

// Matrix transpose
vector<vector<float>> transposeMat(vector<vector<float>> &mat) {
	int rows = mat.size();
	int cols = mat[0].size();
	
	// Initialize the result matrix with transposed dimensions
	vector<vector<float>> result(cols, vector<float>(rows, 0));

	// Perform the transposition
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result[j][i] = mat[i][j];
		}
	}

	return result;
}


void loop() {
	currentTime = millis();
	dt = (currentTime - previousTime) / 1000.0;
	previousTime = currentTime;

	if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
		IMU.readGyroscope(gx, gy, gz);
		IMU.readAcceleration(ax, ay, az);

		// Apply calibration
		gx -= gxOffset;
		gy -= gyOffset;
		gz -= gzOffset;

		// Degrees to radians
		gx *= DEG_TO_RAD;
		gy *= DEG_TO_RAD;
		gz *= DEG_TO_RAD;

		// Madgwick filter (using MARG)
		filter.updateIMU(gx, gy, gz, ax, ay, az);
		filter.getQuaternions(q1, q2, q3, q4);

		// Accelerometer data vector in m/s^2 (actually a 1D matrix for convenience)
		vector<vector<float>> a_m = {
			{ax * 9.80665},
			{ay * 9.80665},
			{az * 9.80665}
		};

		// Rotation matrix based on Madgwick quaternions
		vector<vector<float>> R = {
			{2 * pow(q1, 2) - 1 + 2 * pow(q2, 2), 2 * (q2 * q3 + q1 * q4), 2 * (q2 * q4 - q1 * q3)},
			{2 * (q2 * q3 - q1 * q4), 2 * pow(q1, 2) - 1 + 2 * pow(q3, 2), 2 * (q3 * q4 - q1 * q2)},
			{2 * (q2 * q4 - q1 * q3), 2 * (q3 * q4 - q1 * q2), 2 * pow(q1, 2) - 1 + 2 * pow(q4, 2)}
		};

		// Calculating inertial frame acceleration
		vector<vector<float>> RT = transposeMat(R);
		vector<vector<float>> RT_times_a = multiplyMat(RT, a_m);
		vector<vector<float>> a_i = addMat(RT_times_a, g);

		// Integrate for velocity (m/s)
		velX += a_i[0][0] * dt;
		velY += a_i[1][0] * dt;
		velZ += a_i[2][0] * dt;

		// Integrate again for position (m)
		posX += velX * dt;
		posY += velY * dt;
		posZ += velZ * dt;

		Serial.print(a_i[0][0]);
		Serial.print(",");
		Serial.print(a_i[1][0]);
		Serial.print(",");
		Serial.println(a_i[2][0]);
	}
}