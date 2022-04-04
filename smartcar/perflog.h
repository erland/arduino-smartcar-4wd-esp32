
class PerfLog {
  private:
    long start;
  public:
    PerfLog();
    void begin();
    void end(const char *message);
};
