
#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "CXPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class UNiagaraSystem;
class USoundBase;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

UCLASS(abstract)
class CX_API ACXPawn : public AWheeledVehiclePawn
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* FrontSpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FrontCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* BackSpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* BackCamera;

    TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* SteeringAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* ThrottleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* BrakeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* HandbrakeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* LookAroundAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* ToggleCameraAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* ResetVehicleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* FireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackRange = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    FVector AttackStartOffset = FVector(200.0f, 0.0f, 50.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Effects")
    TObjectPtr<UNiagaraSystem> AttackHitEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Effects")
    TObjectPtr<USoundBase> AttackHitSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed Buff")
    float BaseMaxSpeedKmh = 100.0f;

    bool bFrontCameraActive = false;

public:
    ACXPawn();

    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    virtual void Tick(float Delta) override;

    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser
    ) override;

protected:
    void Steering(const FInputActionValue& Value);
    void Throttle(const FInputActionValue& Value);
    void Brake(const FInputActionValue& Value);
    void StartBrake(const FInputActionValue& Value);
    void StopBrake(const FInputActionValue& Value);
    void StartHandbrake(const FInputActionValue& Value);
    void StopHandbrake(const FInputActionValue& Value);
    void LookAround(const FInputActionValue& Value);
    void ToggleCamera(const FInputActionValue& Value);
    void ResetVehicle(const FInputActionValue& Value);

    void Fire(const FInputActionValue& Value);

    UFUNCTION(Server, Reliable)
    void ServerFire();

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastAttackEffect(FVector Start, FVector End, bool bHit);

    UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
    void BrakeLights(bool bBraking);

public:
    FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
    FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
    FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
    FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};

