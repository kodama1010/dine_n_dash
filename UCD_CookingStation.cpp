#include "UCD_CookingStation.h"

AUCD_CookingStation::AUCD_CookingStation()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AUCD_CookingStation::BeginPlay()
{
	Super::BeginPlay();
}

void AUCD_CookingStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsCooking || !CurrentIngredient) return;

	if (!bCookingComplete)
	{
		// count up until we hit the cook time
		CookTimer += DeltaTime;

		if (CookTimer >= CookTime)
		{
			// ingredient is cooked
			bCookingComplete = true;
			CurrentIngredient->bIsCooked = true;
			CookTimer = CookTime; // clamp so progress bar stays at 1

			UE_LOG(LogTemp, Log, TEXT("CookingStation: %s is done cooking"),
				*CurrentIngredient->GetName());
		}
	}
	else if (bCanBurn && !bIsBurned)
	{
		// ingredient is cooked but sitting too long — start burn timer
		BurnTimer += DeltaTime;

		if (BurnTimer >= BurnTime)
		{
			bIsBurned = true;
			UE_LOG(LogTemp, Log, TEXT("CookingStation: ingredient burned!"));
		}
	}
}

bool AUCD_CookingStation::PlaceIngredient(AUCD_Ingredient* Ingredient)
{
	if (!Ingredient) return false;

	// station already has something on it
	if (CurrentIngredient)
	{
		UE_LOG(LogTemp, Warning, TEXT("CookingStation: already occupied"));
		return false;
	}

	// only accept ingredients that actually need cooking
	if (!Ingredient->bRequiresCooking)
	{
		UE_LOG(LogTemp, Warning, TEXT("CookingStation: this ingredient doesn't need cooking"));
		return false;
	}

	CurrentIngredient = Ingredient;
	bIsCooking        = true;
	bCookingComplete  = false;
	bIsBurned         = false;
	CookTimer         = 0.f;
	BurnTimer         = 0.f;

	// hide the ingredient visually since it's now "on" the station
	Ingredient->SetActorHiddenInGame(true);
	Ingredient->SetActorEnableCollision(false);

	return true;
}

AUCD_Ingredient* AUCD_CookingStation::TakeIngredient()
{
	// can't take it if nothing is here or it's not done yet
	if (!CurrentIngredient || !bCookingComplete) return nullptr;

	// burned food can still be taken but it won't satisfy any recipe
	AUCD_Ingredient* TakenIngredient = CurrentIngredient;

	// restore visibility now that player is holding it
	TakenIngredient->SetActorHiddenInGame(false);
	TakenIngredient->SetActorEnableCollision(true);

	// reset station state
	CurrentIngredient = nullptr;
	bIsCooking        = false;
	bCookingComplete  = false;
	bIsBurned         = false;
	CookTimer         = 0.f;
	BurnTimer         = 0.f;

	return TakenIngredient;
}

float AUCD_CookingStation::GetCookProgress() const
{
	if (CookTime <= 0.f) return 0.f;
	return FMath::Clamp(CookTimer / CookTime, 0.f, 1.f);
}
