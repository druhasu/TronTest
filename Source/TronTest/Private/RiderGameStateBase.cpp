// Fill out your copyright notice in the Description page of Project Settings.


#include "RiderGameStateBase.h"
#include "RiderPlayerState.h"
#include "Rider.h"
#include "UnrealNetwork.h"
#include "Engine\World.h"

ARiderGameStateBase::ARiderGameStateBase()
{
    MatchState = EMatchState::WaitingForPlayers;
}

void ARiderGameStateBase::SetMatchState(EMatchState NewState)
{
    if (HasAuthority())
    {
        MatchState = NewState;

        // server need to call it manually
        OnRep_MatchState();
    }
}

void ARiderGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARiderGameStateBase, MatchState);
}

void ARiderGameStateBase::OnRep_MatchState()
{
    if (MatchState == EMatchState::Playing)
    {
        // notify all pawns to start Moving
        for (auto PlayerState : PlayerArray)
        {
            auto Rider = PlayerState->GetPawn<ARider>();
            Rider->StartMoving();
        }
    }

    OnMatchStateChanged.Broadcast(MatchState);
}