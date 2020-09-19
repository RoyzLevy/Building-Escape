// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

#define OUTPARAM

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("AActor %s has OpenDoor, but no PressurePlate available"), *GetOwner()->GetName());
	}

	FindAudioComponent();
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Missing audio component"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetTotalMassOfActors() > MassToOpenDoors)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	// With Lerp
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	// sound of door
	if (AudioComponent)
	{
		AudioComponent->Play();
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	// With Lerp
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorClosureSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	// sound of door
	if (AudioComponent)
	{
		AudioComponent->Play();
	}
}

float UOpenDoor::GetTotalMassOfActors() const
{
	float TotalMass = 0.f;

	// find all overlapping actors
	TArray<AActor *> OverlappingActors;
	if (PressurePlate)
	{
		PressurePlate->GetOverlappingActors(OUTPARAM OverlappingActors);

		// add actors mass
		for (AActor *Actor : OverlappingActors)
		{
			TotalMass += Actor->GetComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}
	return TotalMass;
}