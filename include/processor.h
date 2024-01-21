#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
    unsigned long long prev_idle{};
    unsigned long long prev_active{};
};

#endif