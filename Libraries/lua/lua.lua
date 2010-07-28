local module_filename = ModuleFilename()
Libraries["lua"] = {
    Build = function(settings)  
                settings.cc.defines:Add("CONF_WITH_LUA")
                settings.cc.includes:Add(Path(PathDir(module_filename) .. "/sdk/include"))
                src = Collect(Path(PathDir(module_filename) .. "/sdk/src/*.c"))
                return Compile(settings, src)
            end
}