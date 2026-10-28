
#include "RaceGameState.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

ARaceGameState::ARaceGameState()
{
    bReplicates = true;
}

void ARaceGameState::SetRacePhase(ERacePhase NewPhase)
{
    if (HasAuthority())
    {
        CurrentPhase = NewPhase;
    }
}

void ARaceGameState::SetRemainingTime(int32 NewTime)
{
    if (HasAuthority())
    {
        RemainingTime = FMath::Max(0, NewTime);
    }
}

void ARaceGameState::SetWinners(
    const TArray<APlayerState*>& NewWinners)
{
    if (HasAuthority())
    {
        Winners = NewWinners;
    }
}

void ARaceGameState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARaceGameState, CurrentPhase);
    DOREPLIFETIME(ARaceGameState, RemainingTime);
    DOREPLIFETIME(ARaceGameState, Winners);
}

