Import("../../framework.lua")

Includes = "Source"
SourceFiles = Collect("Source/*.cpp")
RequiredLibs = {"s7"}

BuildProject("test", RequiredLibs, Includes, SourceFiles)

DefaultTarget("test" .. "_debug")
