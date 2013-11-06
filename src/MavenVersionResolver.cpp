#include "MavenVersionResolver.h"

MavenVersionResolver::MavenVersionResolver()
{
}

MavenVersionResolver::~MavenVersionResolver()
{
}



MavenVersionResult* MavenVersionResolver::resolveVersion(MavenArtefact artefact, std::string basePath, bool localDeploy){
	MavenVersionResult *result = new MavenVersionResult();
	std::string builder;
	std::string sep = PATH_SEPARATOR;
	builder.append(basePath);

	if (basePath.find("http") == 0) {
		sep = "/";
	}
	if (basePath.find(sep) != 0) {
		builder.append(sep);
	}
	if (basePath.find("http") == 0) {
		builder.append(artefact.getGroup().replace(builder.begin(),builder.end(),".", "/"));
	} else {
		builder.append(artefact.getGroup().replace(builder.begin(),builder.end(),".", PATH_SEPARATOR));
	}
	builder.append(sep);
	builder.append(artefact.getName());
	builder.append(sep);
	builder.append(artefact.getVersion());
	builder.append(sep);
	if (localDeploy) {
		builder.append(localmetaFile);
	} else {
		builder.append(metaFile);
	}

	// TODO 

	return result;	
}


std::string MavenVersionResolver::foundRelevantVersion(MavenArtefact artefact, std::string basePath, bool localDeploy){
	std::string askedVersion = artefact.getVersion(); // todo tolower
	bool release = false;
	bool lastest = false;
	std::string sep = PATH_SEPARATOR;
	if (askedVersion.compare("release") == 0) {
		release = true;
	}
	if (askedVersion.compare("latest") == 0) {
		lastest = true;
	}
	if (!release && !lastest) {
		return NULL;
	}
	std::string builder;
	if (basePath.find("http") == 0) {
		sep = "/";
	}
	if (basePath.find(sep) != 0) {
		builder.append(sep);
	}
	if (basePath.find("http") == 0) {
		builder.append(artefact.getGroup().replace(builder.begin(),builder.end(),".", "/"));
	} else {
		builder.append(artefact.getGroup().replace(builder.begin(),builder.end(),".", PATH_SEPARATOR));
	}
	builder.append(sep);
	builder.append(artefact.getName());
	builder.append(sep);
	builder.append(artefact.getVersion());
	builder.append(sep);
	if (localDeploy) {
		builder.append(localmetaFile);
	} else {
		builder.append(metaFile);
	}

}