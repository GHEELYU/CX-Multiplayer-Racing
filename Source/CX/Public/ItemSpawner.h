
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

UCLASS()
class CX_API AItemSpawner : public AActor
{
    GENERATED_BODY()

public:
    AItemSpawner();

protected:
    virtual void BeginPlay() override;

    // 스포너가 생성할 아이템 종류
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
    TSubclassOf<AActor> ItemClass;

    // 아이템 파괴 후 재생성 대기 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
    float RespawnDelay = 2.0f;

    // 현재 생성된 아이템
    UPROPERTY()
    TObjectPtr<AActor> SpawnedItem;

    FTimerHandle RespawnTimerHandle;

    void SpawnItem();

    UFUNCTION()
    void OnSpawnedItemDestroyed(AActor* DestroyedActor);
};

