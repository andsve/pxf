local module_filename = ModuleFilename()
Libraries["luasocket"] = {
    Build = function(settings)  
                settings.cc.defines:Add("CONF_WITH_LUASOCKET")
                --settings.cc.defines:Add("LUASOCKET_API")
                settings.cc.includes:Add(Path(PathPath(module_filename) .. "/sdk/include"))
                src = Collect(Path(PathPath(module_filename) .. "/sdk/src/*.c"))
                return Compile(settings, src)
            end
}