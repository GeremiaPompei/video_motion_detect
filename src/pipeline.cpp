#include <iostream>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>

using namespace std;

template <typename IN, typename OUT>
class Pipeline {
    private:
        mutex lock;
        function<OUT(IN)> callback;
        function<void(OUT)> next;
        queue<IN> inputs;
        int maxNumber;
        int count = 0;
        thread *th;

        void runPipe() {
            while(this->count + 1 < this->maxNumber) {
                this->lock.lock();
                int size = this->inputs.size();
                this->lock.unlock();
                if(size > 0) {
                    this->lock.lock();
                    auto input = this->inputs.front();
                    OUT res = this->callback(input);
                    this->count+=1;
                    this->inputs.pop();
                    this->lock.unlock();
                    this->next(res);
                }
            }
        }

    public:
        Pipeline(int maxNumber, function<OUT(IN)> callback, function<void(OUT)> next) {
            this->maxNumber = maxNumber;
            this->callback = callback;
            this->next = next;
            th = new thread([&] () {this->runPipe(); });
        }

        void run(IN input) {
            this->lock.lock();
            this->inputs.push(input);
            this->lock.unlock();
        }

        void wait() {
            th->join();
        }

};