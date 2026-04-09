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
		static Project& getInstance() {
			static Project instance;
			return instance;
		}
		std::string getName() const { return name; }
		std::string getPath() const { return path; }
		std::string getAssetsPath() const { return path + "/assets"; }
	private:
		std::string name = "firstAttempt";
		std::string path = "C:/Users/franc/Documents/OnYuuProject";
	};
};