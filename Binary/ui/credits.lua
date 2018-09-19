
sizedText = function(t, s)
	return { text = t, size = s }
end

space = function(size) return { text = " ", size = size } end

creditLines = {
	sizedText("Credits", 80),
	space(50),
	sizedText("Developers", 40),
	{ text = "Antoine" },
	{ text = "Philippe" },
	space(nil),
	sizedText("Designers", 40),
	{ text = "Antoine" },
	{ text = "Philippe" }
}

endLine = sizedText("Thanks for playing !", 90)
