local module_filename = ModuleFilename()
Libraries["box2d"] = {
    Build = function(settings)  
                settings.cc.defines:Add("CONF_WITH_BOX2D")
                settings.cc.includes:Add(Path(PathDir(module_filename) .. "/sdk/include"))
                src = Collect(Path(PathDir(module_filename) .. "/sdk/src/*.c"))
                return Compile(settings, src)
            end
}