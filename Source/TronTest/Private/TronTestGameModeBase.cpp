// Fill out your copyright notice in the Description page of Project Settings.


#include "TronTestGameModeBase.h"
#include "Rider.h"
#include "RiderPlayerController.h"
#include "RiderGameStateBase.h"
#include "MapBounds.h"
#include "GameFramework\GameStateBase.h"
#include "EngineUtils.h"
#include "GameFramework\PlayerStart.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "TronTestStatics.h"

ATronTestGameModeBase::ATronTestGameModeBase()
    : Segments(FVector::ZeroVector, 1000)
{
    GameStateClass = ARiderGameStateBase::StaticClass();
}

void ATronTestGameModeBase::InitGameState()
{
    Super::InitGameState();

    RiderGameState = GetGameState<ARiderGameStateBase>();
    RiderGameState->SetMatchState(EMatchState::WaitingForPlayers);

    TActorIterator<AMapBounds> Iter(GetWorld());
    MapExtents = Iter ? Iter->GetBounds().BoxExtent.GetAbsMax() : 1000;
}

FString ATronTestGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
    auto RiderPlayerState = Cast<ARiderPlayerState>(NewPlayerController->PlayerState);

    // find color that is not yet occupied
    for (uint32 i = 0; i < (uint32)EPlayerColor::MAX; ++i)
    {
        bool IsFree = true;
        for (auto PlayerState : GameState->PlayerArray)
        {
            if (PlayerState != RiderPlayerState && Cast<ARiderPlayerState>(PlayerState)->Color == (EPlayerColor)i)
            {
                IsFree = false;
                break;
            }
        }

        if (IsFree)
        {
            RiderPlayerState->Color = (EPlayerColor)i;
            break;
        }
    }

    Players.Add(Cast<ARiderPlayerController>(NewPlayerController));

    return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

