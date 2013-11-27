#include "api/MavenResolver.h"
#include "api/MavenArtefact.h"

#include <iostream>

MavenResolver::MavenResolver()
{
	versionResolver = new MavenVersionResolver();
	basePath = "/home/jed/.m2"; // TO FIX 
}

MavenResolver::~MavenResolver()
{
	delete versionResolver;
	
}


std::string MavenResolver::resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls)
{

	if(name.compare("CPPNodeType") == 0)
	{
		return "build/libkevoree-CPPNode.so";
	}
	
	if(name.compare("kevoree-group-websocket")==0)
	{
		return "build/libwebsocketgroup.so";
	}
	return "";
			//const char *libpath= "build/libwebsocketgroup.so";
    MavenArtefact artefact;
    artefact.setGroup(group);
    artefact.setName(name);
    artefact.setVersion(versionAsked);
    artefact.setExtension(extension);
    
    if (artefact.getVersion().compare("release") == 0 || artefact.getVersion().compare("latest") == 0) 
    {
       std::string vremoteSaved = versionResolver->foundRelevantVersion(artefact, basePath, false);
    	
    	
    	
    }
	
	
}
