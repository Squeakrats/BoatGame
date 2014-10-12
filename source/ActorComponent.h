#pragma once
#include <memory>
class Actor;
class ActorComponent;
typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
class ActorComponent {
protected:
	StrongActorPtr mActor;
public:
	ActorComponent(void){};
	virtual unsigned int GetId(void) = 0;
	StrongActorPtr GetActor(void) { return mActor; };
	virtual void SetActor(StrongActorPtr actor) { mActor = actor; }
//public:
};

