// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"

#include "Tankogeddon.h"
#include "TankPlayerController.h"
#include "Cannon.h"
// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank turret"));
	TurretMesh->SetupAttachment(BodyMesh);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ATankPawn::MoveForward(float AxisValue)
{
	TargetForwardAxisValue = AxisValue;
}
//void ATankPawn::MoveRight(float AxisValue)
//{
//	TargetRightAxisValue = AxisValue;
//}
// Called when the game starts or when spawned
void ATankPawn::SetupCannon(TSubclassOf<ACannon> InCannonClass)
{
	if (Cannon)
	{
		Cannon->Destroy();
		Cannon = nullptr;
	}

	FActorSpawnParameters Params;
	Params.Instigator = this;
	Params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ACannon>(InCannonClass, Params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}
void ATankPawn::SetNewCannon(TSubclassOf<ACannon> InCannonClass)
{
	if (CurrentCannon == CannonClass)
	{
		CurrentCannon = InCannonClass;
		CannonClass = InCannonClass;
		SetupCannon(CannonClass);
	}
	else
	{
		CurrentCannon = InCannonClass;
		CannonClassSecond = InCannonClass;
		SetupCannon(CannonClassSecond);
	}
}
void ATankPawn::ChangeCannon()
{
	if (CurrentCannon == CannonClass)
	{
		int32 current = Cannon->GetAmmunition();
		SetupCannon(CannonClassSecond);
		CurrentCannon = CannonClassSecond;
		Cannon->SetAmmunition(current);
	}
	else
	{
		int32 current = Cannon->GetAmmunition();
		SetupCannon(CannonClass);
		CurrentCannon = CannonClass;
		Cannon->SetAmmunition(current);
	}
}
void ATankPawn::Fire()
{

	if (Cannon)
	{
		Cannon->Fire();
	}
}
void ATankPawn::FireSpecial()
{
	if (Cannon)
	{
		Cannon->FireSpecial();
	}
}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	TankController = Cast<ATankPlayerController>(GetController());
	CurrentCannon = CannonClass;
	SetupCannon(CannonClass);
}
void ATankPawn::RotateRight(float AxisValue)
{
	TargetRightAxisValue = AxisValue;
}
// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Movemevt
	FVector CurrentLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	//FVector RightVector = GetActorRightVector();
	FVector MovePosition = CurrentLocation + ForwardVector * MoveSpeed * TargetForwardAxisValue * DeltaTime;
		//+ RightVector * MoveSpeed * TargetRightAxisValue * DeltaTime;
	SetActorLocation(MovePosition, true);

	//Rotation
	CurrentRightAxisValue = FMath::Lerp(CurrentRightAxisValue, TargetRightAxisValue, InterpolationKey);
	float yawRotation = RotationSpeed * CurrentRightAxisValue * DeltaTime;
	FRotator currentRotation = GetActorRotation();

	yawRotation = currentRotation.Yaw + yawRotation;

	FRotator newRotation = FRotator(0, yawRotation, 0);
	SetActorRotation(newRotation);

	//Turret rotation
	if (TankController)
	{
		FVector mousePos = TankController->GetMousePos();
		FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mousePos);
		FRotator currRotation = TurretMesh->GetComponentRotation();
		targetRotation.Pitch = currRotation.Pitch;
		targetRotation.Roll = currRotation.Roll;
		TurretMesh->SetWorldRotation(FMath::RInterpTo(currRotation, targetRotation, DeltaTime, RotationSpeed));
	}
}
void ATankPawn::IncreaseAmmunition(int Ammunition)
{
	Cannon->SetAmmunition(Cannon->GetAmmunition() + Ammunition);
}