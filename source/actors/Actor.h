#pragma once
#include <memory>
#include <map>
#include "components\ActorComponent.h"
#include "../events/Emitter.h"
#include "glm/glm.hpp"

class Actor;
class EventData;
typedef std::tr1::shared_ptr<Actor> StrongActorPtr;
typedef std::map<unsigned int, StrongActorComponentPtr> ComponentMap;

class Actor : public Emitter<EventData*> {
private:
	ComponentMap mComponentMap;
	unsigned int mId; //come up with some number system. cant just increment because they neeed to be the same server and client side :/
	glm::vec3 mPosition;
	glm::vec3 mRotation; //maybe quat some day?
public:
	const glm::vec3& GetPosition(void);
	const glm::vec3& GetRotation(void);

	void SetPosition(glm::vec3&);
	void AddPosition(glm::vec3&);
	void SetRotation(glm::vec3&);

	unsigned int GetId(void) { return mId; }
	void SetId(unsigned int id) { mId = id; }


	void AddComponent(StrongActorComponentPtr component) {
		auto iterator = mComponentMap.find(component->GetId());
		if (iterator == mComponentMap.end()){
			mComponentMap[component->GetId()] = component;
		}
		//log about double adding?
	}

	template<class ComponentType>
	std::tr1::shared_ptr<ComponentType> GetComponent(unsigned int id) {
		auto iterator = mComponentMap.find(id);
		if (iterator == mComponentMap.end()){

			return std::tr1::shared_ptr<ComponentType>();
		}
		return std::tr1::static_pointer_cast<ComponentType>(iterator->second);
	}

	
};
