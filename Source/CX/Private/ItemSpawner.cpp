
#include "ItemSpawner.h"

#include "Engine/World.h"
#include "TimerManager.h"

AItemSpawner::AItemSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    // 아이템 생성은 서버에서만 처리
    bReplicates = true;
    SetReplicateMovement(false);
}

void AItemSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SpawnItem();
    }
}

void AItemSpawner::SpawnItem()
{
    if (!HasAuthority() || !GetWorld() || !ItemClass)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewItem = GetWorld()->SpawnActor<AActor>(
        ItemClass,
        GetActorLocation(),
        GetActorRotation(),
        SpawnParams
    );

    if (!IsValid(NewItem))
    {
        return;
    }

    SpawnedItem = NewItem;

    // 아이템이 파괴되면 재생성 예약
    SpawnedItem->OnDestroyed.AddDynamic(
        this,
        &AItemSpawner::OnSpawnedItemDestroyed
    );
}

void AItemSpawner::OnSpawnedItemDestroyed(AActor* DestroyedActor)
{
    if (!HasAuthority())
    {
        return;
    }

    if (DestroyedActor != SpawnedItem.Get())
    {
        return;
    }

    SpawnedItem = nullptr;

    GetWorldTimerManager().SetTimer(
        RespawnTimerHandle,
        this,
        &AItemSpawner::SpawnItem,
        RespawnDelay,
        false
    );
}

