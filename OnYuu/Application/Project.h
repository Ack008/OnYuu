#pragma once
#include <string>
namespace OnYuu
{
	class Project
	{
	public:
		Project();
		~Project();
		void Load(const char* path);
		void Save(const char* path);
	private:
		std::string name = "firstAttempt";
		std::string path = "";
	};
};