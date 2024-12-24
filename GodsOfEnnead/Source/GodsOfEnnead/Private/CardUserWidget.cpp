// Fill out your copyright notice in the Description page of Project Settings.


#include "CardUserWidget.h"


int UCardUserWidget::GetNewAttack()
{
    return new_attack;
}

int UCardUserWidget::GetNewHp()
{
    return new_hp;
}

FString UCardUserWidget::GetNewNameCharacter()
{
    return new_name_character;
}

FSlateBrush UCardUserWidget::GetNewIconCharacter()
{
    FSlateBrush Brush;

    FString FormattedName = FormatCharacterName(new_name_character);

    FString PathToTexture = FString::Printf(TEXT("/Game/Assets/UI/Cards/characters/%s.%s"),
                                           *FormattedName, *FormattedName);

    UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *PathToTexture);
    if (Texture)
    {
        Brush.SetResourceObject(Texture);
        Brush.ImageSize = FVector2D(256.0f, 256.0f); // Размер иконки
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Не удалось загрузить текстуру для персонажа: %s"), *(new_name_character.ToLower()));
    }

    return Brush;
    // D:/Gods-of-Ennead/GodsOfEnnead/Content/Assets/UI/Cards/characters/bastet.uasset
}

FString UCardUserWidget::FormatCharacterName(const FString& CharacterName)
{
    FString FormattedName = CharacterName.ToLower();
    FormattedName.ReplaceInline(TEXT(" "), TEXT("_"));
    FormattedName.ReplaceInline(TEXT("'"), TEXT("_"));
    return FormattedName;
}

