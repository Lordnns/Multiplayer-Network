// Fill out your copyright notice in the Description page of Project Settings.


#include "TagUtilityClass.h"
#include "GameplayTagsManager.h"

void UTagUtilityClass::GetAllTagsUnderCategory(FGameplayTag CategoryTag, FGameplayTagContainer& OutTags)
{
	const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	OutTags = TagsManager.RequestGameplayTagChildren(CategoryTag);
}

