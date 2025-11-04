#pragma once
#include <string>
struct TagComponent {
	std::string tag;
	TagComponent() = default;
	TagComponent(const std::string& t) : tag(t) {}
	operator std::string& () { return tag; }
	bool operator==(const TagComponent& other) const { return tag == other.tag; }
};