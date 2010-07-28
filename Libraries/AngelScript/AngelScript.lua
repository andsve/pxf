local module_filename = ModuleFilename()
Libraries["AngelScript"] = {
    Build = function(settings)
                IncludeDir = Path(PathDir(module_filename) .. "/sdk/angelscript/include")
                SourceFiles = Path(PathDir(module_filename) .. "/sdk/angelscript/source/*.cpp")
                settings.cc.defines:Add("CONF_WITH_ANGELSCRIPT")
                settings.cc.includes:Add(IncludeDir)
                src = Collect(SourceFiles)
                return Compile(settings, src)
            end
}