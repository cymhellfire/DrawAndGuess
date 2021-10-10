// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/BasicFileLibrary.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"

bool UBasicFileLibrary::OpenFileDialog(const FString& Title, const FString& DefaultPath, const FString& FileExtension,
                                       TArray<FString>& FilePaths)
{
	if (GEngine)
	{
		if (GEngine->GameViewport)
		{
			void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform)
			{
				uint32 SelectionFlag = EFileDialogFlags::None;
				return DesktopPlatform->OpenFileDialog(ParentWindowHandle, Title, DefaultPath, TEXT(""), FileExtension, SelectionFlag, FilePaths);
			}
		}
	}

	return false;
}
