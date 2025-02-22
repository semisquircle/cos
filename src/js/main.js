const invoke = window.__TAURI__.core.invoke;
const listen = window.__TAURI__.event.listen;



var keyboards;
var currentLang;
var currentLangObject;
var currentOrient;
var currentOrientObject;
var currentKeyboard;

var currentPort = "/dev/cu.usbmodem142301";

var currentCase = 0;
var totalCaseCount = 0;
var totalRows = 0;
var totalCols = 0;
var currentSquircle = {row: 0, col: 0};



function getSvg(path) {
	let foo;
	$.ajax({
		async: false,
		type: "GET",
		dataType: "xml",
		url: path,
		success: function(svgData) {
			foo = $(svgData).contents();
		}
	});
	return foo;
}

var squircleStraightEl = getSvg("img/squircle-straight.svg");
var squircleBtn = $(`
	<div class="squircle-btn">
		${squircleStraightEl[0].outerHTML}
		<div class="squircle-btn-label"></div>
	</div>
`);



var currentScreen = "";
function changeScreen(screen) {
	currentScreen = screen;
	$(".screen").removeClass("current-screen");
	setTimeout(function() {
		$(`#${screen}-screen`).addClass("current-screen");
	}, 500);
}


$(document).ready(async function() {
	invoke("close_serial_port");

	await fetch("keyboards.json").then(response => response.json()).then(data => {
		keyboards = data.sort((a, b) => a.exonym - b.exonym);
	});

	generateLangOptions();
});
