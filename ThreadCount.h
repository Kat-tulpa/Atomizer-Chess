#pragma once

#include <thread>
static unsigned int THREAD_COUNT = std::thread::hardware_concurrency();