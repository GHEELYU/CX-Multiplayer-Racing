
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class CX_API ACoinPickup : public AActor
{
    GENERATED_BODY()

public:
    ACoinPickup();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    UStaticMeshComponent* CoinMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coin")
    int32 CoinValue = 10;

    UPROPERTY(EditDefaultsOnly, Category = "Coin|Effects")
    UNiagaraSystem* PickupEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Coin|Effects")
    USoundBase* PickupSound;

    UFUNCTION()
    void OnCoinOverlap(
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
