// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/DrawingBrush.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionManager.h"
#include "DrawingSystem/DrawingAction_Pencil.h"
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

void ADrawingBrush::DrawBrush(UTextureRenderTarget2D* RenderTarget, FVector2D Location)
{
	// Initial drawing requirements
	UCanvas* DrawingCanvas;
	FVector2D DrawingCanvasSize;
	FDrawToRenderTargetContext NewContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RenderTarget, DrawingCanvas, DrawingCanvasSize, NewContext);

	// Calculate draw location
	FVector2D DrawLocation = DrawingCanvasSize * Location - BrushSize / 2;
	FVector2D DrawBrushSize(BrushSize);

	// Draw texture
	DrawingCanvas->K2_DrawMaterial(BrushMaterialInstance, DrawLocation, DrawBrushSize, FVector2D::ZeroVector);

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, NewContext);
}

void ADrawingBrush::OnDrawButtonPressed()
{
	bDrawing = true;

	// Create the action
	CurrentDrawAction = Cast<UDrawingAction_Pencil>(UDrawingActionManager::GetInstance()->CreateDrawingAction(DAT_Pencil));
	CurrentDrawAction->CopyBrushSetting(this);

	// Temp code
	for (TActorIterator<ADrawingCanvas> Iter(GetWorld()); Iter; ++Iter)
	{
		CurrentDrawAction->SetParentCanvas(*Iter);
	}
}

void ADrawingBrush::OnDrawButtonReleased()
{
	bDrawing = false;
}

void ADrawingBrush::OnUndoPressed()
{
	UDrawingActionManager::GetInstance()->Undo();
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

	if (bDrawing)
	{
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

						// Draw brush
						//DrawBrush(DrawingCanvas->GetCanvasRenderTarget(), UVCoordinate);
						CurrentDrawAction->AppendDrawPoint(UVCoordinate);
					}
				}
			}
		}
	}
}

// Called to bind functionality to input
void ADrawingBrush::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Draw"), EInputEvent::IE_Pressed, this, &ADrawingBrush::OnDrawButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Draw"), EInputEvent::IE_Released, this, &ADrawingBrush::OnDrawButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Undo"), EInputEvent::IE_Pressed, this, &ADrawingBrush::OnUndoPressed);
}

void ADrawingBrush::SetBrushTexture(UTexture2D* NewTexture)
{
	if (BrushTexture == NewTexture)
		return;

	BrushTexture = NewTexture;
	BrushMaterialInstance->SetTextureParameterValue(TEXT("BrushTexture"), BrushTexture);
}

