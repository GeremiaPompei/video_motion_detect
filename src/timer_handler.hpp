#include <iostream>
#include <chrono>
#include <map>

using namespace std;

#ifndef HEADER_TIMER_HANDLER
#define HEADER_TIMER_HANDLER

class TimerHandler
{
private:
    map<string, double> timers;

public:
    TimerHandler(){};

    void computeTime(string label, function<void()> callback)
    {
        auto t_start = chrono::high_resolution_clock::now();
        callback();
        auto t_end = chrono::high_resolution_clock::now();
        double time = chrono::duration<double>(t_end - t_start).count();
        if (this->timers.count(label) > 0)
        {
            this->timers[label] += time;
        }
        else
        {
            this->timers.insert({label, time});
        }
    }

    string toString()
    {
        string report;
        map<string, double>::iterator it;
        for (it = this->timers.begin(); it != this->timers.end(); it++)
        {
            report += it->first + ": " + to_string(it->second) + " seconds\n";
        }
        return report;
    }

    string toCSV()
    {
        string report;
        map<string, double>::iterator it;
        for (it = this->timers.begin(); it != this->timers.end(); it++)
        {
            report += it->first + ";";
        }
        report += "\n";
        for (it = this->timers.begin(); it != this->timers.end(); it++)
        {   
            string time = to_string(it->second);
            int pos = time.find(".");
            report += time.replace(pos, pos + 1, ",") + ";";
        }
        report += "\n";
        return report;
    }
};

#endif