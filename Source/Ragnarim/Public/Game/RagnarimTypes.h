#pragma once

#include "CoreMinimal.h"
#include "RagnarimTypes.generated.h"

UENUM(BlueprintType)
enum class EBuildingCategory : uint8
{
    Structure    UMETA(DisplayName = "Structure"),
    Furniture    UMETA(DisplayName = "Furniture"),
    Artisanat     UMETA(DisplayName = "Artisanat"),
    Research     UMETA(DisplayName = "Research"),
    Defense      UMETA(DisplayName = "Defense"),
    Unbuildable      UMETA(DisplayName = "Unbuildable")
};