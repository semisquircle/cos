import fontforge
import os
import shutil
import re
import psMat

LANGS = {
	"eng": {
		"font": "/Users/Shawn/Desktop/code/arduino/src/font/NotoSans/ttf/NotoSans-Light.ttf",
		"ranges": [range(0x0041, 0x005B), range(0x0061, 0x007B)],
		"misc": "!.,?@()$#[]*+-"
	},
	"gre": {
		"font": "/Users/Shawn/Desktop/code/arduino/src/font/NotoSans/ttf/NotoSans-Light.ttf",
		"ranges": [range(0x03b1, 0x03c9), range(0x0391, 0x03a9)],
		"misc": "!.,?@()€#[]*+-"
	},
	"heb": {
		"font": "/Users/Shawn/Desktop/code/arduino/src/font/NotoSansHebrew/ttf/NotoSansHebrew-ExtraLight.ttf",
		"ranges": [range(0x05D0, 0x05EB)],
		"misc": "!.,?@()₪#[]*+־"
	},
	"jpn": {
		"font": "/Users/Shawn/Desktop/code/arduino/src/font/NotoSansJP/ttf/NotoSansJP-ExtraLight.ttf",
		"ranges": [range(0x3041, 0x3096), range(0x30a1, 0x30fa)],
		"misc": "。、！？（）［］ー¥：〽@#※+-"
	}
}

def export(lang):
	output_dir = lang

	if os.path.exists(output_dir):
		shutil.rmtree(output_dir)
	os.makedirs(output_dir)

	chars = [cp for r in LANGS[lang]["ranges"] for cp in r] + [ord(c) for c in LANGS[lang]["misc"]] + [ord(c) for c in "0123456789"]
	font = fontforge.open(LANGS[lang]["font"])

	for char in chars:
		glyph = font[char]
		svg_path = os.path.join(output_dir, f"{char}.svg")
		
		glyph.left_side_bearing = 0
		glyph.right_side_bearing = 0

		glyph.width = 1000
		x_min, y_min, x_max, y_max = glyph.boundingBox()

		translationX = (1000 - (x_max - x_min)) / 2
		translationY = (font.ascent - font.descent - (y_min + y_max)) / 2
		glyph.transform(psMat.translate(translationX, translationY))

		glyph.export(svg_path)


		# Modify viewbox
		with open(svg_path, "r") as svg_file:
			svg_content = svg_file.read()

		start_string = 'viewBox="'
		end_string = '">'
		new_viewbox = f'viewBox="0 0 1000 1000">'
		svg_content = re.sub(f'{start_string}.*?{end_string}', new_viewbox, svg_content, flags=re.DOTALL)
		svg_content = re.sub('fill="currentColor"', "", svg_content, flags=re.DOTALL)

		with open(svg_path, "w") as svg_file:
			svg_file.write(svg_content)


		# Remove first two lines
		with open(svg_path, "r") as fin:
			data = fin.read().splitlines(True)
		with open(svg_path, "w") as fout:
			fout.writelines(data[2:])

	print(f"Exported {len(chars)} characters.")

export("jpn")