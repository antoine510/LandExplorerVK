
Button = function(name_, x, y, w, h)
	return { ["type"] = "button", name = name_, X = x, Y = y, W = w, H = h }
end

Slider = function(name_, v, min_, max_, x, y)
	return {	["type"] = "slider",
				name = name_,
				value = v,
				["min"] = min_,
				["max"] = max_,
				X = x,
				Y = y }
end

Label = function(text, x, y)
	return { ["type"] = "label", text = text, X = x, Y = y }
end

Walign = 0.0
Halign = 0.0

submenus = {
	[1] = {
		name = "Land Explorer",
		elements = {
			Button("Play", 100, 280, 200, 50),
			Button("Options", 100, 340, 200, 50),
			Button("Instructions", 100, 400, 200, 50),
			Button("Quit", 100, 460, 200, 50)
		}
	},
	[2] = {
		name = "Options",
		elements = {
			Button("Graphic options", 100, 280, 200, 50),
			Button("Audio options", 100, 340, 200, 50),
			Button("Key Bindings", 100, 400, 200, 50),
			Button("Back", 80, 470, 200, 50)
		}
	},
	[3] = {
		name = "Graphic options",
		elements = {
			Button("1920x1080", 100, 280, 200, 50),
			Button("1024x768", 100, 340, 200, 50),
			Button("800x600", 100, 400, 200, 50),
			Button("Toggle Fullscreen", 100, 460, 200, 50),
			Button("Back", 80, 530, 200, 50)
		}
	},
	[4] = {
		name = "Audio options",
		elements = {
			Slider("Master Volume", 100, 0, 100, 100, 280),
			Slider("Music Volume", 50, 0, 100, 100, 340),
			Button("Back", 80, 410, 200, 50),
		}
	},
	[5] = {
		name = "Key Bindings",
		elements = {
			Button("Quit", 100, 220, 200, 50),
			Button("Left", 100, 270, 200, 50),
			Button("Right", 100, 320, 200, 50),
			Button("Up", 100, 370, 200, 50),
			Button("Down", 100, 420, 200, 50),
			Button("Jump", 400, 220, 200, 50),
			Button("Select", 400, 270, 200, 50),
			Button("Map", 400, 320, 200, 50),
			Button("Editor", 400, 370, 200, 50),
			Button("Inventory", 400, 420, 200, 50),
			Button("Back", 80, 480, 200, 50)
		}
	},
	[6] = {
		name = "Instructions",
		elements = {
			Label("You wake up on the beach", 100, 280),
			Label("Options", 100, 340),
			Label("Instructions", 100, 400),
			Label("Instructions", 100, 400),
			Button("Back", 100, 460, 200, 50)
		}
	}
}