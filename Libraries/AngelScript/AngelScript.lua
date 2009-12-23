AngelScript = {
	IncludeDir = PathPath(ModuleFilename()) .. "/sdk/angelscript/include",
	SourceFiles = PathPath(ModuleFilename()) .. "/sdk/angelscript/source/*.cpp",
	Defines = {"CONF_WITH_ANGELSCRIPT"}
}