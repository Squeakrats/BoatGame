#include "MeshComponent.h"
#include "Actor.h"

void MeshComponent::OnActorMove() {
	SetPosition(mActor->GetPosition());
	SetRotation(mActor->GetRotation());
}

void MeshComponent::SetActor(StrongActorPtr actor) {
	mActor = actor;
}