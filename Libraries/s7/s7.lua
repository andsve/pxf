local module_filename = ModuleFilename()
Libraries["s7"] = {
    Build = function(settings)
                IncludeDir = Path(PathPath(module_filename) .. "/sdk/")
                SourceFiles = Path(PathPath(module_filename) .. "/sdk/*.c")
                settings.cc.defines:Add("CONF_WITH_S7")
                settings.cc.includes:Add(IncludeDir)
                src = Collect(SourceFiles)
                return Compile(settings, src)
            end
}