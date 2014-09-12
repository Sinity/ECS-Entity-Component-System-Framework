#pragma once
#include <unordered_map>

using Entity = unsigned int;
using ArgsMap = std::unordered_map<std::string, std::string>;

/** \brief base Component type
*
* Each component have owner member, which defines to what Entity component belongs to. Entity is virtual construct,
* it is just number, really Entities don't exist. Entity is just sum of it's components.
*
* You can use destructors. You can define any method, constructor, but it's meant as a structure of data, not a class.
*
* component definitions should be created with COMPONENT macro, for benefits of defining entities from data.
* It looks that:
*
* COMPONENT(ComponentTypeName) {
*   int x;
*   int y = 42;
*   char* buff;
*
*   void init(ArgsMap args) {
*       x = args["x"];
*   }
*
*   ~ComponentTypeName() {
*       delete buff;
*   }
* };
*
*/
struct Component {
	Entity owner;

	virtual void init(ArgsMap args = ArgsMap()) {
		(void)args;
	}

	virtual ~Component() {
	}
};