AActor* ATronTestGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
    auto RiderPlayerState = Cast<ARiderPlayerState>(Player->PlayerState);
    auto ChosenSpotNamePtr = StartSpotNames.Find(RiderPlayerState->Color);

    if (ChosenSpotNamePtr != nullptr)
    {
        // find player start with matching tag
        for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
        {
            if (It->PlayerStartTag == *ChosenSpotNamePtr)
            {
                return *It;
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No start spot found for color %d"), RiderPlayerState->Color);
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}

void ATronTestGameModeBase::PostLogin(class APlayerController* Player)
{
    Super::PostLogin(Player);

    int32 NumPlayers = UTronTestStatics::GetMaxPlayers(this);

    if (GameState->PlayerArray.Num() == NumPlayers)
    {
        InitiateNewRound();
    }
}

void ATronTestGameModeBase::Logout(AController* Exiting)
{
    Players.Remove(Cast<ARiderPlayerController>(Exiting));
    if (!GetWorld()->bIsTearingDown && RiderGameState->GetMatchState() == EMatchState::Playing)
    {
        // if there are just two players alive and one of them is exiting then other one will become a winner
        TryFindWinner();
    }    

    Super::Logout(Exiting);
}

void ATronTestGameModeBase::AddSegment(class ARider* Rider, const FVector& Start, const FVector& End, int32 Index)
{
    FSegment Segment;
    auto CE = GetCenterAndExtents(Start, End);
    Segment.Center = CE.Get<0>();
    Segment.Extents = CE.Get<1>();
    Segment.Owner = Rider;
    Segment.Index = Index;

    Segments.AddElement(Segment);
}

void ATronTestGameModeBase::CheckMove(class ARider* Rider, const FVector& Start, const FVector& End, int32 Index)
{
    auto GetBox = [](const FVector& Start, const FVector& End)
    {
        auto CE = GetCenterAndExtents(Start, End);
        return FBoxCenterAndExtent(CE.Get<0>(), CE.Get<1>());
    };

    bool bIsLoser = false;

    // first check against map bounds
    if (End.GetAbsMax() > MapExtents)
    {
        bIsLoser = true;
    }

    // check against octree of completed segments
    auto Box = GetBox(Start, End);
    if (!bIsLoser)
    {
        FSegmentsOctree::TConstElementBoxIterator<> Iter(Segments, Box);
        while (Iter.HasPendingElements())
        {
            auto& Segment = Iter.GetCurrentElement();
            // do not register collision with our previous segment
            if (Segment.Owner != Rider || Segment.Index != Index - 1)
            {
                bIsLoser = true;
                break;
            }
            Iter.Advance();
        }
    }

    // check against other players' last segments
    if (!bIsLoser)
    {
        for (auto OtherController : Players)
        {
            auto OtherRider = OtherController->GetRider();

            if (OtherRider == Rider)
                continue;

            FVector OtherStart, OtherEnd;
            OtherRider->GetLastSegment(OtherStart, OtherEnd);
            auto OtherBox = GetBox(OtherStart, OtherEnd);

            if (Intersect(Box, OtherBox))
            {
                bIsLoser = true;
                break;
            }
        }
    }

    // tell player he is loser
    if (bIsLoser)
    {
        auto RiderController = Rider->GetController<ARiderPlayerController>();
        RiderController->LoseRound();

        TryFindWinner();
    }
}

void ATronTestGameModeBase::InitiateNewRound()
{
    RiderGameState->SetMatchState(EMatchState::Countdown);

    auto& TimerManager = GetWorld()->GetTimerManager();
    TimerManager.SetTimer(ActiveTimer, this, &ATronTestGameModeBase::CountdownCompleted, RiderGameState->CountdownTime, false);
}

void ATronTestGameModeBase::InitiateRestart()
{
    RiderGameState->SetMatchState(EMatchState::AfterRound);

    auto& TimerManager = GetWorld()->GetTimerManager();
    TimerManager.SetTimer(ActiveTimer, this, &ATronTestGameModeBase::AfterRoundCompleted, RiderGameState->RestartDelay, false);
}

void ATronTestGameModeBase::TryFindWinner()
{
    ARiderPlayerController* PotentialWinner = nullptr;

    for (auto RiderController : Players)
    {
        if (!RiderController->bIsLoser)
        {
            if (PotentialWinner == nullptr)
            {
                PotentialWinner = RiderController;
            }
            else
            {
                // it is second player alive, no winner yet
                return;
            }
        }
    }

    if (PotentialWinner != nullptr)
    {
        // we use Score-1, because actual Score will be incremented later
        bool bIsLastRound = PotentialWinner->PlayerState->Score >= RiderGameState->ScoreToWin - 1;

        for (auto RiderController : Players)
        {
            RiderController->WinRound(PotentialWinner, bIsLastRound);
        }

        if (bIsLastRound)
        {
            // we found match winner, transition to final state
            RiderGameState->SetMatchState(EMatchState::AfterMatch);
        }
        else
        {
            // start another round
            InitiateRestart();
        }
    }
}

void ATronTestGameModeBase::CountdownCompleted()
{
    Segments = FSegmentsOctree(FVector::ZeroVector, MapExtents);
    RiderGameState->SetMatchState(EMatchState::Playing);
}

void ATronTestGameModeBase::AfterRoundCompleted()
{
    for (auto RiderController : Players)
    {
        auto Rider = RiderController->GetRider();

        RiderController->bIsLoser = false;
        RiderController->UnPossess();
        Rider->Destroy();

        RestartPlayer(RiderController);
    }

    InitiateNewRound();
}

TTuple<FVector, FVector> ATronTestGameModeBase::GetCenterAndExtents(const FVector& Start, const FVector& End)
{
    auto Min = Start.ComponentMin(End);
    auto Max = Start.ComponentMax(End);
    auto HalfSize = (Max - Min) * 0.5f;

    return MakeTuple(Min + HalfSize, HalfSize);
}