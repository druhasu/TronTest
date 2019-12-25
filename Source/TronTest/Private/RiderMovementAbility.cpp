// Fill out your copyright notice in the Description page of Project Settings.

#include "RiderMovementAbility.h"
#include "RiderMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

void URiderMovementAbility::BeginPlay_Implementation()
{
}

EAbilityTickResult URiderMovementAbility::Tick_Implementation(float DeltaTime)
{
    return EAbilityTickResult::Default;
}

void URiderMovementAbility::EndPlay_Implementation()
{
}

FReplicatedMove URiderMovementAbility::MakeDirectionMove(EMoveDirection Direction)
{
    auto Move = Movement->LastMove;
    Move.StartLocation = Movement->GetPawnOwner()->GetActorLocation();
    Move.Direction = Direction;
    Move.StartTime = Movement->GetWorld()->TimeSeconds;
    return Move;
}

FReplicatedMove URiderMovementAbility::MakeSpeedMove(float Speed)
{
    auto Move = Movement->LastMove;
    Move.StartLocation = Movement->GetPawnOwner()->GetActorLocation();
    Move.MoveSpeed = Speed;
    Move.StartTime = Movement->GetWorld()->TimeSeconds;
    return Move;
}