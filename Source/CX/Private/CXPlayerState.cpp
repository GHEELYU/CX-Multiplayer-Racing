
#include "CXPlayerState.h"
#include "Net/UnrealNetwork.h"

ACXPlayerState::ACXPlayerState()
{
    bReplicates = true;
}

void ACXPlayerState::AddCoins(int32 Amount)
{
    if (!HasAuthority() || Amount <= 0)
    {
        return;
    }

    Coins += Amount;
}

bool ACXPlayerState::RemoveCoins(int32 Amount)
{
    if (!HasAuthority() || Amount <= 0 || Coins < Amount)
    {
        return false;
    }

    Coins -= Amount;
    return true;
}

void ACXPlayerState::AddBonusMaxSpeed(float Amount)
{
    if (!HasAuthority() || Amount <= 0.0f)
    {
        return;
    }

    BonusMaxSpeed += Amount;
}

void ACXPlayerState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACXPlayerState, Coins, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ACXPlayerState, BonusMaxSpeed, COND_OwnerOnly);
}
