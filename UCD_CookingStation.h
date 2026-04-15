#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UCD_Ingredient.h"
#include "UCD_CookingStation.generated.h"

// cooking station — player places a cookable ingredient here
// it cooks over time and is ready to pick up when done
// if left too long it burns (optional, can disable in editor)
UCLASS()
class YOURGAME_API AUCD_CookingStation : public AActor
{
	GENERATED_BODY()

public:
	AUCD_CookingStation();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// how long in seconds the ingredient takes to cook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	float CookTime = 5.f;

	// if true, leaving it too long after cooking marks it as burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	bool bCanBurn = true;

	// how long after cooking finishes before it burns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	float BurnTime = 4.f;

	// place an ingredient on this station — called by the player
	// returns false if the station is already occupied
	bool PlaceIngredient(AUCD_Ingredient* Ingredient);

	// pick up the cooked ingredient — called by the player
	// returns nullptr if nothing is ready yet
	AUCD_Ingredient* TakeIngredient();

	// true while an ingredient is on the station
	UFUNCTION(BlueprintPure, Category = "Cooking")
	bool IsOccupied() const { return CurrentIngredient != nullptr; }

	// true once the ingredient has finished cooking
	UFUNCTION(BlueprintPure, Category = "Cooking")
	bool IsCookingComplete() const { return bCookingComplete; }

	// true if the ingredient burned
	UFUNCTION(BlueprintPure, Category = "Cooking")
	bool IsBurned() const { return bIsBurned; }

	// 0-1 progress value, useful for driving a progress bar in ui
	UFUNCTION(BlueprintPure, Category = "Cooking")
	float GetCookProgress() const;

private:
	AUCD_Ingredient* CurrentIngredient = nullptr;

	bool  bIsCooking        = false;
	bool  bCookingComplete  = false;
	bool  bIsBurned         = false;
	float CookTimer         = 0.f;
	float BurnTimer         = 0.f;
};
