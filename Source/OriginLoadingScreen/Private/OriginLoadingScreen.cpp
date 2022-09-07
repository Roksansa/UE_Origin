// Copyright Epic Games, Inc. All Rights Reserved.

#include "OriginLoadingScreen.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MoviePlayer.h"
#include "Components/Widget.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"

#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::ProjectContentDir() / "ExternalContent/UI/Foundation/Fonts/Orbitron/Raw"/ RelativePath + TEXT(".ttf"), __VA_ARGS__ )

class SRPGLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRPGLoadingScreen) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		FSlateBrush* BGBrush = new FSlateBrush();
		FLinearColor RandomColor = FLinearColor(0.5f, 0.5f, 1.f, 1.f);

		BGBrush->TintColor = RandomColor;
		FSlateBrush* BGBrush2 = new FSlateBrush();
		
		FLinearColor Color = FLinearColor::Black;
		Color.A = 0.6f;
		BGBrush2->TintColor = Color;
		
		ChildSlot
			[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)	
				.BorderImage(BGBrush)
				.Padding(FMargin(32.f))
				[
					SNew(SImage)
					.Image(BGBrush2)
				]
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.Padding(FMargin(50.0f))
				.AutoHeight()
				[
					SNew(SThrobber)
					.Visibility(this, &SRPGLoadingScreen::GetLoadIndicatorVisibility)
				]
				+SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Loading"))
					.ColorAndOpacity(FLinearColor::Black)
					.Font(TTF_FONT("Orbitron-Black", 64))
				]
			]
		];
	}

private:
	EVisibility GetLoadIndicatorVisibility() const
	{
		return GetMoviePlayer()->IsLoadingFinished() ? EVisibility::Collapsed : EVisibility::Visible;
	}
};


class FOriginLoadingScreenModule : public IOriginLoadingScreenModule
{
public:
	virtual void StartupModule() override
	{
		if (IsMoviePlayerEnabled())
		{
			CreateScreen();
		}
	}
	
	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void StartInGameLoadingScreen() override
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 1.f;
		LoadingScreen.WidgetLoadingScreen = SNew(SRPGLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		
	}

	virtual void StopInGameLoadingScreen() override
	{
		GetMoviePlayer()->StopMovie();
	}

	virtual void CreateScreen()
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 1.f;
		LoadingScreen.WidgetLoadingScreen = SNew(SRPGLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
	
private:
	FDelegateHandle Handler;
};
#undef TTF_FONT
IMPLEMENT_GAME_MODULE(FOriginLoadingScreenModule, OriginLoadingScreen);
