// Languages
function generateLangOptions() {
	for (let o = 0; o < keyboards.length; o++) {
		let keyboard = keyboards[o];
		let iso = keyboard.iso;
		let exonym = keyboard.exonym;
		let btn = squircleBtn.clone();

		btn.attr("data-iso", iso);
		btn.find(".squircle-btn-label").html(exonym);
		$("#lang-option-container").append(btn);
	}
}

$("#lang-option-container").on("click", ".squircle-btn", function() {
	currentLang = $(this).attr("data-iso");
	currentLangObject = keyboards.find(obj => obj.iso === currentLang);
	let font = currentLangObject.font;

	generateOrientOptions();
	$("#keyboard-screen").attr("data-current-iso", currentLang);
	$("#keyboard-screen").css("font-family", font);
	changeScreen("orient");
});



// Orientation
function generateOrientOptions() {
	for (let o = 0; o < currentLangObject.orients.length; o++) {
		let orient = currentLangObject.orients[o];
		let abbr = orient.abbr;
		let name = orient.name;
		let btn = squircleBtn.clone();

		btn.attr("data-orient", abbr);
		btn.find(".squircle-btn-label").html(name);
		$("#orient-option-container").append(btn);
	}
}

$("#orient-option-container").on("click", ".squircle-btn", function() {
	currentOrient = $(this).attr("data-orient");
	currentOrientObject = currentLangObject.orients.find(obj => obj.abbr === currentOrient);
	currentKeyboard = currentOrientObject.chars;
	let btn = squircleBtn.clone();

	btn.find(".squircle-btn-label").html("Listen");
	$("#port-input-container").append(btn);
	$("#keyboard-screen").attr("data-current-orient", currentOrient);
	changeScreen("port");
	$("#port-input").val(currentPort).focus();
});



// Port
$("#port-input-container").on("click", ".squircle-btn", function() {
	currentPort = $("#port-input").val();

	invoke("open_serial_port", {
		port_name: currentPort,
		baud_rate: 115200
	});
	// invoke("read_joycon");

	generateKeyboard();
	changeScreen("calib");
});
