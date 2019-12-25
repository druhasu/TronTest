// Fill out your copyright notice in the Description page of Project Settings.


#include "RiderPlayerState.h"
#include "RiderPlayerController.h"
#include "GameFramework\Pawn.h"
#include "UnrealNetwork.h"

void ARiderPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARiderPlayerState, Color);
}

void ARiderPlayerState::OnRep_Score()
{
    OnScoreChanged.Broadcast(FMath::RoundToInt(Score));
}