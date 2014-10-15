#pragma once
#include <map>
#include <memory>
#include <functional>
#include <list>

template<class argType>
class Emitter {
	typedef std::function<void(argType)> callback;
	typedef std::shared_ptr<Emitter<argType>> StrongEmitterPtr;
	typedef std::weak_ptr<Emitter<argType>> WeakEmitterPtr;

protected:
	std::map<unsigned int, std::list<callback>> mListeners;
	WeakEmitterPtr mParent;
public:
	Emitter(void){};

	void SetParent(StrongEmitterPtr parent) {
		mParent = parent;
	}

	void On(unsigned int id, callback function) {
		mListeners[id].push_back(function);
	}

	void Emit(unsigned int id, argType arg) {
		auto itr = mListeners.find(id);
		if (itr != mListeners.end()){
			auto list = itr->second;
			for (auto function : list) {
				function(arg);
			}
		}
		
		StrongEmitterPtr parent = mParent.lock();
		if (parent != nullptr) {
			parent->Emit(id, arg);
		}
	}
};

template<>
class Emitter<void> {
	typedef std::function<void(void)> callback;
	typedef std::shared_ptr<Emitter<void>> StrongEmitterPtr;
	typedef std::weak_ptr<Emitter<void>> WeakEmitterPtr;

protected:
	std::map<unsigned int, std::list<callback>> mListeners;
	WeakEmitterPtr mParent;
public:
	Emitter(void){};

	void SetParent(StrongEmitterPtr parent) {
		mParent = parent;
	}

	void On(unsigned int id, callback function) {
		mListeners[id].push_back(function);
	}

	void Emit(unsigned int id) {
		auto itr = mListeners.find(id);
		if (itr != mListeners.end()){
			auto list = itr->second;
			for (auto function : list) {
				function();
			}
		}
		
		StrongEmitterPtr parent = mParent.lock();
		if (parent != nullptr) {
			parent->Emit(id);
		}
	}
};