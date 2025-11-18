workspace "HackAssembler"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Assembler"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "Assembler/Build.lua"
