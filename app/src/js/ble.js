// Motion events
var isRecentered = false;
var isRecentering = false;
var justRecentered = false;

var remote;
var yaw, pitch, roll;
var yawOffset, pitchOffset, rollOffset;
var yawDelta = 8;
var pitchDelta = 8;
var rollDelta = 120;



function recenter() {
	isRecentered = true;
	isRecentering = false;
	justRecentered = true;

	pitchOffset = pitch - pitchDelta;
	yawOffset = yaw;
	rollOffset = roll;

	console.log("Recentered!");
}

var Buttons = {
	stack: [],

	downStart: 0,
	downTime: 0,
	recenterTimer: null,
	recenterTime: 700,

	add: function(el) {
		this.stack.unshift(el);
	},
	remove: function(key) {
		let index = this.stack.indexOf(key);
		if (index > -1) this.stack.splice(index, 1);
	},
	top: function() {
		if (this.stack.length > 0) return this.stack[0];
		return "";
	},
	isPressing: function(key) {
		return this.stack.includes(key);
	},
	dir: function(code) {
		return Object.keys(this.dpad).find(dir => this.dpad[dir] === code)
	}
}



listen("arduino-update", function(data) {
	console.log(data.payload);
	/* let newRemote = JSON.parse(data.payload);
	if (remote === undefined) remote = structuredClone(newRemote);


	//* IMU
	yaw = newRemote.imu.yaw;
	pitch = newRemote.imu.pitch;
	roll = newRemote.imu.roll;

	if (isRecentered) {
		// Recenter angles based on body frame
		let recenteredYaw = ((yaw - yawOffset + 180) % 360 + 360) % 360 - 180;
		let recenteredPitch = Math.max(-90, Math.min(90, pitch - pitchOffset));
		let recenteredRoll = ((roll - rollOffset + 180) % 360 + 360) % 360 - 180;

		// Calculate relative row
		let numCols = currentKeyboard[currentSquircle.row].length;
		let colRaw = Math.round(recenteredYaw / yawDelta + (numCols / 2));
		let newCol = Math.max(0, Math.min(colRaw, numCols - 1));

		// Calculate relative column
		let numRows = currentKeyboard.length;
		let rowRaw = Math.round(recenteredPitch / pitchDelta + (numRows / 2));
		let newRow = Math.max(0, Math.min(rowRaw, numRows - 1));
	
		// Calculate and apply character/accent index
		let newCharIndex = Math.round(recenteredRoll / rollDelta);
		changeCharIndex(newCharIndex);

		if (
			(newRow !== currentSquircle.row || newCol !== currentSquircle.col) &&
			(newCharIndex == 0)
		) {
			currentSquircle.row = newRow;
			currentSquircle.col = newCol;
			changeSquircle();
		}
	}


	//* Dpad buttons
	for (let d = 0; d < dirNames.length; d++) {
		let dir = dirNames[d];
		if (remote.btns.dpad[dir] !== newRemote.btns.dpad[dir]) {
			// On dpad button down
			if (newRemote.btns.dpad[dir] == true) {
				let quadEl = $(`.current-squircle .squircle-quad[data-dir="${dir}"]`);
				if (quadEl.length) {
					let ctrl = quadEl.attr("data-ctrl");
					switch (ctrl) {
						case "space":
							type("\u00A0");
							break;
	
						case "backspace":
							$("#keyboard-input-text").text((_, txt) => txt.slice(0, -1));
							break;
	
						default:
							let char = "";
							if (quadEl.children().length > 1) {
								char = quadEl.find(`.squircle-char[data-case="${currentCase}"]`).attr("data-char");
							} else {
								char = quadEl.find(".squircle-char").attr("data-char");
							}
							Buttons.add(dir);
							type(char);
							break;
					}
	
					$(".current-squircle").attr("data-bump-dir", dir);
				}
			}
			
			// On dpad button up
			else {
				Buttons.remove(dir);
				$(".current-squircle").attr("data-bump-dir", Buttons.top());
			}
		}
	}


	//* Select button
	if (remote.btns.select !== newRemote.btns.select) {
		// On select button down
		if (newRemote.btns.select == true) {
			Buttons.downStart = Date.now();
			Buttons.recenterTimer = setInterval(function() {
				Buttons.downTime = Date.now() - Buttons.downStart;
				if (Buttons.downTime > Buttons.recenterTime) {
					clearInterval(Buttons.recenterTimer);
					recenter();
					if (currentScreen == "calib") changeScreen("keyboard");
				}
			}, 1);
		}

		// On select button up
		else {
			if (!justRecentered) toggleCase();
			
			clearInterval(Buttons.recenterTimer);
			isRecentering = false;
			justRecentered = false;
		}
	}


	remote = structuredClone(newRemote); */
});

$(document).on("keydown", function(e) {
	if (e.code == "Space") {
		recenter();
		if (currentScreen == "calib") changeScreen("keyboard");
	}
});