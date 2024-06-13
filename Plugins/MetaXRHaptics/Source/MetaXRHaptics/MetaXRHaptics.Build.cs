/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 *
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

using UnrealBuildTool;
using System.IO;

public class MetaXRHaptics : ModuleRules
{
    public MetaXRHaptics(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Projects" });

        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "../ThirdParty/include") });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/lib/Win64/haptics_sdk.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            var PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "CopyNativeLibraryAndroid.xml"));
        }
        else
        {
            throw new BuildException("Unsupported target platform");
        }
    }
}
