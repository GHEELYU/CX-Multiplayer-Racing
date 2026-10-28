
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RaceGameState.generated.h"

UENUM(BlueprintType)
enum class ERacePhase : uint8
{
    Waiting UMETA(DisplayName = "Waiting"),
    Playing UMETA(DisplayName = "Playing"),
    Finished UMETA(DisplayName = "Finished")
};

UCLASS()
class CX_API ARaceGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ARaceGameState();

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
    ERacePhase CurrentPhase = ERacePhase::Waiting;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
    int32 RemainingTime = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
    TArray<APlayerState*> Winners;

    void SetRacePhase(ERacePhase NewPhase);
    void SetRemainingTime(int32 NewTime);
    void SetWinners(const TArray<APlayerState*>& NewWinners);

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;
};

