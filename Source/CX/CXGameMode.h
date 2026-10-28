
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CXGameMode.generated.h"

class APlayerStart;
class ARaceGameState;

UCLASS(MinimalAPI)
class ACXGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACXGameMode();

protected:
    virtual AActor* ChoosePlayerStart_Implementation(
        AController* Player
    ) override;

    virtual void PostLogin(
        APlayerController* NewPlayer
    ) override;

private:
    bool IsPlayerStartAvailable(
        const APlayerStart* PlayerStart
    ) const;

    void UpdateRaceTimer();
    void StartRace();
    void FinishRace();

private:
    UPROPERTY(EditDefaultsOnly, Category = "Vehicle Spawn")
    FVector VehicleSpawnCheckHalfExtent =
        FVector(250.0, 120.0, 80.0);

    UPROPERTY(EditDefaultsOnly, Category = "Vehicle Spawn")
    FVector VehicleSpawnCheckOffset =
        FVector(0.0, 0.0, 80.0);

    UPROPERTY(EditDefaultsOnly, Category = "Race")
    int32 WaitingDuration = 10;

    UPROPERTY(EditDefaultsOnly, Category = "Race")
    int32 PlayingDuration = 60;

    FTimerHandle RaceTimerHandle;

    bool bRaceTimerStarted = false;
};

