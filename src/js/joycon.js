const AHRS = require("ahrs");

const ahrs = new AHRS({
	algorithm: "Madgwick"
});

String.prototype.between = function(str1, str2) {
	return this.split(str1).pop().split(str2)[0];
}

listen("joycon-update", async function(data) {
	let result = data.payload;
	let imuJson = result.between("IMUData { data: [", "]");
		imuJson = "[" + imuJson.replaceAll("AxisData ", "") + "]";
		imuJson = imuJson.replace(/(['"])?([a-z0-9A-Z_]+)(['"])?:/g, '"$2": ');
	let imu = JSON.parse(imuJson);

	let gx = imu[0].gyro_1;
	let gy = imu[0].gyro_2;
	let gz = imu[0].gyro_3;

	let ax = imu[0].accel_x;
	let ay = imu[0].accel_y;
	let az = imu[0].accel_z;

	ahrs.update(gx, gy, gz, ax, ay, az);
	let filter = ahrs.getEulerAngles();
	[p, y, r] = [filter.pitch, filter.heading, filter.roll];

	if (isCalibrated) {
		console.log(p, y);

		let numRows = currentKeyboard.length;
		let rowRaw = Math.round((p - po) / pd + (numRows / 2));
		let newRow = Math.max(0, Math.min(rowRaw, numRows - 1));
	
		let numCols = currentKeyboard[currentSquircle.row].length;
		let colRaw = Math.round((y - yo) / yd + (numCols / 2));
		let newCol = Math.max(0, Math.min(colRaw, numCols - 1));
	
		if (newRow !== currentSquircle.row || newCol !== currentSquircle.col) {
			currentSquircle.row = newRow;
			currentSquircle.col = newCol;
			changeSquircle();
		}
	}
});



/* var foo = {
	common: {
		input_report_id: 33,
		timer: 47,
		battery: {
			level: Full,
			is_charging: false
		},
		connection_info: {
			device: JoyCon,
			is_powered: false
		},
		pushed_buttons: {
			right: [],
			shared: [],
			left: []
		},
		left_analog_stick_data: {
			horizontal: 0, vertical: 0
		},
		right_analog_stick_data: {
			horizontal: 2175,
			vertical: 1879
		},
		vibrator_input_report: 12
	},
	extra: {
		data: [
			{
				accel_x: 18560,
				accel_y: 0,
				accel_z: 0,
				gyro_1: 0,
				gyro_2: 0,
				gyro_3: 0
			},
			{
				accel_x: 0,
				accel_y: 0,
				accel_z: 0,
				gyro_1: 0,
				gyro_2: 0,
				gyro_3: 0
			},
			{
				accel_x: 0,
				accel_y: 0,
				accel_z: 0,
				gyro_1: 0,
				gyro_2: 0,
				gyro_3: 0
			}
		]
	}
} */