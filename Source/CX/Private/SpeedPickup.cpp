
#include "SpeedPickup.h"
#include "../CXPawn.h"
#include "CXPlayerState.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ASpeedPickup::ASpeedPickup()
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

    PickupMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));

    PickupMesh->SetupAttachment(CollisionSphere);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASpeedPickup::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        CollisionSphere->OnComponentBeginOverlap.AddDynamic(
            this, &ASpeedPickup::OnPickupOverlap);
    }
}

void ASpeedPickup::OnPickupOverlap(
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

    PlayerState->AddBonusMaxSpeed(SpeedBonus);

    CollisionSphere->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);

    MulticastPickupEffect();

    SetActorHiddenInGame(true);
    Destroy();
}

void ASpeedPickup::MulticastPickupEffect_Implementation()
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

