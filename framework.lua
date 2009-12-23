-- http://shrine.teeworlds.com/bam/bam.html#6.4.3
-- http://teeworlds.com/trac/teeworlds/browser/trunk/bam.lua
-- http://teeworlds.com/trac/teeworlds/browser/trunk/other/sdl/sdl.lua

path_prefix = PathPath(ModuleFilename())

Import(path_prefix .. "/Libraries/AngelScript/AngelScript.lua")
Import(path_prefix .. "/Libraries/RtAudio/RtAudio.lua")

object_base = path_prefix .. Path("/Build/ObjectFiles/")
include_base = path_prefix .. "/Include"
source_base = path_prefix .. "/Source/*.cpp"

config = NewConfig()
config:Add(OptCCompiler("cc"))
config:Finalize("config.bam")

debug_settings = config:NewSettings()
debug_settings.config_name = "debug"
debug_settings.config_ext = "_d"
debug_settings.debug = 1
debug_settings.optimize = 0
debug_settings.cc.defines:Add("CONF_DEBUG")

release_settings = config:NewSettings()
release_settings.config_name = "release"
release_settings.config_ext = ""
release_settings.debug = 0
release_settings.optimize = 1
release_settings.cc.defines:Add("CONF_RELEASE")

function Intermediate_Output(settings, input)
    return Path(object_base .. "/" .. PathBase(PathFilename(input)) .. settings.config_ext)
end

function CreateSettings(settings)
    -- Intermediate directory for object files
    settings.cc.Output = Intermediate_Output
    
    -- Set compiler specific values
--    if config.compiler.value == "cl" then
--        settings.cc.flags:Add("/EHsc")
--    elseif config.compiler.value == "gcc" then
--        settings.cc.flags:Add("-Wall", "-fno-exceptions")
--    end
	settings.cc.flags:Add("/EHsc")
    
    -- Add include directory
    settings.cc.includes:Add(include_base)
    
    -- Add library includes
    settings.cc.includes:Add(RtAudio.IncludeDir)
    settings.cc.includes:Add(AngelScript.IncludeDir)
    
    -- Separate setting files
    framework_settings = settings:Copy()
 
	return framework_settings
end

function BuildFramework()
	    -- Compile framework
    framework = Compile(framework_settings, CollectRecursive(source_base))
    return framework
end

function BuildLibrary(settings, library)
	lib_settings = settings:Copy()
	lib_src = Collect(library.SourceFiles)
	lib = Compile(lib_settings, lib_src)
	return lib
end

function BuildProject(settings, framework, name, source_dir)
	-- Compile RtAudio
	rtaudio = BuildLibrary(settings, RtAudio)
	
	-- Compile AngelScript
	angelscript = BuildLibrary(settings, AngelScript)
	
    -- Compile Project
    project = Compile(settings, CollectRecursive(source_dir.."/*.cpp"))
    project_exe = Link(settings, name, project, framework, rtaudio, angelscript)
    project_target = PseudoTarget(name.."_"..settings.config_name, project_exe)

    PseudoTarget(settings.config_name, project_target)
end

FrameworkDebugSettings = CreateSettings(debug_settings)
FrameworkDebug = BuildFramework(FrameworkDebugSettings)

FrameworkReleaseSettings = CreateSettings(release_settings)
FrameworkRelease = BuildFramework(FrameworkReleaseSettings)


