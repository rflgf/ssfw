#pragma once

// C++/C standard library headers.
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <list>
#include <map>
#include <stdio.h>
#include <string>

// vendor headers.
#include "nlohmann/json.hpp"

namespace ssfw
{

// assertion and logging.
#pragma warning(push)
#pragma warning(disable : 4996)
#define SSFW_CLEAN_ERRNO() (errno == 0 ? "None" : strerror(errno))
#define SSFW_LOG_ERROR(M, ...)                                                 \
	fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__,  \
	        SSFW_CLEAN_ERRNO(), ##__VA_ARGS__)
#define SSFW_ASSERT(A, M, ...)                                                 \
	if (!(A))                                                                  \
	{                                                                          \
		SSFW_LOG_ERROR(M, ##__VA_ARGS__);                                      \
		assert(A);                                                             \
	}
#pragma warning(pop)

// type definitions.
using entity = size_t;
using time_unit = size_t;

} // namespace ssfw
