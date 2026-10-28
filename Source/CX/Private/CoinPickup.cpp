
#include "CoinPickup.h"
#include "../CXPawn.h"
#include "CXPlayerState.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ACoinPickup::ACoinPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    NetUpdateFrequency = 1.0f;

    CollisionSphere =
        CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));

    SetRootComponent(CollisionSphere);

    CollisionSphere->InitSphereRadius(120.0f);
    CollisionSphere->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(
        ECC_Pawn, ECR_Overlap);
    CollisionSphere->SetGenerateOverlapEvents(true);

    CoinMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));

    CoinMesh->SetupAttachment(CollisionSphere);
    CoinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACoinPickup::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        CollisionSphere->OnComponentBeginOverlap.AddDynamic(
            this, &ACoinPickup::OnCoinOverlap);
    }
}

void ACoinPickup::OnCoinOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!HasAuthority())
    {
        return;
    }

    ACXPawn* Vehicle = Cast<ACXPawn>(OtherActor);

    if (!IsValid(Vehicle))
    {
        return;
    }

    ACXPlayerState* PlayerState =
        Vehicle->GetPlayerState<ACXPlayerState>();

    if (!IsValid(PlayerState))
    {
        return;
    }

    // 서버에서 코인 지급
    PlayerState->AddCoins(CoinValue);

    // 모든 클라이언트에 획득 효과 전달
    MulticastPickupEffect();

    CollisionSphere->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);

    SetActorHiddenInGame(true);
    Destroy();
}

void ACoinPickup::MulticastPickupEffect_Implementation()
{
    const FVector Location = GetActorLocation();

    if (PickupEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            PickupEffect,
            Location
        );
    }

    if (PickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            PickupSound,
            Location
        );
    }
}

