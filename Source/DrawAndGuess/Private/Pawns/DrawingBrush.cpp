// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/DrawingBrush.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionManager.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "Engine/Canvas.h"
#include "Framework/DAGPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

// Sets default values
ADrawingBrush::ADrawingBrush()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BrushSize = 50;
	bDrawPressed = false;

	bReplicates = true;
	CachedRole = ROLE_MAX;
}

// Called when the game starts or when spawned
void ADrawingBrush::BeginPlay()
{
	Super::BeginPlay();

	// Create material instance
	if (IsValid(BrushMaterial))
	{
		UObject* MIDOuter = GetWorld();
		BrushMaterialInstance = UMaterialInstanceDynamic::Create(BrushMaterial, MIDOuter);
		if (MIDOuter == nullptr)
		{
			BrushMaterialInstance->SetFlags(RF_Transient);
		}

		BrushMaterialInstance->SetTextureParameterValue(TEXT("BrushTexture"), GetBrushTexture());
		BrushMaterialInstance->SetVectorParameterValue(TEXT("BrushColor"), GetBrushColor());
	}
}

void ADrawingBrush::ServerBroadcastInputEvents_Implementation(const TArray<FDrawingInputEvent>& InputEvents)
{
	// Broadcast all input events to all clients
	MulticastReceiveInputEvents(InputEvents);
}

void ADrawingBrush::MulticastReceiveInputEvents_Implementation(const TArray<FDrawingInputEvent>& InputEvents)
{
	if (!bLocal)
	{
		// Append input event into received queue
		ReceivedInputEventQueue.Append(InputEvents);
	}
}

FVector2D ADrawingBrush::GetDrawingPoint(ADrawingCanvas*& DesiredCanvas)
{
	FVector2D DrawingPoint = NullVector;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		FHitResult CursorRayHitResult;
		if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, CursorRayHitResult))
		{
			ADrawingCanvas* DrawingCanvas = Cast<ADrawingCanvas>(CursorRayHitResult.Actor);
			if (DrawingCanvas && AcceptCanvas.Contains(DrawingCanvas))
			{
				FCollisionQueryParams QueryParams {TEXT("DrawBrushTrace"), true, this};
				QueryParams.bReturnFaceIndex = true;
				if (GetWorld()->LineTraceSingleByChannel(CursorRayHitResult, CursorRayHitResult.TraceStart, CursorRayHitResult.TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams))
				{
					FVector2D UVCoordinate;
					UGameplayStatics::FindCollisionUV(CursorRayHitResult, 0, UVCoordinate);

					// Change the canvas pointer if no canvas is passed in
					if (DesiredCanvas == nullptr)
					{
						DesiredCanvas = DrawingCanvas;
						DrawingPoint = UVCoordinate;
					}
					else
					{
						// Only return the drawing point if canvas matched
						if (DrawingCanvas == DesiredCanvas)
						{
							DrawingPoint = UVCoordinate;
						}
					}
				}
			}
		}
	}

	return DrawingPoint;
}

void ADrawingBrush::HandleDrawingInputEvent(FDrawingInputEvent InputEvent)
{
	switch (InputEvent.InputType)
	{
	case DIE_CreateAction:
		// Create the action
		if (ADrawingActionManager* DrawingActionManager = GetDrawingActionManager())
		{
			// Create a local drawing action to draw with
			CurrentDrawAction = DrawingActionManager->CreateDrawingAction(GetDrawingActionType());
			CurrentDrawAction->CopyBrushSettings(this);
		}
		break;
	case DIE_Pressed:
		if (CurrentDrawAction != nullptr)
		{
			// Setup the parent canvas
			CurrentDrawAction->SetParentCanvas(InputEvent.Canvas);
			// Pass in input location to action
			CurrentDrawAction->AddInputPoint(InputEvent.InputLocation);
		}
		break;
	case DIE_Released:
		if (CurrentDrawAction != nullptr)
		{
			CurrentDrawAction->StopInput(InputEvent.InputLocation);

			// Submit current action
			if (ADrawingActionManager* DrawingActionManager = GetDrawingActionManager())
			{
				DrawingActionManager->SubmitDrawingAction(CurrentDrawAction);
			}

			// Release the current action
			CurrentDrawAction = nullptr;
		}
		break;
	case DIE_Dragged:
		if (CurrentDrawAction != nullptr)
		{
			// The parent canvas might be null
			if (!CurrentDrawAction->HasCanvasToDraw())
			{
				CurrentDrawAction->SetParentCanvas(InputEvent.Canvas);
			}

			CurrentDrawAction->UpdatePreviewPoint(InputEvent.InputLocation);
		}
		break;
	default: ;
	}
}

ADrawingActionManager* ADrawingBrush::GetDrawingActionManager()
{
	if (ADAGPlayerController* MyPlayerController = Cast<ADAGPlayerController>(GetController()))
	{
		return MyPlayerController->GetDrawingActionManager();
	}

	return MyDrawingActionManager;
}

