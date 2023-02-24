// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class PINGPONG_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* PlayerScore;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* EnemyScore;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* BallScore;

	UPROPERTY()
	class APingPongPlayerController* OwningPlayer = nullptr;
	UPROPERTY()
	class APingPongBall* Ball;
	UPROPERTY()
	TArray<class APingPongGate*> Gates;

protected:
	void Init();
	
public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
