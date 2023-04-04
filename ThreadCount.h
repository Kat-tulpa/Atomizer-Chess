#pragma once

#include <thread>
static const unsigned int THREAD_COUNT = std::thread::hardware_concurrency();