#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UCD_Ingredient.generated.h"

// ingredient types used to identify and match recipes
UENUM(BlueprintType)
enum class EIngredientType : uint8
{
	None      UMETA(DisplayName = "None"),
	Bread     UMETA(DisplayName = "Bread"),
	Patty     UMETA(DisplayName = "Patty"),
	Lettuce   UMETA(DisplayName = "Lettuce"),
	Tomato    UMETA(DisplayName = "Tomato"),
	Cheese    UMETA(DisplayName = "Cheese"),
	// add more here as needed
};

// a physical ingredient actor that can be picked up and combined
UCLASS()
class YOURGAME_API AUCD_Ingredient : public AActor
{
	GENERATED_BODY()

public:
	AUCD_Ingredient();

	// what type of ingredient this is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredient")
	EIngredientType IngredientType = EIngredientType::None;

	// whether this ingredient needs to be cooked before it can be used in a dish
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredient")
	bool bRequiresCooking = false;

	// whether this ingredient has been cooked
	UPROPERTY(BlueprintReadOnly, Category = "Ingredient")
	bool bIsCooked = false;
};
