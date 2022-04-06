#ifndef PERFLOG_H
#define PERFLOG_H

class PerfLog {
  private:
    long start;
  public:
    PerfLog();
    void begin();
    void end(const char *message);
};

#endif
