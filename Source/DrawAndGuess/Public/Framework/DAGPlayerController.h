#pragma once

#include "CoreMinimal.h"
#include "DrawAndGuessType.h"
#include "DAGChatMessage.h"
#include "DAGPlayerController.generated.h"

class ADrawingActionManager;
class ADrawingCanvas;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveChatMessageSignature, FDAGChatMessage, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveStringSignature, FString, Word);

UCLASS()
class ADAGPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	/** Return the client to main menu gracefully. */
	UFUNCTION(Client, Reliable)
	void ClientReturnToMainMenuWithReason(EReturnToMainMenuReason::Type Reason);

	/** Send just before the host start game. */
	UFUNCTION(Client, Reliable)
	void ClientPreStartGame();

	UFUNCTION(Client, Reliable)
	void ClientUploadPlayerInfo();

	UFUNCTION(Server, Reliable)
	void ServerReceivePlayerInfo(const FString& InPlayerName);

	void HandleReturnToMainMenu();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PlayerController")
	void ServerSetPlayerName(const FString& PlayerName);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PlayerController")
	void ServerSetLobbyState(EPlayerLobbyState NewState);

	void SetDrawingActionManager(ADrawingActionManager* NewDrawingActionManager);

	ADrawingActionManager* GetDrawingActionManager() const { return DrawingActionManager; }

	void SetDrawingActionManagerToBrush();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddAcceptCanvas(ADrawingCanvas* NewCanvas);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRemoveAcceptCanvas(ADrawingCanvas* TargetCanvas);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PlayerController")
	void ServerSendChatMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(FDAGChatMessage Message);

	UFUNCTION(Client, Reliable)
	void ClientReceiveWord(const FString& Word);

	UFUNCTION(Client, Reliable)
	void ClientReceiveHint(const FString& Hint);

	UFUNCTION(Client, Reliable)
	void ClientReceiveCandidateWords(const TArray<FString>& CandidateWords);

	UFUNCTION(BlueprintCallable, Category="PlayerController")
	void ClearDrawingActions();

	UFUNCTION(Client, Reliable)
	void ClientReceiveLeaderboard(const TArray<FString>& NameArray, const TArray<int32>& ScoreArray);

	UFUNCTION(Client, Reliable)
	void ClientReceiveGameRestart();

	UFUNCTION(Client, Reliable)
	void ClientReceiveCorrectWord(const FString& Word);

	UFUNCTION(Client, Reliable)
	void ClientReceivePlayerRoundStart(class ADAGPlayerState* RoundPlayer);

	UFUNCTION(Exec)
	void ExecCheckAllPlayerId();

	UFUNCTION(Exec)
	void ExecSetPlayerDrawOnCanvas(int32 PlayerId, FString CanvasName, bool bCanDraw);

protected:
	virtual void OnPossess(APawn* PawnToPossess) override;

	void CleanupSessionOnReturnMain();

	UFUNCTION()
	void OnRep_DrawingActionManager();

	UFUNCTION(BlueprintImplementableEvent, Meta=(DisplayName="OnReceiveCandidateWords"))
	void K2_OnReceiveCandidateWords(const TArray<FString>& CandidateWords);

	UFUNCTION(BlueprintImplementableEvent, Meta=(DisplayName="OnReceiveLeaderboardData"))
	void K2_OnReceiveLeaderboardData(const TArray<FString>& NameArray, const TArray<int32>& ScoreArray);

	UFUNCTION(BlueprintImplementableEvent, Meta=(DisplayName="OnReceiveGameRestart"))
	void K2_OnReceiveGameRestart();

	UFUNCTION(BlueprintImplementableEvent, Meta=(DisplayName="OnReceiveCorrectWord"))
	void K2_OnReceiveCorrectWord(const FString& CorrectWord);

	UFUNCTION(BlueprintImplementableEvent, Meta=(DisplayName="OnReceivePlayerRoundStart"))
	void K2_OnReceivePlayerRoundStart(class ADAGPlayerState* RoundPlayer);

public:
	UPROPERTY(BlueprintAssignable, Category="PlayerController")
	FOnReceiveChatMessageSignature OnReceiveChatMessage;

	UPROPERTY(BlueprintAssignable, Category="PlayerController")
	FOnReceiveStringSignature OnReceiveWord;

	UPROPERTY(BlueprintAssignable, Category="PlayerController")
	FOnReceiveStringSignature OnReceiveHint;

protected:

	UPROPERTY(ReplicatedUsing=OnRep_DrawingActionManager, VisibleAnywhere)
	ADrawingActionManager* DrawingActionManager;
};
