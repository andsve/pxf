local module_filename = ModuleFilename()
Libraries["soil"] = {
    Build = function(settings)  
                settings.cc.defines:Add("CONF_WITH_SOIL")
                settings.cc.includes:Add(Path(PathPath(module_filename) .. "/sdk/include"))
                src = Collect(Path(PathPath(module_filename) .. "/sdk/src/*.c"))
                return Compile(settings, src)
            end
}