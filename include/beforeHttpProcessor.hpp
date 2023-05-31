#ifndef BEFORE_HTTP_PROCESSOR_HPP
#define BEFORE_HTTP_PROCESSOR_HPP

#include "MethodHandler.hpp"
#include "Request.hpp"
#include "ResponseGenerator.hpp"

void printVector(std::vector<char> vec);

void fillRequest(Request &request_data);

void fillResponse(Response &response_data);

std::vector<char> getHttpMessage();

#endif
