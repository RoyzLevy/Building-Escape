// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"

#define OUTPARAM

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrabber::SetPhysicsHandle()
{
	// Check for physics handle component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>(); // find the component.
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics handle component on %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grabber player ready"));
	}
}

void UGrabber::SetInputComponent()
{
	// Check for input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>(); // find the component.
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No input component on %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Input component available"));
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);	   // bind input to function call of this instance.
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release); // bind input to function call of this instance.
	}
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	SetPhysicsHandle();
	SetInputComponent();
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab!"));
	// only ray-cast when the key is pressed.
	FVector LineTraceEnd = GetLineTraceEnd();

	// try reaching any actors with physics body collision channel set.
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	// if there is, attach the physics handle
	UPrimitiveComponent *ComponentToGrab = HitResult.GetComponent();
	if (HitResult.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, LineTraceEnd);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Release!"));

	// release the physics handle.
	if (PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->ReleaseComponent();
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// if physics handle attached, move holded object.
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	// get player viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUTPARAM PlayerViewPointLocation, OUTPARAM PlayerViewPointRotation);

	FVector LineTraceEnd = GetLineTraceEnd();

	// ray-cast
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner()); // simple collision, ignoring the owner.
	GetWorld()->LineTraceSingleByObjectType(
		OUTPARAM HitResult,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams);

	AActor *ActorHit = HitResult.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Event: %s"), *ActorHit->GetName());
	}

	return HitResult;
}

FVector UGrabber::GetLineTraceEnd() const
{
	// get player viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUTPARAM PlayerViewPointLocation, OUTPARAM PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * PlayerReach;
	return LineTraceEnd;
}