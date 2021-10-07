// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/DrawingBrush.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionManager.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "Engine/ActorChannel.h"
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

	BrushSize = 100;
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

		BrushMaterialInstance->SetTextureParameterValue(TEXT("BrushTexture"), BrushTexture);
	}
}

void ADrawingBrush::ServerBroadcastInputEvents_Implementation(const TArray<FDrawingInputEvent>& InputEvents)
{
	// Broadcast all input events to all clients
	MulticastReceiveInputEvents(InputEvents);
}

void ADrawingBrush::MulticastReceiveInputEvents_Implementation(const TArray<FDrawingInputEvent>& InputEvents)
{
	//if ((GetNetMode() == NM_Client && GetLocalRole() != ROLE_AutonomousProxy))
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
			if (DrawingCanvas)
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
			CurrentDrawAction = DrawingActionManager->CreateDrawingAction(DrawingActionType);
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
	if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
	{
		if (ADrawingActionManager* DrawingActionManager = GetDrawingActionManager())
		{
			DrawingActionManager->Undo();
		}
	}
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

// Called every frame
void ADrawingBrush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CachedRole != GetLocalRole())
	{
		CachedRole = GetLocalRole();

		UE_LOG(LogInit, Log, TEXT("[%s] Change local role to %s"), *GetPathName(), *UEnum::GetValueAsString<ENetRole>(CachedRole));
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
			UE_LOG(LogInit, Log, TEXT("[Brush] Process received input: %d %s"), GetNetMode(), *GetName());
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
}

void ADrawingBrush::SetBrushTexture(UTexture2D* NewTexture)
{
	if (BrushTexture == NewTexture)
		return;

	BrushTexture = NewTexture;
	BrushMaterialInstance->SetTextureParameterValue(TEXT("BrushTexture"), BrushTexture);
}

void ADrawingBrush::SetBrushColor(FLinearColor NewColor)
{
	if (BrushColor == NewColor)
		return;

	BrushColor = NewColor;
	BrushMaterialInstance->SetVectorParameterValue(TEXT("BrushColor"), BrushColor);
}

void ADrawingBrush::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParam;
	SharedParam.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ADrawingBrush, MyDrawingActionManager, SharedParam);
}
