
#include "CXGameMode.h"
#include "CXPlayerController.h"
#include "CXPlayerState.h"
#include "RaceGameState.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"

ACXGameMode::ACXGameMode()
{
    PlayerControllerClass = ACXPlayerController::StaticClass();
    PlayerStateClass = ACXPlayerState::StaticClass();

    // 게임 상태 및 타이머를 모든 클라이언트에 복제
    GameStateClass = ARaceGameState::StaticClass();
}

void ACXGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);


    if (NewPlayer)
    {
        const FString PlayerName =
            NewPlayer->PlayerState
            ? NewPlayer->PlayerState->GetPlayerName()
            : TEXT("Player");

        const FString JoinMessage =
            FString::Printf(TEXT("[SYSTEM] %s joined the game."),
                *PlayerName);

        for (FConstPlayerControllerIterator It =
            GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (ACXPlayerController* PC =
                Cast<ACXPlayerController>(It->Get()))
            {
                PC->ClientReceiveChatMessage(JoinMessage);
            }
        }
    }


    if (!bRaceTimerStarted)
    {
        bRaceTimerStarted = true;

        if (ARaceGameState* RaceState =
            GetGameState<ARaceGameState>())
        {
            RaceState->SetRacePhase(ERacePhase::Waiting);
            RaceState->SetRemainingTime(WaitingDuration);
        }

        GetWorldTimerManager().SetTimer(
            RaceTimerHandle,
            this,
            &ACXGameMode::UpdateRaceTimer,
            1.0f,
            true
        );
    }
}

void ACXGameMode::UpdateRaceTimer()
{
    ARaceGameState* RaceState =
        GetGameState<ARaceGameState>();

    if (!IsValid(RaceState))
    {
        return;
    }

    const int32 NewTime = RaceState->RemainingTime - 1;
    RaceState->SetRemainingTime(NewTime);

    if (NewTime > 0)
    {
        return;
    }

    if (RaceState->CurrentPhase == ERacePhase::Waiting)
    {
        StartRace();
    }
    else if (RaceState->CurrentPhase == ERacePhase::Playing)
    {
        FinishRace();
    }
}

void ACXGameMode::StartRace()
{
    ARaceGameState* RaceState =
        GetGameState<ARaceGameState>();

    if (!IsValid(RaceState))
    {
        return;
    }

    RaceState->SetRacePhase(ERacePhase::Playing);
    RaceState->SetRemainingTime(PlayingDuration);
}

void ACXGameMode::FinishRace()
{
    ARaceGameState* RaceState =
        GetGameState<ARaceGameState>();

    if (!IsValid(RaceState))
    {
        return;
    }

    GetWorldTimerManager().ClearTimer(RaceTimerHandle);

    int32 HighestCoins = MIN_int32;
    TArray<APlayerState*> WinningPlayers;

    // 서버의 모든 플레이어 점수 비교
    for (APlayerState* PlayerState : RaceState->PlayerArray)
    {
        ACXPlayerState* CoinPlayerState =
            Cast<ACXPlayerState>(PlayerState);

        if (!IsValid(CoinPlayerState))
        {
            continue;
        }

        const int32 Coins = CoinPlayerState->GetCoins();

        if (Coins > HighestCoins)
        {
            HighestCoins = Coins;
            WinningPlayers.Empty();
            WinningPlayers.Add(CoinPlayerState);
        }
        else if (Coins == HighestCoins)
        {
            // 최고 점수가 같으면 공동 승자
            WinningPlayers.Add(CoinPlayerState);
        }
    }

    RaceState->SetWinners(WinningPlayers);
    RaceState->SetRemainingTime(0);
    RaceState->SetRacePhase(ERacePhase::Finished);
}

AActor* ACXGameMode::ChoosePlayerStart_Implementation(
    AController* Player
)
{
    TArray<APlayerStart*> AvailablePlayerStarts;

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* PlayerStart = *It;

        if (IsPlayerStartAvailable(PlayerStart))
        {
            AvailablePlayerStarts.Add(PlayerStart);
        }
    }

    checkf(
        AvailablePlayerStarts.IsEmpty() == false,
        TEXT("There is no available Vehicle PlayerStart.")
    );

    const int32 RandomIndex =
        FMath::RandHelper(AvailablePlayerStarts.Num());

    return Cast<AActor>(AvailablePlayerStarts[RandomIndex]);
}

bool ACXGameMode::IsPlayerStartAvailable(
    const APlayerStart* PlayerStart
) const
{
    if (IsValid(PlayerStart) == false ||
        IsValid(GetWorld()) == false)
    {
        return false;
    }

    const FTransform PlayerStartTransform =
        PlayerStart->GetActorTransform();

    const FVector CheckLocation =
        PlayerStartTransform.TransformPosition(
            VehicleSpawnCheckOffset
        );

    const FQuat CheckRotation =
        PlayerStartTransform.GetRotation();

    const FCollisionShape CheckShape =
        FCollisionShape::MakeBox(
            VehicleSpawnCheckHalfExtent
        );

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);

    const FCollisionQueryParams QueryParams(
        SCENE_QUERY_STAT(VehiclePlayerStartCheck),
        false
    );

    const bool bVehicleExists =
        GetWorld()->OverlapAnyTestByObjectType(
            CheckLocation,
            CheckRotation,
            ObjectQueryParams,
            CheckShape,
            QueryParams
        );

    return !bVehicleExists;
}

