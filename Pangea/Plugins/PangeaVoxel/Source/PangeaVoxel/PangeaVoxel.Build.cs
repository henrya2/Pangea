using System.IO;
using UnrealBuildTool;

public class PangeaVoxel : ModuleRules
{
    public PangeaVoxel(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "ApplicationCore",
                "Core",
                "Engine",
                "Renderer",
                "SignalProcessing",
                "TraceLog",
                "FastNoiseGenerator",
                "FastNoise",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[] {
                "CoreUObject",
                "PhysicsCore",
                "RenderCore",
                "RHI",
                "VectorVM",
                "ProceduralMeshComponent"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(ModuleDirectory, "Private")
            });
    }
}
