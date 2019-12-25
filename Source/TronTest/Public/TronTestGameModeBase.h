// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RiderPlayerState.h"
#include "SegmentsOctree.h"
#include "TronTestGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class TRONTEST_API ATronTestGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TMap<EPlayerColor, FName> StartSpotNames;

public:
    ATronTestGameModeBase();

    virtual void InitGameState() override;

    virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
    virtual void PostLogin(APlayerController* Player);
    virtual void Logout(AController* Exiting);

    void AddSegment(class ARider* Rider, const FVector& Start, const FVector& End, int32 Index);
    void CheckMove(class ARider* Rider, const FVector& Start, const FVector& End, int32 Index);

protected:
    void InitiateNewRound();
    void InitiateRestart();

private:
    void TryFindWinner();
    void CountdownCompleted();
    void AfterRoundCompleted();
    static TTuple<FVector, FVector> GetCenterAndExtents(const FVector& Start, const FVector& End);

    TArray<class ARiderPlayerController*> Players;
    class ARiderGameStateBase* RiderGameState;
    float MapExtents;
    FSegmentsOctree Segments;
    FTimerHandle ActiveTimer;
};
