#ifndef MAVENARTEFACT_H_
#define MAVENARTEFACT_H_
#include <string>
class MavenArtefact
{
public:
	MavenArtefact();
	virtual ~MavenArtefact();
	std::string  getExtension();
	void setExtension(std::string  extension);
	std::string  getVersion();
	void setVersion(std::string  version);
	std::string  getGroup();
	void setGroup(std::string  group);
	std::string  getName();
	void setName(std::string  name);
	
private:
	std::string name;
	std::string group;
	std::string version;
	std::string extension;
};

#endif /*MAVENARTEFACT_H_*/
