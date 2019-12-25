// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiderTypes.h"
#include "GameFramework/PlayerState.h"
#include "RiderPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChangedDelegate, int, Score);

UCLASS()
class TRONTEST_API ARiderPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    float ServerTimeDiff = 0.f;

    UPROPERTY(BlueprintReadOnly, Replicated)
    EPlayerColor Color;

    UPROPERTY(BlueprintAssignable)
    FScoreChangedDelegate OnScoreChanged;

public:
    virtual void OnRep_Score() override;
};
