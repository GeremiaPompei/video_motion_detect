#include <iostream>
#include <chrono>
#include <map>

using namespace std;

#ifndef HEADER_TIMER_HANDLER
#define HEADER_TIMER_HANDLER

struct Timer {
    chrono::time_point<chrono::high_resolution_clock> t_start;
    bool stopped;
    double time;
};

class TimerHandler {
    private:
        map<string, Timer> timers;
        
    public:
        TimerHandler() {};

        void start(string label) {
            if(this->timers.count(label) > 0) {
                this->timers[label].stopped = false;
                this->timers[label].t_start = chrono::high_resolution_clock::now();
            } else {
                struct Timer timer;
                timer.t_start = chrono::high_resolution_clock::now();
                timer.stopped = false;
                this->timers.insert({label, timer});
            }
        }

        void stop(string label) {
            auto t_end = chrono::high_resolution_clock::now();
            this->timers[label].stopped = true;
            this->timers[label].time += chrono::duration<double>(t_end - this->timers[label].t_start).count();
        }
        
        string toString() {
            string report;
            map<string, Timer>::iterator it;
            for (it = this->timers.begin(); it != this->timers.end(); it++) {
                report += it->first + ": " + to_string(it->second.time) + " seconds\n";
            }
            return report;
        }
};

#endif