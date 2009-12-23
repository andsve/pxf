RtAudio = {
	IncludeDir = PathPath(ModuleFilename()) .. "/sdk/",
	SourceFiles = PathPath(ModuleFilename()) .. "/sdk/*.cpp",
	Defines = {"CONF_WITH_RTAUDIO"}
}