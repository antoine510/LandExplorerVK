blocAtlas = "textures/blocAtlas.png"
backwallAtlas = "textures/backwallAtlas.png"

-- Sprites --
breakingBloc = {
	path = "textures/breakBloc.png",
	clipSize = { w = 16, h = 16 }
}
playerHearth = {
	path = "textures/heart.png",
	screenOrigin = { x = 1, y = 0 }
}
itemSlot = {
	path = "textures/itemSlot.png"
}
itemAtlas = {
	path = "textures/itemAtlas.png",
	clipSize = { w = 32, h = 32 }
}

local surfaceBG = function(bgPath)
	return {
		path = bgPath,
		clipSize = { w = TERRAIN_WIDTH * 8, h = -1 },
		scale = 2.5,
		linearSampling = true
	}
end

backgroundRenderer = {
	sun = {
		path = "textures/background/sun.png"
	},
	backgrounds = {
		plain = surfaceBG("textures/background/bgPlain.png"),
		mountain = surfaceBG("textures/background/bgMountain.png"),
		ocean = surfaceBG("textures/background/bgOcean.png"),
		underground = {
			path = "textures/background/bgUnderground.png",
			fullscreen = true
		},
		menu = {
			path = "textures/background/bgMenu.png",
			fullscreen = true
		},
		generation = {
			path = "textures/background/backgroundGen.png",
			fullscreen = true
		},
		win = {
			path = "textures/background/backgroundWin.png",
			fullscreen = true
		}
	}
}

menuRenderer = {
	outline = {
		path = "textures/menu/outline.png",
		position = { x = 10, y = 10 }
	}
}

texSets = {
	[1] = {
		mapIcon = {
			path = "textures/player/playerMapIcon.png",
			origin = { x = 0.5, y = 0.5 }
		},
		states = {
			{ path = "textures/player/player.png" },
			{ path = "textures/player/tomb.png" }
		},
		animations = {
			{
				path = "textures/player/playerWalk.png",
				frameWidth = 30,
				delay = 100,
				loop = true
			},
			{
				path = "textures/player/playerFreefallLeft.png",
				frameWidth = 30,
				delay = 200,
				loop = true
			},
			{
				path = "textures/player/playerFreefallRight.png",
				frameWidth = 30,
				delay = 200,
				loop = true
			}
		}
	},
	[2] = {
		mapIcon = {
			path = "textures/sheep/sheepMapIcon.png",
			origin = { x = 0.5, y = 0.5 }
		},
		states = {
			{ path = "textures/sheep/sheepRight.png" },
			{ path = "textures/sheep/sheepLeft.png" }
		}
	},
	[3] = {
		mapIcon = {
			path = "textures/archer/mapIcon.png",
			origin = { x = 0.5, y = 0.5 }
		},
		states = {
			{ path = "textures/archer/archer.png" }
		}
	},
	[4] = {
		states = {
			{ path = "textures/arrow/arrow.png" }
		}
	},
	[5] = {
		states = {
			{ path = "textures/sword/default.png" }
		}
	},
	[6] = {
		states = {
			{ path = "textures/boat/boat.png" }
		}
	},
	[7] = {
		states = {
			{ path = "textures/guard/guard.png" }
		}
	}
}