void ADrawingBrush::OnDrawButtonPressed()
{
	bDrawPressed = true;

	InputEventQueue.Add(FDrawingInputEvent{
		NullVector,
		EDrawingInputType::DIE_CreateAction,
		nullptr,
	});

	ADrawingCanvas* DrawingCanvas = nullptr;
	FVector2D DrawingPoint = GetDrawingPoint(DrawingCanvas);
	if (DrawingPoint != NullVector)
	{
		InputEventQueue.Add(FDrawingInputEvent{
			DrawingPoint,
			EDrawingInputType::DIE_Pressed,
			DrawingCanvas
		});
	}
}

void ADrawingBrush::OnDrawButtonReleased()
{
	bDrawPressed = false;

	ADrawingCanvas* DrawingCanvas = CurrentDrawAction ? CurrentDrawAction->GetParentCanvas() : nullptr;
	FVector2D DrawingPoint = GetDrawingPoint(DrawingCanvas);
	// if (DrawingPoint != NullVector)
	// {
		InputEventQueue.Add(FDrawingInputEvent{
			DrawingPoint,
			EDrawingInputType::DIE_Released,
			DrawingCanvas
		});
	//}
}

void ADrawingBrush::OnUndoPressed()
{
	ServerUndo();
}

void ADrawingBrush::OnCursorAxisChanged(float Input)
{
	if (FMath::Abs(Input) > 0.01f && bDrawPressed)
	{
		ADrawingCanvas* DrawingCanvas = CurrentDrawAction ? CurrentDrawAction->GetParentCanvas() : nullptr;
		FVector2D DrawingPoint = GetDrawingPoint(DrawingCanvas);
		if (DrawingPoint != NullVector)
		{
			InputEventQueue.Add(FDrawingInputEvent{
				DrawingPoint,
				EDrawingInputType::DIE_Dragged,
				DrawingCanvas
			});
		}
	}
}

void ADrawingBrush::OnRep_DrawingActionManager()
{
	UE_LOG(LogInit, Log, TEXT("OnRep_DrawingActionManager: %d %s"), GetNetMode(), *GetName());
}

void ADrawingBrush::ServerSetDrawingActionManager_Implementation(ADrawingActionManager* NewDrawingActionManager)
{
	MyDrawingActionManager = NewDrawingActionManager;

	MARK_PROPERTY_DIRTY_FROM_NAME(ADrawingBrush, MyDrawingActionManager, this);
}

void ADrawingBrush::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Show the mouse cursor
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		PlayerController->SetShowMouseCursor(true);
	}
}

void ADrawingBrush::UnPossessed()
{
	// Hide the mouse cursor when un-possessed
	if (APlayerController* CurrentController = Cast<APlayerController>(GetController()))
	{
		CurrentController->SetShowMouseCursor(false);
	}

	Super::UnPossessed();
}

void ADrawingBrush::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (APlayerController* CurrentController = Cast<APlayerController>(GetController()))
	{
		CurrentController->SetShowMouseCursor(true);
	}
}

// Called every frame
void ADrawingBrush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CachedRole != GetLocalRole())
	{
		CachedRole = GetLocalRole();
	}

	// Upload local input event queue
	if (GetLocalRole() > ROLE_SimulatedProxy && InputEventQueue.Num() > 0)
	{
		ServerBroadcastInputEvents(InputEventQueue);
	}

	// Process all pending input events locally
	if (InputEventQueue.Num() > 0)
	{
		for (FDrawingInputEvent InputEvent : InputEventQueue)
		{
			HandleDrawingInputEvent(InputEvent);
		}

		// Clear the queue
		InputEventQueue.Empty();
	}

	// Process all received input events
	if (!bLocal && ReceivedInputEventQueue.Num() > 0)
	{
		for (FDrawingInputEvent InputEvent : ReceivedInputEventQueue)
		{
			HandleDrawingInputEvent(InputEvent);
		}

		// Clear the queue
		ReceivedInputEventQueue.Empty();
	}
}

// Called to bind functionality to input
void ADrawingBrush::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Draw"), EInputEvent::IE_Pressed, this, &ADrawingBrush::OnDrawButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Draw"), EInputEvent::IE_Released, this, &ADrawingBrush::OnDrawButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Undo"), EInputEvent::IE_Pressed, this, &ADrawingBrush::OnUndoPressed);

	PlayerInputComponent->BindAxis(TEXT("CursorX"), this, &ADrawingBrush::OnCursorAxisChanged);
	PlayerInputComponent->BindAxis(TEXT("CursorY"), this, &ADrawingBrush::OnCursorAxisChanged);

	bLocal = true;

	ServerSetBrushSize(BrushSize);
	ServerSetBrushColor(BrushColor);
	ServerSetBrushTexture(BrushTexture);
}

void ADrawingBrush::AddAcceptCanvas(ADrawingCanvas* NewCanvas)
{
	if (IsValid(NewCanvas))
	{
		AcceptCanvas.AddUnique(NewCanvas);
	}
}

