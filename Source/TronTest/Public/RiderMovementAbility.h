// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiderTypes.h"
#include "RiderMovementAbility.generated.h"

UCLASS(Blueprintable)
class TRONTEST_API URiderMovementAbility : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadonly, EditDefaultsOnly)
    float Duration = 1;

    UPROPERTY(BlueprintReadonly, EditDefaultsOnly)
    float Cooldown = 1;

    UPROPERTY(BlueprintReadonly, Transient)
    class URiderMovementComponent* Movement;

public:
    UFUNCTION(BlueprintNativeEvent)
    void BeginPlay();

    UFUNCTION(BlueprintNativeEvent)
    EAbilityTickResult Tick(float DeltaTime);

    UFUNCTION(BlueprintNativeEvent)
    void EndPlay();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FReplicatedMove MakeDirectionMove(EMoveDirection Direction);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FReplicatedMove MakeSpeedMove(float Speed);
};