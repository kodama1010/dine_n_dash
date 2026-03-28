#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DND_DashMovementComponent.generated.h"

// handles all movement and dashing for the player
// attach to any ACharacter to get top-down movement + dash
//
// how to use:
//   call SetMovementInputVector() each frame from the character's input handlers
//   call TryDash() from the dash input action
//   tune the values below in the editor

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UDND_DashMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDND_DashMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalMaxWalkSpeed = 600.f;

	// pass in the raw forward and right axis values each frame
	void SetMovementInputVector(float ForwardAxis, float RightAxis);

	// dash settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashSpeedMultiplier = 4.f;

	// how long the dash burst lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashDuration = 0.2f;

	// time before player can dash again after one finishes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashCooldown = 1.0f;

	// call this from the dash input action
	void TryDash();

	UFUNCTION(BlueprintPure, Category = "Dash")
	bool IsDashing() const { return bIsDashing; }

	UFUNCTION(BlueprintPure, Category = "Dash")
	bool IsDashOnCooldown() const { return DashCooldownRemaining > 0.f; }

	// called by the bump component to freeze this character
	void ApplyMovementStun();

	// called by the bump component once stun timer runs out
	void RemoveMovementStun();

private:
	class UCharacterMovementComponent* OwnerMovement = nullptr;

	// last non-zero input direction, used to decide which way to dash
	FVector LastInputDirection = FVector::ZeroVector;

	bool  bIsDashing            = false;
	float DashTimeRemaining     = 0.f;
	float DashCooldownRemaining = 0.f;
	FVector DashDirection       = FVector::ZeroVector;

	void TickDash(float DeltaTime);
};
