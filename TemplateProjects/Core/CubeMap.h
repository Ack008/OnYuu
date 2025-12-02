#pragma once
#include <vector>
#include <string>
#include <memory>
class CubeMap {
public:
	virtual void bind(unsigned int unit = 0) = 0;
	unsigned int getID() const { return ID; }
	static std::shared_ptr<CubeMap> createCubeMap(const std::vector<std::string>& faces);
private:
	unsigned int ID;

};