// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Rider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntChangedDelegate, int, Value);

UCLASS()
class TRONTEST_API ARider : public APawn
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    class URiderMovementComponent* Movement;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    TSubclassOf<class URiderMovementAbility> MovementAbilityClass;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing=OnRep_AbilityActivationsCount)
    int32 AbilityActivationsCount;

    UPROPERTY(BlueprintAssignable)
    FIntChangedDelegate AbilityActivationsCountChanged;

public:
	ARider();

    void StartMoving();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    bool GetLastSegment(FVector& Start, FVector& End);

    UFUNCTION(BlueprintCallable)
    void ActivateAbility();

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void PlayerStateChanged(class ARiderPlayerState* InPlayerState);

    UFUNCTION()
    virtual void OnRep_AbilityActivationsCount();

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerActivateAbility();

private:
    void MoveUp();
    void MoveDown();
    void MoveRight();
    void MoveLeft();
};
