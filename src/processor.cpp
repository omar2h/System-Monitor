#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization()
{
    unsigned long long idle_time{LinuxParser::IdleJiffies()};
    unsigned long long active_time{LinuxParser::ActiveJiffies()};
    unsigned long long prev_total_time{prev_idle + prev_active};
    unsigned long long total_time{idle_time + active_time};
    unsigned long long total_time_d{total_time - prev_total_time};
    unsigned long long idle_time_d{idle_time - prev_idle};
    prev_idle = idle_time;
    prev_active = active_time;
    return static_cast<float>(total_time_d - idle_time_d) / total_time_d;
}