#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DND_BumpCollisionComponent.generated.h"

// handles player-to-player bump detection and stun
// when two players collide fast enough, both freeze briefly
//
// how to use:
//   add this alongside DashMovementComponent on the character
//   make sure the capsule collision is set to BlockAllDynamic
//   both components must be on the same actor

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YOURGAME_API UDND_BumpCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDND_BumpCollisionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// how long players are frozen after a bump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bump")
	float BumpImmobileDuration = 0.5f;

	// minimum relative speed needed to trigger a bump stun
	// stops players from freezing just by standing next to each other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bump")
	float BumpMinRelativeSpeed = 200.f;

	UFUNCTION(BlueprintPure, Category = "Bump")
	bool IsBumpStunned() const { return bIsBumpStunned; }

	// applies stun to this character
	// also called on the other player during a collision so both freeze
	void ApplyBumpStun();

private:
	bool  bIsBumpStunned        = false;
	float BumpStunTimeRemaining = 0.f;

	// sibling component on the same actor
	class UDND_DashMovementComponent* DashMoveComp = nullptr;

	UFUNCTION()
	void OnOwnerHit(
		AActor* SelfActor,
		AActor* OtherActor,
		FVector NormalImpulse,
		const FHitResult& Hit);

	void TickBumpStun(float DeltaTime);
};
