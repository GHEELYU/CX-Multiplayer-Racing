
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CXPlayerController.generated.h"

class UInputMappingContext;
class ACXPawn;
class UCXUI;
class UUserWidget;

UCLASS(abstract)
class CX_API ACXPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    bool bUseSteeringWheelControls = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input,
        meta = (EditCondition = "bUseSteeringWheelControls"))
    UInputMappingContext* SteeringWheelInputMappingContext;

    UPROPERTY(Replicated)
    TObjectPtr<ACXPawn> VehiclePawn;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
    TSubclassOf<UCXUI> VehicleUIClass;

    UPROPERTY()
    TObjectPtr<UCXUI> VehicleUI;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat")
    TSubclassOf<UUserWidget> ChatWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> ChatWidget;

    bool bChatOpen = false;

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    virtual void OnPossess(APawn* InPawn) override;

    void OpenChat();

    UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
    void OnChatOpened();

public:
    virtual void Tick(float Delta) override;

    UFUNCTION(BlueprintCallable, Category = "Chat")
    void SubmitChatMessage(const FString& Message);

    UFUNCTION(Server, Reliable)
    void ServerSubmitChatMessage(const FString& Message);

    UFUNCTION(Client, Reliable)
    void ClientReceiveChatMessage(const FString& Message);

    UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
    void OnChatMessageReceived(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "Chat")
    void CloseChat();
};

