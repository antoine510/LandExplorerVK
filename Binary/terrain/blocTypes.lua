terrainBloc = function(lightOpa, offset)
	return {
		lightOpacity = lightOpa,
		atlasOffset = offset,
		solid = true,
		cvFunction = "edges"
	}
end

blocs = {
	[1] = {
		lightOpacity = 1,
		atlasOffset = 0
	},
	[2] = terrainBloc(2, 16),
	[3] = terrainBloc(3, 32),
	[4] = {
		lightOpacity = 1,
		atlasOffset = 48,
		liquid = true,
		cvFunction = "liquid"
	},
	[5] = terrainBloc(3, 64),
	[6] = terrainBloc(2, 80),
	[7] = {
		lightOpacity = 1,
		atlasOffset = 1
	},
	[8] = terrainBloc(2, 96),
	[9] = terrainBloc(2, 112)
}

backwalls = {
	[1] = {
		lightOpacity = 1,
		atlasOffset = 0
	},
	[2] = {
		lightOpacity = 1,
		atlasOffset = 16,
		cvFunction = "edges"
	},
	[3] = {
		lightOpacity = 1,
		atlasOffset = 32,
		cvFunction = "edges"
	}
}
