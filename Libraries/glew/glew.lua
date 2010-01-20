local module_filename = ModuleFilename()
Libraries["glew"] = {
    Build = function(settings)
                settings.cc.defines:Add("CONF_WITH_GLEW")
                settings.cc.defines:Add("GLEW_STATIC")
                settings.cc.includes:Add(Path(PathPath(module_filename) .. "/sdk/include"))

                src = Collect(Path(PathPath(module_filename) .. "/sdk/source/*.c"))
                
                return Compile(settings, src)
            end
}