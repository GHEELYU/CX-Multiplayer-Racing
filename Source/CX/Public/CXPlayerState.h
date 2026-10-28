
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CXPlayerState.generated.h"

UCLASS()
class CX_API ACXPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ACXPlayerState();

    // 코인 추가 및 차감
    void AddCoins(int32 Amount);
    bool RemoveCoins(int32 Amount);

    // 추가 최대 속도 증가
    void AddBonusMaxSpeed(float Amount);

    int32 GetCoins() const { return Coins; }
    float GetBonusMaxSpeed() const { return BonusMaxSpeed; }

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

protected:
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Stats")
    int32 Coins = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Stats")
    float BonusMaxSpeed = 0.0f;
};
