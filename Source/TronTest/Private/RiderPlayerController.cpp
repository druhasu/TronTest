// Fill out your copyright notice in the Description page of Project Settings.


#include "RiderPlayerController.h"
#include "Rider.h"
#include "RiderMovementComponent.h"
#include "RiderPlayerState.h"
#include "Engine\World.h"

void ARiderPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SyncServerTime();
}

void ARiderPlayerController::SyncServerTime()
{
    if (!HasAuthority())
    {
        LastRequestTime = GetWorld()->TimeSeconds;
        ServerRequestTime();
    }
}

void ARiderPlayerController::LoseRound()
{
    GetRider()->Movement->Deactivate();
    bIsLoser = true;

    if (HasAuthority())
    {
        ClientLoseRound();
    }
}

void ARiderPlayerController::WinRound(ARiderPlayerController* Winner, bool bIsLastRound)
{
    GetRider()->Movement->Deactivate();

    if (HasAuthority())
    {
        if (this == Winner)
        {
            PlayerState->Score += 1;
            // we need to call this on server
            PlayerState->OnRep_Score();
        }

        FEndRoundInfo Info;
        Info.WinnerColor = Winner->GetPlayerState<ARiderPlayerState>()->Color;
        Info.bIsLastRound = bIsLastRound;

        ClientWinRound(Info);
    }
}

ARider* ARiderPlayerController::GetRider()
{
    return Cast<ARider>(GetPawn());
}

bool ARiderPlayerController::ServerRequestTime_Validate()
{
    return true;
}

void ARiderPlayerController::ServerRequestTime_Implementation()
{
    ClientResponseTime(GetWorld()->TimeSeconds);
}

void ARiderPlayerController::ClientResponseTime_Implementation(float ServerTime)
{
    auto CurrentTime = GetWorld()->TimeSeconds;
    auto RoundTripTime = CurrentTime - LastRequestTime;
    auto RiderPlayerState = Cast<ARiderPlayerState>(PlayerState);

    ServerTimeDiff = ServerTime - CurrentTime + RoundTripTime * 0.5f;

    if (RiderPlayerState)
    {
        RiderPlayerState->ServerTimeDiff = ServerTime - CurrentTime + RoundTripTime * 0.5f;
    }
}

void ARiderPlayerController::ClientLoseRound_Implementation()
{
    NotifyLose();
}

void ARiderPlayerController::ClientWinRound_Implementation(FEndRoundInfo EndInfo)
{
    NotifyWin(EndInfo.WinnerColor, EndInfo.bIsLastRound);
}