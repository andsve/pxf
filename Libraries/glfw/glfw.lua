local module_filename = ModuleFilename()
Libraries["glfw"] = {
    Build = function(settings)
                if family == "unix" then
                    if platform == "macosx" then
                        glfw_platform = "macosx"
                    else
                        glfw_platform = "x11"
                    end
                elseif family == "windows" then
                    glfw_platform = "win32"
                end
    
                settings.cc.defines:Add("CONF_WITH_RTAUDIO")
                settings.cc.includes:Add(Path(PathPath(module_filename) .. "/sdk/include"))
                settings.cc.includes:Add(Path(PathPath(module_filename) .. "/sdk/src"))
                settings.cc.includes:Add(Path(PathPath(module_filename) .. "/sdk/src/" .. glfw_platform))
                src = Collect(Path(PathPath(module_filename) .. "/sdk/src/*.c"),
                              Path(PathPath(module_filename) .. "/sdk/src/" .. glfw_platform .. "/*.c")
                )
                return Compile(settings, src)
            end
}