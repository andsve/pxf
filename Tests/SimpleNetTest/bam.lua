Import("../../framework.lua")

Includes = "Source"
SourceFiles = Collect("Source/*.cpp")
RequiredLibs = {"glew", "glfw", "soil", "lua", "enet"}

BuildProject("test", RequiredLibs, Includes, SourceFiles)

DefaultTarget("test" .. "_debug")
