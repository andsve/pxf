local module_filename = ModuleFilename()
Libraries["enet"] = {
    Build = function(settings)  
                settings.cc.defines:Add("CONF_WITH_ENET")
                settings.cc.includes:Add(Path(PathDir(module_filename) .. "/sdk/include"))
                src = Collect(Path(PathDir(module_filename) .. "/sdk/src/*.c"))
                return Compile(settings, src)
            end
}