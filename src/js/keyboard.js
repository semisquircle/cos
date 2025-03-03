// Generate keyboard
const ctrlKeys = {
	"\u2423": {
		name: "space"
	},
	"\u232B": {
		name: "backspace"
	}
}

function generateKeyboard() {
	totalRows = currentKeyboard.length;

	for (let r = 0; r < currentKeyboard.length; r++) {
		let row = currentKeyboard[r];
		let rowEl = $(`<div class="squircle-row" data-row="${r}">`);

		if (row.length > totalCols) totalCols = row.length;

		for (let s = 0; s < row.length; s++) {
			let squircle = row[s];
			let squircleEl = getSvg("img/squircles/systems/straight.svg");
			squircleEl.attr("data-col", s);
			squircleEl.attr("data-current-char-index", "0");

			for (let d = 0; d < squircle.length; d++) {
				let dir = squircle[d];
				let dpadDir = dirNames[d];
				let quadEl = $(`<div class="squircle-quad" data-dir="${dpadDir}"></div>`);

				for (let c = 0; c < dir.length; c++) {
					let cayse = dir[c];
					let caseCount = dir.length;
					if (caseCount > totalCaseCount) totalCaseCount = caseCount;

					for (let ch = 0; ch < cayse.length; ch++) {
						let char = cayse[ch];
						let svg = "";
						let ctrl = ctrlKeys[char];
	
						if (char !== " ") {
							if (ctrl !== undefined) {
								quadEl.attr("data-ctrl", ctrl.name);
								svg = getSvg(`img/ctrl/${ctrl.name}.svg`);
							} else {
								svg = getSvg(`img/glyphs/${currentLang}/${char.charCodeAt(0)}.svg`);
								svg.addClass("squircle-char");
								if (caseCount > 1) svg.attr("data-case", c);
								svg.attr("data-char-index", ch);
								svg.attr("data-char", char);
							}
						}
	
						quadEl.append(svg);
					}
				}

				squircleEl.find(".squircle-front-container").append(quadEl);
			}

			rowEl.append(squircleEl);
		}

		$("#keyboard").append(rowEl);
	}
}

function toggleCase() {
	currentCase = (currentCase + 1) % totalCaseCount;
	$("#keyboard-screen").attr("data-current-case", currentCase);
}