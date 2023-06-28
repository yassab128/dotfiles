#pragma once

#define MEMINFO "/proc/meminfo"

#define CONVERT_FROM_KB_TO_MB(x) (x) >> 10U

void *get_mem_usage_in_MB(void *arg);
