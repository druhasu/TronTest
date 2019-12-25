// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiderTypes.h"
#include "GameFramework/PlayerController.h"
#include "RiderPlayerController.generated.h"

UCLASS()
class TRONTEST_API ARiderPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    float ServerTimeDiff = 0.f;

    bool bIsLoser = false;

public:
    UFUNCTION(BlueprintCallable)
    void SyncServerTime();

    void LoseRound();
    void WinRound(class ARiderPlayerController* Winner, bool bIsLastRound);

    class ARider* GetRider();

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent)
    void NotifyLose();

    UFUNCTION(BlueprintImplementableEvent)
    void NotifyWin(EPlayerColor WinnerColor, bool bIsLastRound);

protected:
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRequestTime();

    UFUNCTION(Client, Reliable)
    void ClientResponseTime(float ServerTime);

    UFUNCTION(Client, Reliable)
    void ClientLoseRound();

    UFUNCTION(Client, Reliable)
    void ClientWinRound(FEndRoundInfo EndInfo);

private:
    float LastRequestTime = 0.f;
};
