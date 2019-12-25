// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiderTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerColor : uint8
{
    Red,
    Green,
    Blue,
    Yellow,

    MAX,
};

UENUM(BlueprintType)
enum class EMatchState : uint8
{
    // Waiting for all players to connect to server
    WaitingForPlayers,
    // Countdown before starting round
    Countdown,
    // Actual gameplay state
    Playing,
    // Pause between end of one round and start of next one
    AfterRound,
    // Match is finished
    AfterMatch,
};

UENUM(BlueprintType)
enum class EMoveDirection : uint8
{
    Up = 0,
    Right,
    Down,
    Left,

    MAX
};

USTRUCT(BlueprintType)
struct TRONTEST_API FReplicatedMove
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadonly)
    FVector_NetQuantize StartLocation;

    UPROPERTY(BlueprintReadonly)
    EMoveDirection Direction;

    UPROPERTY(BlueprintReadonly)
    float MoveSpeed;

    UPROPERTY(BlueprintReadonly)
    float StartTime;
};

USTRUCT()
struct TRONTEST_API FEndRoundInfo
{
    GENERATED_BODY()

public:
    UPROPERTY()
    EPlayerColor WinnerColor;

    UPROPERTY()
    uint8 bIsLastRound : 1;
};

UENUM()
enum class EAbilityTickResult : uint8
{
    // Apply regular move logic
    Default = 0,

    // Ability already moved actor, no need to apply regular move logic
    OverrideMovement,
};