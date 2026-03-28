#include "DND_BumpCollisionComponent.h"
#include "DND_DashMovementComponent.h"
#include "GameFramework/Character.h"

UDND_BumpCollisionComponent::UDND_BumpCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDND_BumpCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// bind to the hit event so we get collision callbacks without touching the character class
	Owner->OnActorHit.AddDynamic(this, &UDND_BumpCollisionComponent::OnOwnerHit);

	// grab the sibling movement component
	DashMoveComp = Owner->FindComponentByClass<UDND_DashMovementComponent>();

	if (!DashMoveComp)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("BumpCollisionComponent on %s: couldn't find DashMovementComponent, stun won't affect movement"),
			*Owner->GetName());
	}
}

void UDND_BumpCollisionComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TickBumpStun(DeltaTime);
}

void UDND_BumpCollisionComponent::OnOwnerHit(
	AActor* SelfActor,
	AActor* OtherActor,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	// only care about hits on other players that also have this component
	UDND_BumpCollisionComponent* OtherBumpComp =
		OtherActor ? OtherActor->FindComponentByClass<UDND_BumpCollisionComponent>() : nullptr;

	if (!OtherBumpComp) return;

	// check relative speed — ignore slow/gentle contact
	const FVector SelfVelocity  = SelfActor  ? SelfActor->GetVelocity()  : FVector::ZeroVector;
	const FVector OtherVelocity = OtherActor ? OtherActor->GetVelocity() : FVector::ZeroVector;
	const float   RelativeSpeed = (SelfVelocity - OtherVelocity).Size();

	if (RelativeSpeed < BumpMinRelativeSpeed) return;

	// stun both players at the same time
	ApplyBumpStun();
	OtherBumpComp->ApplyBumpStun();
}

void UDND_BumpCollisionComponent::ApplyBumpStun()
{
	// refreshes the timer if already stunned rather than stacking
	bIsBumpStunned        = true;
	BumpStunTimeRemaining = BumpImmobileDuration;

	if (DashMoveComp)
		DashMoveComp->ApplyMovementStun();
}

void UDND_BumpCollisionComponent::TickBumpStun(float DeltaTime)
{
	if (!bIsBumpStunned) return;

	BumpStunTimeRemaining -= DeltaTime;

	if (BumpStunTimeRemaining <= 0.f)
	{
		// stun over, let the player move again
		bIsBumpStunned        = false;
		BumpStunTimeRemaining = 0.f;

		if (DashMoveComp)
			DashMoveComp->RemoveMovementStun();
	}
}
