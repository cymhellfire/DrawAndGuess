// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawingSystem/DrawingSystemCommon.h"
#include "GameFramework/Pawn.h"
#include "DrawingBrush.generated.h"

class UDrawingActionBase;
class ADrawingCanvas;
class ADrawingActionManager;

UCLASS()
class DRAWANDGUESS_API ADrawingBrush : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrawingBrush();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * Broadcast an array of input events through server.
	 *
	 * @param InputEvents			Input events to synchronize
	 */
	UFUNCTION(Server, Reliable)
	void ServerBroadcastInputEvents(const TArray<FDrawingInputEvent>& InputEvents);

	/**
	 * Send a series of input events to all connected clients.
	 *
	 * @param InputEvents			Input events to send.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastReceiveInputEvents(const TArray<FDrawingInputEvent>& InputEvents);

	/**
	 * Get the drawing point location in given canvas.
	 * Or get the drawing point location and drawing canvas.
	 *
	 * @param DesiredCanvas			Given a canvas that drawing point belong to.
	 * (Pass in nullptr will be modified to detected canvas.)
	 * @return	The UV coordinate of drawing point in canvas space.
	 * (NullVector when no drawing point match requirement.)
	 */
	FVector2D GetDrawingPoint(ADrawingCanvas*& DesiredCanvas);

	void HandleDrawingInputEvent(FDrawingInputEvent InputEvent);

	ADrawingActionManager* GetDrawingActionManager();

	void OnDrawButtonPressed();
	void OnDrawButtonReleased();

	void OnUndoPressed();

	void OnCursorAxisChanged(float Input);

	UFUNCTION()
	void OnRep_DrawingActionManager();

public:
	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="DrawingBrush")
	void SetBrushSize(float NewSize) { BrushSize = NewSize; }

	float GetBrushSize() const { return BrushSize; }

	void SetBrushTexture(UTexture2D* NewTexture);

	UTexture2D* GetBrushTexture() const { return BrushTexture; }

	UFUNCTION(BlueprintCallable, Category="DrawingBrush")
	void SetBrushColor(FLinearColor NewColor);

	FLinearColor GetBrushColor() const { return BrushColor; }

	UFUNCTION(Server, Reliable)
	void ServerSetDrawingActionManager(ADrawingActionManager* NewDrawingActionManager);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	UMaterial* BrushMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	UTexture2D* BrushTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	float BrushSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	FLinearColor BrushColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	TEnumAsByte<EDrawingActionType> DrawingActionType;

	UPROPERTY(VisibleAnywhere, Transient)
	bool bLocal;

	UPROPERTY(ReplicatedUsing=OnRep_DrawingActionManager, Transient, VisibleAnywhere)
	ADrawingActionManager* MyDrawingActionManager;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BrushMaterialInstance;

	uint8 bDrawPressed : 1;

	ENetRole CachedRole;

	UPROPERTY()
	UDrawingActionBase* CurrentDrawAction;

	// Local input queue
	TArray<FDrawingInputEvent> InputEventQueue;

	// Received input queue from server
	TArray<FDrawingInputEvent> ReceivedInputEventQueue;
};
