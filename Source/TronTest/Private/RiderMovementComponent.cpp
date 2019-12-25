// Fill out your copyright notice in the Description page of Project Settings.


#include "RiderMovementComponent.h"
#include "Rider.h"
#include "RiderPlayerState.h"
#include "RiderPlayerController.h"
#include "TronTestGameModeBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "RiderMovementAbility.h"

URiderMovementComponent::URiderMovementComponent()
{
    MoveSpeed = 500;

    SetIsReplicated(true);
}

void URiderMovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URiderMovementComponent, LastMove);
    DOREPLIFETIME(URiderMovementComponent, Path);
    DOREPLIFETIME(URiderMovementComponent, AbilityCooldown);
}

void URiderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    EAbilityTickResult TickResult = EAbilityTickResult::Default;
    if (CurrentAbility != nullptr)
    {
        CurrentAbility->Duration -= DeltaTime;
        TickResult = CurrentAbility->Tick(DeltaTime);

        if (CurrentAbility->Duration <= 0)
        {
            CurrentAbility->EndPlay();
            CurrentAbility = nullptr;
        }
    }

    if (LastMove.MoveSpeed > 0 && TickResult != EAbilityTickResult::OverrideMovement)
    {
        auto Controller = Cast<ARiderPlayerController>(GetWorld()->GetFirstPlayerController());

        auto ServerTimeDiff = Controller ? Controller->ServerTimeDiff : 0;
        auto TimeSinceTurn = GetWorld()->TimeSeconds - LastMove.StartTime + ServerTimeDiff;
        auto DirectionVector = DirectionToVector(LastMove.Direction);
        
        auto OldLocation = PawnOwner->GetActorLocation();
        auto NewLocation = LastMove.StartLocation + DirectionVector * TimeSinceTurn * LastMove.MoveSpeed;
        auto NewRotation = DirectionVector.ToOrientationQuat();

        PawnOwner->SetActorLocationAndRotation(NewLocation, NewRotation);

        auto GameMode = GetWorld()->GetAuthGameMode<ATronTestGameModeBase>();
        if (GameMode != nullptr)
        {
            auto Rider = Cast<ARider>(GetPawnOwner());
            GameMode->CheckMove(Rider, OldLocation, NewLocation, Path.Num());
        }
    }

    if (AbilityCooldown > 0)
    {
        AbilityCooldown -= DeltaTime;
    }
}

void URiderMovementComponent::TurnLeft()
{
    uint8 Direction = (uint8)LastMove.Direction;
    Direction = (Direction + (uint8)EMoveDirection::MAX - 1) % (uint8)EMoveDirection::MAX;
    ChangeDirection((EMoveDirection)Direction);
}

void URiderMovementComponent::TurnRight()
{
    uint8 Direction = (uint8)LastMove.Direction;
    Direction = (Direction + 1) % (uint8)EMoveDirection::MAX;
    ChangeDirection((EMoveDirection)Direction);
}

void URiderMovementComponent::ChangeDirection(EMoveDirection InDirection)
{
    if (bIsActive && WillChangeDirection(InDirection))
    {
        if (PawnOwner->Role < ROLE_Authority)
        {
            ServerChangeDirection(InDirection);
        }
        else
        {
            auto Location = PawnOwner->GetActorLocation();

            if (Path.Num() > 0)
            {
                auto GameMode = GetWorld()->GetAuthGameMode<ATronTestGameModeBase>();
                GameMode->AddSegment(Cast<ARider>(PawnOwner), Path.Last(), Location, Path.Num());
            }

            LastMove.StartLocation = Location;
            LastMove.Direction = InDirection;
            LastMove.StartTime = GetWorld()->TimeSeconds;

            Path.Add(Location);
        }
    }
}

bool URiderMovementComponent::ActivateAbility(TSubclassOf<class URiderMovementAbility> AbilityClass)
{
    if (bIsActive && *AbilityClass && PawnOwner->Role == ROLE_Authority && AbilityCooldown <= 0)
    {
        if (CurrentAbility != nullptr)
        {
            CurrentAbility->EndPlay();
        }

        CurrentAbility = NewObject<URiderMovementAbility>(this, AbilityClass);
        CurrentAbility->Movement = this;
        CurrentAbility->BeginPlay();

        AbilityCooldown = CurrentAbility->Cooldown + CurrentAbility->Duration;
        return true;
    }

    return false;
}

void URiderMovementComponent::ApplyMove(FReplicatedMove Move)
{
    if (bIsActive && PawnOwner->Role == ROLE_Authority)
    {
        LastMove = Move;
        Path.Add(Move.StartLocation);
    }
}

bool URiderMovementComponent::ServerChangeDirection_Validate(EMoveDirection InDirection)
{
    return InDirection < EMoveDirection::MAX;
}

void URiderMovementComponent::ServerChangeDirection_Implementation(EMoveDirection InDirection)
{
    ChangeDirection(InDirection);
}

bool URiderMovementComponent::WillChangeDirection(EMoveDirection InDirection)
{
    auto CurrentDirection = LastMove.Direction;
    return CurrentDirection != InDirection && CurrentDirection != OppositeDirection(InDirection);
}

FVector URiderMovementComponent::DirectionToVector(EMoveDirection InDirection)
{
    switch (InDirection)
    {
        case EMoveDirection::Up:
            return FVector::ForwardVector;

        case EMoveDirection::Right:
            return FVector::RightVector;

        case EMoveDirection::Down:
            return FVector::BackwardVector;

        case EMoveDirection::Left:
            return FVector::LeftVector;

        default:
            return FVector::ZeroVector;
    }
}

EMoveDirection URiderMovementComponent::OppositeDirection(EMoveDirection InDirection)
{
    return (EMoveDirection)((uint8)InDirection ^ 2);
}
