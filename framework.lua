-- http://shrine.teeworlds.com/bam/bam.html#6.4.3
-- http://teeworlds.com/trac/teeworlds/browser/trunk/bam.lua
-- http://teeworlds.com/trac/teeworlds/browser/trunk/other/sdl/sdl.lua

path_prefix = PathDir(ModuleFilename())

-- Import all Libraries/*/*.lua and add to dictionary Libraries
Libraries = {}
for i,n in ipairs(CollectDirs(path_prefix .. "/Libraries/")) do
    name = PathFilename(PathBase(n))
    Import(n .. "/" .. name .. ".lua")
end

object_base = Path(path_prefix .. "/Build/ObjectFiles/")
include_base = Path(path_prefix .. "/Include")
source_base = Path(path_prefix .. "/Source/*.cpp")

debug_settings = NewSettings()
debug_settings.config_name = "debug"
debug_settings.config_ext = "_d"
debug_settings.debug = 1
debug_settings.optimize = 0
debug_settings.cc.defines:Add("CONF_DEBUG")

release_settings = NewSettings()
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
    
       -- Libraries
    if family == "windows" then
    
        -- Set compiler specific values
        --  if config.compiler.value == "cl" then
        --      settings.cc.flags:Add("/EHsc")
        --  elseif config.compiler.value == "gcc" then
        --      settings.cc.flags:Add("-Wall", "-fno-exceptions")
        --  end
        settings.cc.flags:Add("/EHsc")
        
        settings.link.libs:Add("opengl32")
        settings.link.libs:Add("glu32")
        settings.link.libs:Add("ws2_32")
        settings.link.libs:Add("gdi32")
        settings.link.libs:Add("user32")
        --settings.link.libs:Add("dsound")
        settings.link.libs:Add("ole32")
        settings.link.libs:Add("d3d9")
        
        settings.cc.includes:Add(Path("%DXSDK_DIR%\\Include"))
        
    elseif family == "unix" then
        if platform == "macosx" then
            settings.link.frameworks:Add("AGL")
            settings.link.frameworks:Add("OpenGL")
            settings.link.frameworks:Add("Carbon")
            settings.link.frameworks:Add("AudioToolbox")
            settings.link.frameworks:Add("CoreAudio")
            settings.link.frameworks:Add("AudioUnit")
        else
            settings.link.libs:Add("asound")
            settings.link.libs:Add("GLU")
            settings.link.libs:Add("GL")
            settings.link.libs:Add("pthread")
        end
    end
    
    -- Add include directory
    settings.cc.includes:Add(include_base)
 
    return settings
end

function tableMerge(t1, t2)
    for k,v in pairs(t2) do
        if type(v) == "table" then
                if type(t1[k] or false) == "table" then
                        tableMerge(t1[k] or {}, t2[k] or {})
                else
                        t1[k] = v
                end
        else
                t1[k] = v
        end
    end
    return t1
end

function BuildFramework(settings)
    -- Compile framework
    --framework = Compile(settings, CollectRecursive(source_base))
             
    
    framework = Compile(settings, Collect(path_prefix .. "/Source/*.cpp"),
                                  CollectRecursive(path_prefix .. "/Source/Base/*.cpp",
                                                   path_prefix .. "/Source/Extra/*.cpp",
                                                   path_prefix .. "/Source/Graphics/*.cpp",
                                                   path_prefix .. "/Source/Input/*.cpp",
                                                   path_prefix .. "/Source/Math/*.cpp",
                                                   path_prefix .. "/Source/Resource/*.cpp"))
    return framework
end

function BuildLibrary(settings, library)
    lib_settings = settings:Copy()
    lib_src = Collect(library.SourceFiles)
    lib = Compile(lib_settings, lib_src)
    return lib
end

DebugSettings = CreateSettings(debug_settings)
ReleaseSettings = CreateSettings(release_settings)

function BuildProject(name, required_libs, include_dir, source_files)
    function DoBuild(settings)
        
        CompiledLibs = {}
        for i,n in ipairs(required_libs) do
            CompiledLibs[i] = Libraries[n].Build(settings)
        end
        
        framework = BuildFramework(settings)
        
        settings.cc.includes:Add(Path(path_prefix .. include_dir))
        
        -- Compile Project
        project = Compile(settings, source_files)
        project_exe = Link(settings, name, project, CompiledLibs, framework)
        project_target = PseudoTarget(name.."_"..settings.config_name, project_exe)

        PseudoTarget(settings.config_name, project_target)
    end
    
    DoBuild(DebugSettings)
    DoBuild(ReleaseSettings)
end
