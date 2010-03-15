Import("../../framework.lua")

Includes = "Source"
SourceFiles = Collect("Source/*.cpp")
RequiredLibs = {"glew", "glfw", "soil", "lua"}

BuildProject("test", RequiredLibs, Includes, SourceFiles)

DefaultTarget("test" .. "_debug")
