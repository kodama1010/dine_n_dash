#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DND_PlayerCharacter.generated.h"

// main player character for dine n dash
// movement and dash logic -> DashMovementComponent
// bump/stun logic -> BumpCollisionComponent
// this class just owns both and handles input

// setup:
//   - replace YOURGAME_API with your module name
//   - add input mappings in project settings -> input:
//       axis:   "MoveForward" (w/s or left stick y)
//       axis:   "MoveRight"   (a/d or left stick x)
//       action: "Dash"        (spacebar or gamepad south button)

UCLASS()
class YOURGAME_API ADND_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADND_PlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDND_DashMovementComponent* DashMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDND_BumpCollisionComponent* BumpCollisionComponent;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnDashPressed();

	// cached so both axes can be sent together each frame
	// avoids one axis overwriting the other with 0
	float CachedForwardAxis = 0.f;
	float CachedRightAxis   = 0.f;
};
