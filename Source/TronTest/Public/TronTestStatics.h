// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RiderPlayerState.h"
#include "TronTestStatics.generated.h"

/**
 * 
 */
UCLASS()
class TRONTEST_API UTronTestStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, Category = "Tron Test", meta = (WorldContext = "WorldContextObject"))
    static class ARiderPlayerController* GetRiderPlayerController(UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "Tron Test", meta = (WorldContext = "WorldContextObject"))
    static class ARiderPlayerState* GetRiderPlayerState(UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "Tron Test", meta = (WorldContext = "WorldContextObject"))
    static class ARiderPlayerState* GetRiderPlayerStateWithColor(UObject* WorldContextObject, EPlayerColor Color);

    UFUNCTION(BlueprintPure, Category = "Tron Test", meta = (WorldContext = "WorldContextObject"))
    static class ARiderGameStateBase* GetRiderGameState(UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "Tron Test", meta = (WorldContext = "WorldContextObject"))
    static int32 GetMaxPlayers(UObject* WorldContextObject);
};
