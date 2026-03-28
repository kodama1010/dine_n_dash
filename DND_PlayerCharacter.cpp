#include "DND_PlayerCharacter.h"
#include "DND_DashMovementComponent.h"
#include "DND_BumpCollisionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

ADND_PlayerCharacter::ADND_PlayerCharacter()
{
	// attach both components
	DashMovementComponent = CreateDefaultSubobject<UDND_DashMovementComponent>(
		TEXT("DashMovementComponent"));

	BumpCollisionComponent = CreateDefaultSubobject<UDND_BumpCollisionComponent>(
		TEXT("BumpCollisionComponent"));

	// capsule needs hit events on so OnActorHit fires
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->BodyInstance.bNotifyRigidBodyCollision = true;
}

void ADND_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// components handle their own setup in their BeginPlay
}

void ADND_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADND_PlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",   this, &ADND_PlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ADND_PlayerCharacter::OnDashPressed);
}

void ADND_PlayerCharacter::MoveForward(float Value)
{
	// cache then send both axes together so neither overwrites the other
	CachedForwardAxis = Value;
	if (DashMovementComponent)
		DashMovementComponent->SetMovementInputVector(CachedForwardAxis, CachedRightAxis);
}

void ADND_PlayerCharacter::MoveRight(float Value)
{
	CachedRightAxis = Value;
	if (DashMovementComponent)
		DashMovementComponent->SetMovementInputVector(CachedForwardAxis, CachedRightAxis);
}

void ADND_PlayerCharacter::OnDashPressed()
{
	if (DashMovementComponent)
		DashMovementComponent->TryDash();
}
