// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider.h"
#include "RiderMovementComponent.h"
#include "RiderPlayerState.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

const FName MoveUpName("MoveUp");
const FName MoveDownName("MoveDown");
const FName MoveRightName("MoveRight");
const FName MoveLeftName("MoveLeft");

// Sets default values
ARider::ARider()
{
    PrimaryActorTick.bCanEverTick = false;

    Movement = CreateDefaultSubobject<URiderMovementComponent>(TEXT("MovementComp"));
    Movement->bAutoActivate = false;
}

void ARider::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARider, AbilityActivationsCount);
}

void ARider::StartMoving()
{
    Movement->Activate();

    if (HasAuthority())
    {
        // find initial direction
        auto CheckDir = [this](FVector Direction) { return (GetActorForwardVector() | Direction) > 0.7f; };

        FReplicatedMove Move;
        Move.Direction = EMoveDirection::Up;
        Move.MoveSpeed = Movement->MoveSpeed;
        Move.StartLocation = GetActorLocation();
        Move.StartTime = GetWorld()->TimeSeconds;

        if (CheckDir(FVector::RightVector))
            Move.Direction = EMoveDirection::Right;
        else if (CheckDir(FVector::DownVector))
            Move.Direction = EMoveDirection::Down;
        else if (CheckDir(FVector::LeftVector))
            Move.Direction = EMoveDirection::Left;

        Movement->ApplyMove(Move);
    }
}

// Called to bind functionality to input
void ARider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction(MoveUpName, IE_Pressed, this, &ARider::MoveUp);
    PlayerInputComponent->BindAction(MoveDownName, IE_Pressed, this, &ARider::MoveDown);
    PlayerInputComponent->BindAction(MoveRightName, IE_Pressed, this, &ARider::MoveRight);
    PlayerInputComponent->BindAction(MoveLeftName, IE_Pressed, this, &ARider::MoveLeft);
}

void ARider::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    auto RiderPlayerState = Cast<ARiderPlayerState>(GetPlayerState());
    if (RiderPlayerState != nullptr)
    {
        PlayerStateChanged(RiderPlayerState);
    }
}

void ARider::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    auto RiderPlayerState = Cast<ARiderPlayerState>(GetPlayerState());
    if (RiderPlayerState != nullptr)
    {
        PlayerStateChanged(RiderPlayerState);
    }
}

bool ARider::GetLastSegment(FVector& Start, FVector& End)
{
    if (Movement->Path.Num() > 0)
    {
        Start = Movement->Path.Last();
        End = GetActorLocation();

        return true;
    }

    return false;
}

void ARider::ActivateAbility()
{
    if (AbilityActivationsCount > 0)
    {
        if (Role < ROLE_Authority)
        {
            ServerActivateAbility();
        }
        else
        {
            if (Movement->ActivateAbility(MovementAbilityClass))
            {
                --AbilityActivationsCount;
                OnRep_AbilityActivationsCount();
            }
        }
    }
}

void ARider::OnRep_AbilityActivationsCount()
{
    AbilityActivationsCountChanged.Broadcast(AbilityActivationsCount);
}

bool ARider::ServerActivateAbility_Validate()
{
    return true;
}

void ARider::ServerActivateAbility_Implementation()
{
    ActivateAbility();
}

void ARider::MoveUp()
{
    Movement->ChangeDirection(EMoveDirection::Up);
}

void ARider::MoveDown()
{
    Movement->ChangeDirection(EMoveDirection::Down);
}

void ARider::MoveRight()
{
    Movement->ChangeDirection(EMoveDirection::Right);
}

void ARider::MoveLeft()
{
    Movement->ChangeDirection(EMoveDirection::Left);
}
