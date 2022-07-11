#include <iostream>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>

using namespace std;

template <class IN>
class PipelineIn
{
public:
    virtual void run(IN input) = 0;
    virtual void eosAndWait(int n = -1) = 0;
};

template <class IN, class OUT>
class Pipeline : public PipelineIn<IN>
{
private:
    mutex lock;
    function<OUT(IN)> callback;
    PipelineIn<OUT> *next;
    queue<IN> inputs;
    int maxNumber = -1;
    int stored = 0;
    int done = 0;
    thread *th;
    bool last = false;

    void runPipe()
    {
        while (this->maxNumber < 0 || this->done + 1 <= this->maxNumber)
        {
            this->lock.lock();
            int size = this->inputs.size();
            this->lock.unlock();
            if (size > 0)
            {
                this->lock.lock();
                auto input = this->inputs.front();
                auto res = this->callback(input);
                this->done += 1;
                this->inputs.pop();
                this->lock.unlock();
                if (!this->last)
                {
                    this->next->run(res);
                }
            }
        }
    }

public:
    Pipeline(function<OUT(IN)> callback, PipelineIn<OUT> *next)
    {
        this->callback = callback;
        this->next = next;
        th = new thread([&]()
                        { this->runPipe(); });
    }

    Pipeline(function<OUT(IN)> callback)
    {
        this->callback = callback;
        this->last = true;
        th = new thread([&]()
                        { this->runPipe(); });
    }

    void run(IN input)
    {
        this->lock.lock();
        this->stored++;
        this->inputs.push(input);
        this->lock.unlock();
    }

    void eosAndWait(int n = -1)
    {
        this->lock.lock();
        int maxNumber = n > 0 ? n : this->stored;
        this->maxNumber = maxNumber;
        bool last = !this->last;
        this->lock.unlock();
        if (last)
        {
            next->eosAndWait(maxNumber);
        }
        th->join();
    }
};