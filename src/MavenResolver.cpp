#include "MavenResolver.h"
#include "MavenArtefact.h"



MavenResolver::MavenResolver()
{
	versionResolver = new MavenVersionResolver();
	basePath = "/home/jed/.m2"; // TO FIX 
}

MavenResolver::~MavenResolver()
{
	delete versionResolver;
	
}


FILE * MavenResolver::resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::vector<std::string> urls){
	
    MavenArtefact artefact;
    artefact.setGroup(group);
    artefact.setName(name);
    artefact.setVersion(versionAsked);
    artefact.setExtension(extension);
    
    if (artefact.getVersion().compare("release") == 0 || artefact.getVersion().compare("latest") == 0) {
       std::string vremoteSaved = versionResolver->foundRelevantVersion(artefact, basePath, false);
    	
    	
    	
    }
	
	
}