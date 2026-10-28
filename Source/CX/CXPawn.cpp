
#include "CXPawn.h"
#include "CXPlayerState.h"
#include "CXWheelFront.h"
#include "CXWheelRear.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

ACXPawn::ACXPawn()
{
    bReplicates = true;
    SetReplicateMovement(true);

    FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
    FrontSpringArm->SetupAttachment(GetMesh());
    FrontSpringArm->TargetArmLength = 0.0f;
    FrontSpringArm->bDoCollisionTest = false;
    FrontSpringArm->bEnableCameraRotationLag = true;
    FrontSpringArm->CameraRotationLagSpeed = 15.0f;
    FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

    FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
    FrontCamera->SetupAttachment(FrontSpringArm);
    FrontCamera->bAutoActivate = false;

    BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
    BackSpringArm->SetupAttachment(GetMesh());
    BackSpringArm->TargetArmLength = 650.0f;
    BackSpringArm->SocketOffset.Z = 150.0f;
    BackSpringArm->bDoCollisionTest = false;
    BackSpringArm->bInheritPitch = false;
    BackSpringArm->bInheritRoll = false;
    BackSpringArm->bEnableCameraRotationLag = true;
    BackSpringArm->CameraRotationLagSpeed = 2.0f;
    BackSpringArm->CameraLagMaxDistance = 50.0f;

    BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
    BackCamera->SetupAttachment(BackSpringArm);

    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionProfileName(FName("Vehicle"));

    ChaosVehicleMovement =
        CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
}

void ACXPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(
            SteeringAction, ETriggerEvent::Triggered, this, &ACXPawn::Steering);
        EnhancedInputComponent->BindAction(
            SteeringAction, ETriggerEvent::Completed, this, &ACXPawn::Steering);

        EnhancedInputComponent->BindAction(
            ThrottleAction, ETriggerEvent::Triggered, this, &ACXPawn::Throttle);
        EnhancedInputComponent->BindAction(
            ThrottleAction, ETriggerEvent::Completed, this, &ACXPawn::Throttle);

        EnhancedInputComponent->BindAction(
            BrakeAction, ETriggerEvent::Triggered, this, &ACXPawn::Brake);
        EnhancedInputComponent->BindAction(
            BrakeAction, ETriggerEvent::Started, this, &ACXPawn::StartBrake);
        EnhancedInputComponent->BindAction(
            BrakeAction, ETriggerEvent::Completed, this, &ACXPawn::StopBrake);

        EnhancedInputComponent->BindAction(
            HandbrakeAction, ETriggerEvent::Started, this, &ACXPawn::StartHandbrake);
        EnhancedInputComponent->BindAction(
            HandbrakeAction, ETriggerEvent::Completed, this, &ACXPawn::StopHandbrake);

        EnhancedInputComponent->BindAction(
            LookAroundAction, ETriggerEvent::Triggered, this, &ACXPawn::LookAround);
        EnhancedInputComponent->BindAction(
            ToggleCameraAction, ETriggerEvent::Triggered, this, &ACXPawn::ToggleCamera);

        EnhancedInputComponent->BindAction(
            ResetVehicleAction, ETriggerEvent::Triggered, this, &ACXPawn::ResetVehicle);

        if (FireAction)
        {
            EnhancedInputComponent->BindAction(
                FireAction, ETriggerEvent::Started, this, &ACXPawn::Fire);
        }
    }
    else
    {
        UE_LOG(LogTemplateVehicle, Error,
            TEXT("Failed to find Enhanced Input component."));
    }
}

void ACXPawn::Tick(float Delta)
{
    Super::Tick(Delta);

    const bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
    GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

    float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
    CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

    BackSpringArm->SetRelativeRotation(
        FRotator(0.0f, CameraYaw, 0.0f));

    const float CurrentSpeedKmh =
        GetVelocity().Size() * 0.036f;

    float BonusSpeed = 0.0f;

    if (const ACXPlayerState* PS = GetPlayerState<ACXPlayerState>())
    {
        BonusSpeed = PS->GetBonusMaxSpeed();
    }

    const float AllowedMaxSpeed =
        BaseMaxSpeedKmh + BonusSpeed;

    if (CurrentSpeedKmh >= AllowedMaxSpeed)
    {
        ChaosVehicleMovement->SetThrottleInput(0.0f);
    }
}

