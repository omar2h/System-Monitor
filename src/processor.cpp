#include "processor.h"
#include "linux_parser.h"

// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization()
{
    unsigned long idle_time{LinuxParser::IdleJiffies()};
    unsigned long active_time{LinuxParser::ActiveJiffies()};
    unsigned long prev_total_time{prev_idle + prev_active};
    unsigned long total_time{idle_time + active_time};
    unsigned long total_time_d{total_time - prev_total_time};
    unsigned long idle_time_d{idle_time - prev_idle};
    prev_idle = idle_time;
    prev_active = active_time;
    return static_cast<float>(total_time_d - idle_time_d) / total_time_d;
}