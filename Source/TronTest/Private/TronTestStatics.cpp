// Fill out your copyright notice in the Description page of Project Settings.


#include "TronTestStatics.h"
#include "TronTestGameModeBase.h"
#include "RiderPlayerController.h"
#include "RiderPlayerState.h"
#include "RiderGameStateBase.h"
#include "Engine.h"
#include "OnlineSubsystemUtils.h"

ARiderPlayerController* UTronTestStatics::GetRiderPlayerController(UObject* WorldContextObject)
{
    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
    auto PlayerController = World->GetFirstPlayerController();

    return Cast<ARiderPlayerController>(PlayerController);
}

ARiderPlayerState* UTronTestStatics::GetRiderPlayerState(UObject* WorldContextObject)
{
    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
    auto PlayerController = World->GetFirstPlayerController();

    if (PlayerController)
    {
        return Cast<ARiderPlayerState>(PlayerController->PlayerState);
    }

    return nullptr;
}

ARiderPlayerState* UTronTestStatics::GetRiderPlayerStateWithColor(UObject* WorldContextObject, EPlayerColor Color)
{
    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
    auto GameState = World->GetGameState();

    for (auto PlayerState : GameState->PlayerArray)
    {
        auto RiderPlayerState = Cast<ARiderPlayerState>(PlayerState);
        if (RiderPlayerState->Color == Color)
        {
            return RiderPlayerState;
        }
    }

    return nullptr;
}

ARiderGameStateBase* UTronTestStatics::GetRiderGameState(UObject* WorldContextObject)
{
    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
    return World->GetGameState<ARiderGameStateBase>();
}

int UTronTestStatics::GetMaxPlayers(UObject* WorldContextObject)
{
    auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

    auto System = Online::GetSubsystem(World, NAME_None);
    auto Sessions = System->GetSessionInterface();
    auto CurrentSession = Sessions->GetNamedSession(NAME_GameSession);

    int NumPlayers = 4;

    // CurrentSession will be null in PIE Multiplayer
    if (CurrentSession != nullptr)
    {
        NumPlayers = CurrentSession->SessionSettings.NumPublicConnections;
    }

    return NumPlayers;
}