void ADrawingBrush::RemoveAcceptCanvas(ADrawingCanvas* TargetCanvas)
{
	if (IsValid(TargetCanvas))
	{
		// Submit current draw action immediately if parent canvas is removing
		if (CurrentDrawAction && CurrentDrawAction->GetParentCanvas() == TargetCanvas)
		{
			if (ADrawingActionManager* DrawingActionManager = GetDrawingActionManager())
			{
				DrawingActionManager->SubmitDrawingAction(CurrentDrawAction);
			}

			// Release the current action
			CurrentDrawAction = nullptr;
		}
		AcceptCanvas.Remove(TargetCanvas);
	}
}

void ADrawingBrush::SetBrushSize(float NewSize)
{
	if (BrushSize == NewSize)
		return;

	if (bLocal)
	{
		BrushSize = NewSize;
		ServerSetBrushSize(NewSize);
	}
}

void ADrawingBrush::ServerSetBrushSize_Implementation(float NewSize)
{
	SyncBrushSize = NewSize;

	MARK_PROPERTY_DIRTY_FROM_NAME(ADrawingBrush, SyncBrushSize, this);
}

void ADrawingBrush::SetBrushTexture(UTexture2D* NewTexture)
{
	if (BrushTexture == NewTexture)
		return;

	if (bLocal)
	{
		BrushTexture = NewTexture;
		if (IsValid(BrushMaterialInstance))
		{
			BrushMaterialInstance->SetTextureParameterValue(TEXT("BrushTexture"), BrushTexture);
		}

		ServerSetBrushTexture(NewTexture);
	}
}

void ADrawingBrush::SetBrushColor(FLinearColor NewColor)
{
	if (BrushColor == NewColor)
		return;

	if (bLocal)
	{
		BrushColor = NewColor;
		if (IsValid(BrushMaterialInstance))
		{
			BrushMaterialInstance->SetVectorParameterValue(TEXT("BrushColor"), BrushColor);
		}

		ServerSetBrushColor(NewColor);
	}
}

void ADrawingBrush::SetDrawingActionType(EDrawingActionType NewActionType)
{
	if (NewActionType == DrawingActionType)
		return;

	if (bLocal)
	{
		DrawingActionType = NewActionType;

		ServerSetDrawingActionType(NewActionType);
	}
}

void ADrawingBrush::ServerClear_Implementation()
{
	MulticastClear();
}

void ADrawingBrush::MulticastClear_Implementation()
{
	GetDrawingActionManager()->Clear();
}

void ADrawingBrush::ServerSetDrawingActionType_Implementation(EDrawingActionType NewActionType)
{
	SyncDrawingActionType = NewActionType;
	MARK_PROPERTY_DIRTY_FROM_NAME(ADrawingBrush, SyncDrawingActionType, this);
}

void ADrawingBrush::ServerUndo_Implementation()
{
	MulticastUndo();
}

void ADrawingBrush::MulticastUndo_Implementation()
{
	GetDrawingActionManager()->Undo();
}

void ADrawingBrush::ServerSetBrushTexture_Implementation(UTexture2D* NewTexture)
{
	SyncBrushTexture = NewTexture;

	MARK_PROPERTY_DIRTY_FROM_NAME(ADrawingBrush, SyncBrushTexture, this);
}

void ADrawingBrush::OnRep_BrushTexture()
{
	if (!bLocal)
	{
		UE_LOG(LogInit, Log, TEXT("OnRep_BrushTexture %s"), *GetPathName());
		if (IsValid(BrushMaterialInstance))
		{
			BrushMaterialInstance->SetTextureParameterValue(TEXT("BrushTexture"), SyncBrushTexture);
		}
	}
}

void ADrawingBrush::ServerSetBrushColor_Implementation(FLinearColor NewColor)
{
	SyncBrushColor = NewColor;

	MARK_PROPERTY_DIRTY_FROM_NAME(ADrawingBrush, SyncBrushColor, this);
}

void ADrawingBrush::OnRep_BrushColor()
{
	if (!bLocal)
	{
		UE_LOG(LogInit, Log, TEXT("OnRep_BrushColor %s"), *GetPathName());
		if (IsValid(BrushMaterialInstance))
		{
			BrushMaterialInstance->SetVectorParameterValue(TEXT("BrushColor"), BrushColor);
		}
	}
}

void ADrawingBrush::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParam;
	SharedParam.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ADrawingBrush, MyDrawingActionManager, SharedParam);
	DOREPLIFETIME_WITH_PARAMS(ADrawingBrush, SyncBrushColor, SharedParam);
	DOREPLIFETIME_WITH_PARAMS(ADrawingBrush, SyncBrushSize, SharedParam);
	DOREPLIFETIME_WITH_PARAMS(ADrawingBrush, SyncBrushTexture, SharedParam);
	DOREPLIFETIME_WITH_PARAMS(ADrawingBrush, SyncDrawingActionType, SharedParam);
}
