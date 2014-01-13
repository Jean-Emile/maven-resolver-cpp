/*
 * HTTPResponseImpl.cpp
 *
 *  Created on: 20 déc. 2013
 *      Author: edaubert
 */
#include <network/http/api/HTTPResponseImpl.h>

namespace network {
namespace http {
namespace api {

HTTPResponseImpl::HTTPResponseImpl() {
	status = internal_server_error;
	stream = new std::istream(0);
}

HTTPResponseImpl::~HTTPResponseImpl() {
	delete stream;
}

void HTTPResponseImpl::addHeader(std::string _name, std::string _value) {
	std::map<std::string, std::string>::iterator position = headers.find(_name);
	if (position == headers.end()) {
		headers[_name] = _value;
	} else {
		headers.erase(position);
		headers[_name] = _value;
	}
}
void HTTPResponseImpl::removeHeader(std::string _name) {
	std::map<std::string, std::string>::iterator position = headers.find(_name);
	if (position != headers.end()) {
		headers.erase(position);
	}
}
void HTTPResponseImpl::setStatus(Status _status) {
	status = _status;
}

void HTTPResponseImpl::setStream(std::istream * _stream) {
	stream = _stream;
}

Status HTTPResponseImpl::getStatus() {
	return status;
}
std::map<std::string, std::string> HTTPResponseImpl::getHeaders() {
	return headers;
}
std::string HTTPResponseImpl::getHeader(std::string _name) {
	std::map<std::string, std::string>::iterator position = headers.find(_name);
	if (position != headers.end()) {
		return headers[_name];
	} else {
		return NULL;
	}
}

std::string HTTPResponseImpl::getContentAsString() {
	/*stream->seekg(0, stream->end);
	 int length = stream->tellg();
	 stream->seekg(0, stream->beg);
	 std::cout<< length << std::endl;
	 char * bytes = new char[length];
	 stream->read(bytes, length);
	 std::string * content = new std::string();*/
	/*stream->seekg(0, stream->end);
	 int length = stream->tellg();
	 stream->seekg(0, stream->beg);

	 std::string str;
	 str.resize(length, ' '); // reserve space
	 char* begin = &*str.begin();

	 stream->read(begin, length);*/
	std::string content;
	while (!stream->eof()) {
		std::string s;
		std::getline(*stream, s);
		content.append(s);
		content.append("\n");
	}
	return content;
}
//short int * HTTPResponseImpl::getContent() {
//}
//long HTTPResponseImpl::getContentLength() {
//}
std::istream& HTTPResponseImpl::getStream() {
	return *stream;
}
}
}
}

