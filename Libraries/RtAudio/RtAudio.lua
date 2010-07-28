local module_filename = ModuleFilename()
Libraries["RtAudio"] = {
    Build = function(settings)
                IncludeDir = Path(PathDir(module_filename) .. "/sdk/")
                SourceFiles = Path(PathDir(module_filename) .. "/sdk/*.cpp")
                settings.cc.defines:Add("CONF_WITH_RTAUDIO")
                settings.cc.includes:Add(IncludeDir)
                src = Collect(SourceFiles)
                return Compile(settings, src)
            end
}