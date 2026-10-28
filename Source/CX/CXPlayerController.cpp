
#include "CXPlayerController.h"
#include "CXPawn.h"
#include "CXUI.h"

#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"

void ACXPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (!IsLocalController())
    {
        return;
    }

    VehicleUI = CreateWidget<UCXUI>(this, VehicleUIClass);
    check(VehicleUI);
    VehicleUI->AddToViewport();

    if (ChatWidgetClass)
    {
        ChatWidget = CreateWidget<UUserWidget>(
            this, ChatWidgetClass
        );

        if (ChatWidget)
        {
            ChatWidget->AddToViewport();
        }
    }
}

void ACXPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<
        UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);

        if (bUseSteeringWheelControls &&
            SteeringWheelInputMappingContext)
        {
            Subsystem->AddMappingContext(
                SteeringWheelInputMappingContext, 1
            );
        }
    }

    InputComponent->BindKey(
        EKeys::Enter,
        IE_Pressed,
        this,
        &ACXPlayerController::OpenChat
    );
}

void ACXPlayerController::OpenChat()
{
    if (!IsLocalController() || !ChatWidget || bChatOpen)
    {
        return;
    }

    bChatOpen = true;
    bShowMouseCursor = true;

    // WBP_Chat의 FocusChatInput에서
    // UI 입력 모드와 키보드 포커스를 설정한다.
    OnChatOpened();
}

void ACXPlayerController::CloseChat()
{
    if (!IsLocalController())
    {
        return;
    }

    bChatOpen = false;

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);

    bShowMouseCursor = false;
}

void ACXPlayerController::SubmitChatMessage(
    const FString& Message)
{
    FString CleanMessage = Message;
    CleanMessage.TrimStartAndEndInline();

    if (CleanMessage.IsEmpty())
    {
        CloseChat();
        return;
    }

    CleanMessage = CleanMessage.Left(200);

    ServerSubmitChatMessage(CleanMessage);
    CloseChat();
}

void ACXPlayerController::ServerSubmitChatMessage_Implementation(
    const FString& Message)
{
    FString CleanMessage = Message;
    CleanMessage.TrimStartAndEndInline();
    CleanMessage = CleanMessage.Left(200);

    if (CleanMessage.IsEmpty())
    {
        return;
    }

    const FString PlayerName =
        PlayerState ? PlayerState->GetPlayerName() : TEXT("Player");

    const FString FullMessage =
        FString::Printf(
            TEXT("%s: %s"),
            *PlayerName,
            *CleanMessage
        );

    if (UWorld* World = GetWorld())
    {
        for (FConstPlayerControllerIterator It =
            World->GetPlayerControllerIterator(); It; ++It)
        {
            if (ACXPlayerController* PC =
                Cast<ACXPlayerController>(It->Get()))
            {
                PC->ClientReceiveChatMessage(FullMessage);
            }
        }
    }
}

void ACXPlayerController::ClientReceiveChatMessage_Implementation(
    const FString& Message)
{
    OnChatMessageReceived(Message);
}

void ACXPlayerController::Tick(float Delta)
{
    Super::Tick(Delta);

    if (IsValid(VehiclePawn) && IsValid(VehicleUI))
    {
        VehicleUI->UpdateSpeed(
            VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed()
        );

        VehicleUI->UpdateGear(
            VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear()
        );
    }
}

void ACXPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    VehiclePawn = CastChecked<ACXPawn>(InPawn);
}

void ACXPlayerController::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, VehiclePawn);
}

