// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiderTypes.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GenericOctreePublic.h"
#include "RiderMovementComponent.generated.h"

/**
 * Component for Tron-like movement
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class TRONTEST_API URiderMovementComponent : public UPawnMovementComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float MoveSpeed;

    UPROPERTY(BlueprintReadonly, Transient, Replicated)
    FReplicatedMove LastMove;

    UPROPERTY(BlueprintReadonly, Transient, Replicated)
    TArray<FVector> Path;

    UPROPERTY(BlueprintReadonly, Transient, Replicated)
    float AbilityCooldown;

    UPROPERTY()
    class URiderMovementAbility* CurrentAbility;

public:
    URiderMovementComponent();

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

public:
    UFUNCTION(BlueprintCallable)
    void TurnLeft();

    UFUNCTION(BlueprintCallable)
    void TurnRight();

    UFUNCTION(BlueprintCallable)
    void ChangeDirection(EMoveDirection InDirection);

    bool ActivateAbility(TSubclassOf<class URiderMovementAbility> AbilityClass);

    UFUNCTION(BlueprintCallable)
    void ApplyMove(FReplicatedMove Move);

protected:
    UFUNCTION(reliable, server, WithValidation)
    void ServerChangeDirection(EMoveDirection NewDirection);

    bool WillChangeDirection(EMoveDirection InDirection);
    FVector DirectionToVector(EMoveDirection InDirection);
    EMoveDirection OppositeDirection(EMoveDirection InDirection);
};
