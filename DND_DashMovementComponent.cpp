#include "DND_DashMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UDND_DashMovementComponent::UDND_DashMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDND_DashMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// cache the movement component so we're not looking it up every tick
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerMovement = OwnerCharacter->GetCharacterMovement();
		OwnerMovement->MaxWalkSpeed = NormalMaxWalkSpeed;

		// flat top-down game, no gravity needed
		OwnerMovement->GravityScale = 0.f;
	}
}

void UDND_DashMovementComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TickDash(DeltaTime);
}

void UDND_DashMovementComponent::SetMovementInputVector(float ForwardAxis, float RightAxis)
{
	// ignore input while dashing or stunned
	if (bIsDashing) return;
	if (OwnerMovement && OwnerMovement->MovementMode == MOVE_None) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (!FMath::IsNearlyZero(ForwardAxis))
	{
		OwnerCharacter->AddMovementInput(FVector(1.f, 0.f, 0.f), ForwardAxis);
		LastInputDirection.X = ForwardAxis;
	}
	else
	{
		LastInputDirection.X = 0.f;
	}

	if (!FMath::IsNearlyZero(RightAxis))
	{
		OwnerCharacter->AddMovementInput(FVector(0.f, 1.f, 0.f), RightAxis);
		LastInputDirection.Y = RightAxis;
	}
	else
	{
		LastInputDirection.Y = 0.f;
	}
}

void UDND_DashMovementComponent::TryDash()
{
	if (!OwnerMovement) return;

	// bail if already dashing, on cooldown, or stunned
	if (bIsDashing)                               return;
	if (DashCooldownRemaining > 0.f)              return;
	if (OwnerMovement->MovementMode == MOVE_None) return;

	// use last input direction, fall back to actor forward if standing still
	FVector Dir = LastInputDirection;
	if (Dir.IsNearlyZero())
	{
		Dir   = GetOwner()->GetActorForwardVector();
		Dir.Z = 0.f;
	}
	Dir.Normalize();

	DashDirection     = Dir;
	bIsDashing        = true;
	DashTimeRemaining = DashDuration;

	OwnerMovement->MaxWalkSpeed = NormalMaxWalkSpeed * DashSpeedMultiplier;
}

void UDND_DashMovementComponent::TickDash(float DeltaTime)
{
	if (bIsDashing)
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if (OwnerCharacter)
			OwnerCharacter->AddMovementInput(DashDirection, 1.f);

		DashTimeRemaining -= DeltaTime;

		if (DashTimeRemaining <= 0.f)
		{
			// dash done, restore speed and start cooldown
			bIsDashing        = false;
			DashTimeRemaining = 0.f;

			if (OwnerMovement)
				OwnerMovement->MaxWalkSpeed = NormalMaxWalkSpeed;

			DashCooldownRemaining = DashCooldown;
		}
	}

	if (DashCooldownRemaining > 0.f)
		DashCooldownRemaining = FMath::Max(0.f, DashCooldownRemaining - DeltaTime);
}

void UDND_DashMovementComponent::ApplyMovementStun()
{
	if (!OwnerMovement) return;

	// cancel dash cleanly before freezing
	if (bIsDashing)
	{
		bIsDashing        = false;
		DashTimeRemaining = 0.f;
		OwnerMovement->MaxWalkSpeed = NormalMaxWalkSpeed;
	}

	OwnerMovement->StopMovementImmediately();
	OwnerMovement->DisableMovement();
}

void UDND_DashMovementComponent::RemoveMovementStun()
{
	if (!OwnerMovement) return;

	OwnerMovement->SetMovementMode(MOVE_Walking);
	OwnerMovement->MaxWalkSpeed = NormalMaxWalkSpeed;
}