void ACXPawn::Steering(const FInputActionValue& Value)
{
    ChaosVehicleMovement->SetSteeringInput(Value.Get<float>());
}

void ACXPawn::Throttle(const FInputActionValue& Value)
{
    ChaosVehicleMovement->SetThrottleInput(Value.Get<float>());
}

void ACXPawn::Brake(const FInputActionValue& Value)
{
    ChaosVehicleMovement->SetBrakeInput(Value.Get<float>());
}

void ACXPawn::StartBrake(const FInputActionValue& Value)
{
    BrakeLights(true);
}

void ACXPawn::StopBrake(const FInputActionValue& Value)
{
    BrakeLights(false);
    ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ACXPawn::StartHandbrake(const FInputActionValue& Value)
{
    ChaosVehicleMovement->SetHandbrakeInput(true);
    BrakeLights(true);
}

void ACXPawn::StopHandbrake(const FInputActionValue& Value)
{
    ChaosVehicleMovement->SetHandbrakeInput(false);
    BrakeLights(false);
}

void ACXPawn::LookAround(const FInputActionValue& Value)
{
    BackSpringArm->AddLocalRotation(
        FRotator(0.0f, Value.Get<float>(), 0.0f));
}

void ACXPawn::ToggleCamera(const FInputActionValue& Value)
{
    bFrontCameraActive = !bFrontCameraActive;

    FrontCamera->SetActive(bFrontCameraActive);
    BackCamera->SetActive(!bFrontCameraActive);
}

void ACXPawn::ResetVehicle(const FInputActionValue& Value)
{
    const FVector ResetLocation =
        GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

    FRotator ResetRotation = GetActorRotation();
    ResetRotation.Pitch = 0.0f;
    ResetRotation.Roll = 0.0f;

    SetActorTransform(
        FTransform(ResetRotation, ResetLocation, FVector::OneVector),
        false,
        nullptr,
        ETeleportType::TeleportPhysics
    );

    GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

    UE_LOG(LogTemplateVehicle, Log, TEXT("Reset Vehicle"));
}

void ACXPawn::Fire(const FInputActionValue& Value)
{
    if (!IsLocallyControlled())
    {
        return;
    }

    ServerFire();
}

void ACXPawn::ServerFire_Implementation()
{
    if (!HasAuthority() || !GetWorld())
    {
        return;
    }

    const FVector Forward = GetMesh()->GetForwardVector();

    const FVector Start =
        GetMesh()->GetComponentLocation()
        + Forward * 250.0f
        + GetMesh()->GetUpVector() * 50.0f;

    const FVector End = Start + Forward * AttackRange;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    const FVector EffectEnd = bHit ? Hit.ImpactPoint : End;

    MulticastAttackEffect(Start, EffectEnd, bHit);

    if (!bHit)
    {
        return;
    }

    ACXPawn* TargetVehicle = Cast<ACXPawn>(Hit.GetActor());

    if (!IsValid(TargetVehicle))
    {
        return;
    }

    UGameplayStatics::ApplyDamage(
        TargetVehicle,
        5.0f,
        GetController(),
        this,
        UDamageType::StaticClass()
    );
}

float ACXPawn::TakeDamage(
    float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    const float ActualDamage = Super::TakeDamage(
        DamageAmount,
        DamageEvent,
        EventInstigator,
        DamageCauser
    );

    if (!HasAuthority() || !IsValid(DamageCauser))
    {
        return ActualDamage;
    }

    ACXPawn* Attacker = Cast<ACXPawn>(DamageCauser);

    if (!IsValid(Attacker) || Attacker == this)
    {
        return ActualDamage;
    }

    ACXPlayerState* VictimState = GetPlayerState<ACXPlayerState>();
    ACXPlayerState* AttackerState =
        Attacker->GetPlayerState<ACXPlayerState>();

    if (!IsValid(VictimState) || !IsValid(AttackerState))
    {
        return ActualDamage;
    }

    if (VictimState->RemoveCoins(5))
    {
        AttackerState->AddCoins(5);
    }

    return ActualDamage;
}

void ACXPawn::MulticastAttackEffect_Implementation(
    FVector Start,
    FVector End,
    bool bHit
)
{
    if (!bHit)
    {
        return;
    }

    if (AttackHitEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            AttackHitEffect,
            End
        );
    }

    if (AttackHitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            AttackHitSound,
            End
        );
    }
}

#undef LOCTEXT_NAMESPACE

