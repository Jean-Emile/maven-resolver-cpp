#ifndef MAVENVERSIONRESULT_H_
#define MAVENVERSIONRESULT_H_
#include <string>

class MavenVersionResult
{
public:
	MavenVersionResult();
	virtual ~MavenVersionResult();
	std::string getLastUpdate();
	void setLastUpdate(std::string lastUpdate);
	 std::string getValue();
	 void setValue(std::string value);
	 std::string getUrl_origin();
	 void setUrl_origin(std::string url_origin);
	 bool isNotDeployed();
	 void setNotDeployed(bool notDeployed);
	 bool isPrior(MavenVersionResult remote);
	 
private:
	std::string value;
	std::string lastUpdate;
	std::string url_origin;
	bool notDeployed ;

};

#endif /*MAVENVERSIONRESULT_H_*/
