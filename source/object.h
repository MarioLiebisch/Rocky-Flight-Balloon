#pragma once
#include <list>
#include <memory>

class Solid;
class Hazard;

class Object {
protected:
	bool mQueryDeletion;
public:
	Object(void) : mQueryDeletion(false) { }
	virtual ~Object(void) { }
	virtual void update(std::list<std::shared_ptr<Solid> > &solids, std::list<std::shared_ptr<Hazard> > &hazards) = 0;
	bool toDelete(void) const { return mQueryDeletion; }
};
