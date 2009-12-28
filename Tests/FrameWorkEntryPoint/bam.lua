Import("../../framework.lua")

Includes = "Source"
SourceFiles = Collect("Source/*.cpp")
RequiredLibs = {}

BuildProject("test", RequiredLibs, Includes, SourceFiles)

DefaultTarget("test" .. "_debug")
