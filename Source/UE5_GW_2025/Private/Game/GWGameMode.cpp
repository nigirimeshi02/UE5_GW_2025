// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GWGameMode.h"

AGWGameMode::AGWGameMode()
{
	PlayerLife = 3;
}

void AGWGameMode::DecreaseLife()
{
	PlayerLife--;

	if (PlayerLife <= 0)
	{
		OnGameOver();
	}
}
