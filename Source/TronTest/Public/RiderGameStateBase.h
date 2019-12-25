// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiderTypes.h"
#include "GameFramework/GameStateBase.h"
#include "RiderGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMatchStateChangedDelegate, EMatchState, NewState);

UCLASS()
class TRONTEST_API ARiderGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
    // Score count needed to win the match
    UPROPERTY(EditDefaultsOnly, BlueprintReadonly)
    int ScoreToWin;

    // Time between rounds in seconds
    UPROPERTY(EditDefaultsOnly, BlueprintReadonly)
    float RestartDelay;

    // Time for countdown in seconds
    UPROPERTY(EditDefaultsOnly, BlueprintReadonly)
    float CountdownTime;

    UPROPERTY(BlueprintAssignable)
    FMatchStateChangedDelegate OnMatchStateChanged;

public:
    ARiderGameStateBase();

    EMatchState GetMatchState() const { return MatchState; }
    void SetMatchState(EMatchState NewState);

protected:
    UFUNCTION()
    void OnRep_MatchState();

protected:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MatchState, Transient)
    EMatchState MatchState;
};
