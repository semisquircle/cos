// Motion events
var isCalibrated = false;
var isCalibrating = false;
var justCalibrated = false;
var p, y, r, po, yo, ro;
var pd = 3;
var yd = 8;

function calibrate() {
	isCalibrated = true;
	isCalibrating = false;
	justCalibrated = true;

	po = p;
	yo = y;
	ro = r;

	console.log("Calibrated!");
}

function changeSquircle() {
	$(".squircle").attr("data-bump-dir", "");
	$(".squircle").removeClass("current-squircle");
	$(`[data-row="${currentSquircle.row}"] [data-col="${currentSquircle.col}"]`).addClass("current-squircle");
}

listen("arduino-update", function(data) {
	let obj = JSON.parse(data.payload);
	p = -obj.pitch;
	y = -obj.yaw;
	r = obj.roll;

	if (isCalibrated) {
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



// Key events
var Buttons = {
	dpad: {
		left:  "ArrowLeft",
		up:    "ArrowUp",
		right: "ArrowRight",
		down:  "ArrowDown"
	},
	center:    "Space",
	back:      "KeyQ",

	stack: [],

	downStart: 0,
	downTime: 0,
	calibrateTime: 700,

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

function type(char) {
	let text = $("#keyboard-input-text").text();
	$("#keyboard-input-text").text(text + char);
}

$(document).on("keydown", async function(e) {
	if (Object.values(Buttons.dpad).includes(e.code)) {
		let dir = Buttons.dir(e.code);
		if (!Buttons.isPressing(dir)) {
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
						if (quadEl.children().length > 1)
							char = quadEl.find(`.squircle-char[data-case="${currentCase}"]`).attr("data-char");
						else
							char = quadEl.find(".squircle-char").attr("data-char");

						Buttons.add(dir);
						type(char);
						break;
				}

				$(".current-squircle").attr("data-bump-dir", dir);
			}
		}
	}

	else if (e.code == Buttons.center) {
		if (!justCalibrated) {
			if (!isCalibrating) {
				isCalibrating = true;
				Buttons.downStart = Date.now();
			} else {
				Buttons.downTime = Date.now() - Buttons.downStart;
				if (Buttons.downTime >= Buttons.calibrateTime) {
					calibrate();
					if (currentScreen == "calib") changeScreen("keyboard");
				}
			}
		}
	}
})
.on("keyup", function(e) {
	if (Object.values(Buttons.dpad).includes(e.code)) {
		let dir = Buttons.dir(e.code);
		Buttons.remove(dir);
		$(".current-squircle").attr("data-bump-dir", Buttons.top());
	}

	else if (e.code == Buttons.center) {
		if (!justCalibrated) toggleCase();
		isCalibrating = false;
		justCalibrated = false;
	}
});
