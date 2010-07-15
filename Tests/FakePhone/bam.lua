Import("../../framework.lua")

debug_settings.cc.defines:Add("TARGET_OS_IPHONEFAKEDEV")
release_settings.cc.defines:Add("TARGET_OS_IPHONEFAKEDEV")

Includes = "Source"
SourceFiles = Collect("Source/*.cpp")
RequiredLibs = {"glew", "glfw", "soil", "lua", "enet", "box2d"}

BuildProject("test", RequiredLibs, Includes, SourceFiles)

DefaultTarget("test" .. "_debug")
