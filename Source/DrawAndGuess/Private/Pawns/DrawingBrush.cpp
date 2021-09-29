// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/DrawingBrush.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionManager.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
ADrawingBrush::ADrawingBrush()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BrushSize = 100;
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
	case DIE_Pressed:
		// Setup the parent canvas
		CurrentDrawAction->SetParentCanvas(InputEvent.Canvas);
		// Pass in input location to action
		CurrentDrawAction->AddInputPoint(InputEvent.InputLocation);
		break;
	case DIE_Released:
		if (CurrentDrawAction != nullptr)
		{
			CurrentDrawAction->StopInput(InputEvent.InputLocation);

			// Submit current action
			if (UDrawingActionManager* DrawingActionManager = GetDrawingActionManager())
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

UDrawingActionManager* ADrawingBrush::GetDrawingActionManager() const
{
	if (APlayerController* MyPlayerController = Cast<APlayerController>(GetController()))
	{
		return MyPlayerController->GetLocalPlayer()->GetSubsystem<UDrawingActionManager>();
	}

	return nullptr;
}

void ADrawingBrush::OnDrawButtonPressed()
{
	bDrawPressed = true;

	// Create the action
	if (UDrawingActionManager* DrawingActionManager = GetDrawingActionManager())
	{
		CurrentDrawAction = DrawingActionManager->CreateDrawingAction(DrawingActionType);
		CurrentDrawAction->CopyBrushSettings(this);
	}

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

	ADrawingCanvas* DrawingCanvas = CurrentDrawAction->GetParentCanvas();
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
		if (UDrawingActionManager* DrawingActionManager = MyPlayerController->GetLocalPlayer()->GetSubsystem<UDrawingActionManager>())
		{
			DrawingActionManager->Undo();
		}
	}
}

void ADrawingBrush::OnCursorAxisChanged(float Input)
{
	if (FMath::Abs(Input) > 0.01f && bDrawPressed)
	{
		ADrawingCanvas* DrawingCanvas = CurrentDrawAction->GetParentCanvas();
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

	// Process all pending input events
	if (InputEventQueue.Num() > 0)
	{
		for (FDrawingInputEvent InputEvent : InputEventQueue)
		{
			HandleDrawingInputEvent(InputEvent);
		}

		// Clear the queue
		InputEventQueue.Empty();
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

