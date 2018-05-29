#ifndef DEXTIMER_H
#define DEXTIMER_H

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace dex {

class  Timer {
private:
    boost::shared_ptr<boost::thread> thr;

    template <typename Callable>
    class func {
        unsigned int dur;
        Callable& cb;

    public:
        func (long long dur, const Callable& cb): dur (dur), cb((Callable&)cb) {}
        void operator() () {
            boost::this_thread::sleep(boost::posix_time::milliseconds(dur));
            cb();
        }
    };

public:
    template <typename Callable> Timer (long long dur, const  Callable& cb)  {
        func<Callable> ff (dur, cb);
        thr.reset(new boost::thread(ff));
    }

    ~Timer() {}
};

}

#endif //DEXTIMER_H
