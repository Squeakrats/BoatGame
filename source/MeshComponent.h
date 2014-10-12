#pragma once
#include "SceneNode.h"
#include "ActorComponent.h"
#include <memory>
class MeshComponent;
typedef std::shared_ptr<MeshComponent> StrongMeshComponentPtr;
class MeshComponent : public SceneNode, public ActorComponent {
public:
	virtual unsigned int GetId(void) { return 1; }
	virtual void SetActor(StrongActorPtr);
	void OnActorMove(void);
};