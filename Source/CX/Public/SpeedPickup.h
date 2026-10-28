
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpeedPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USoundBase;
class UNiagaraSystem;

UCLASS()
class CX_API ASpeedPickup : public AActor
{
    GENERATED_BODY()

public:
    ASpeedPickup();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speed")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speed")
    UStaticMeshComponent* PickupMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Speed")
    float SpeedBonus = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Speed|Effects")
    UNiagaraSystem* PickupEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Speed|Effects")
    USoundBase* PickupSound;

    UFUNCTION()
    void OnPickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPickupEffect();
};

