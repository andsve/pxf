Import("../../framework.lua")

BuildProject(FrameworkDebugSettings, FrameworkDebug, "test", "Source")
BuildProject(FrameworkReleaseSettings, FrameworkRelease, "test", "Source")

DefaultTarget("test" .. "_debug")
