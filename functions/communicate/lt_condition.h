#ifndef LT_CONDITION_H
#define LT_CONDITION_H

#include <condition_variable>
#include "awe_log.h"
#include "lt_data/lt_data_t.h"
#include "algo/algo.h"

enum lt_condition_stat_t
{
    lt_condition_stat_notstart,
    lt_condition_stat_waiting,
    lt_condition_stat_notified
};

class lt_condition : public ServerSan_Algo::algo_obj
{
public:
    void from_json_obj(const json_obj &obj) override
    {
    }
    
    json_obj to_json_obj() const override
    {
        return json_obj();
    }

private:
    lt_condition_stat_t     stat     = lt_condition_stat_notstart;
    std::mutex              lock;
    std::condition_variable cond;
    int                     error;
    lt_data_t               _data;
public:
    lt_condition() : error(0), stat(lt_condition_stat_notstart), _data()
    {
        AWE_MODULE_DEBUG("cond",
                         "XXXXXXXXXXXXXXXX    lt_condition   XXXXXXXXXXXXXXXXXXX this %p",
                         this);
    }
    
    int wait()
    {
        AWE_MODULE_DEBUG("cond",
                         "+++++++++++++++++++++wait before lock this %p", this);
        std::unique_lock<std::mutex> lck(lock);
        AWE_MODULE_DEBUG("cond", "---------------------wait after lock this %p",
                         this);
        
        if ( stat == lt_condition_stat_notstart )
        {
            stat = lt_condition_stat_waiting;
            AWE_MODULE_DEBUG("cond", "+-+-+-is_to_wait before wait this %p",
                             this);
            while(stat != lt_condition_stat_notified)
            {
                cond.wait(lck);
            }
            AWE_MODULE_DEBUG("cond", "+-+-+-is_to_wait after wait this %p",
                             this);
        }
        else if ( stat == lt_condition_stat_notified )
        {
            AWE_MODULE_DEBUG("cond", "+-+-+-notified has been set this %p",
                             this);
        }
        else
        {
            AWE_MODULE_DEBUG("cond", "!!!!!!!!!!!!!!Too many waiting this %p",
                             this);
            abort();
        }
        stat = lt_condition_stat_notstart;
        return error;
    }
    
    void notify(const lt_data_t &data, int err)
    {
        error = err;
        if ( !error )
        {
            _data = data;
        }
        AWE_MODULE_DEBUG("cond",
                         "***************************notify before lock %p", this);
        std::unique_lock<std::mutex> lck(lock);
        AWE_MODULE_DEBUG("cond",
                         "***************************notify after lock this %p",
                         this);
        if ( stat == lt_condition_stat_waiting )
        {
            AWE_MODULE_DEBUG("cond",
                             "+-+-+-is_to_wait before notify_one this %p",
                             this);
            stat = lt_condition_stat_notified;
            AWE_MODULE_DEBUG("cond",
                             "+-+-+-is_to_wait after notify_one this %p", this);
            cond.notify_all();
        }
        else if ( stat == lt_condition_stat_notstart )
        {
            AWE_MODULE_DEBUG("cond", "+-+-+-notified is being set this %p",
                             this);
            stat = lt_condition_stat_notified;
        }
        else
        {
            AWE_MODULE_DEBUG("cond", "!!!!!!!!!!!!!!Too many notify this %p",
                             this);
            abort();
        }
    }
    
    void notify()
    {
        AWE_MODULE_DEBUG("cond",
                         "***************************notify before lock %p", this);
        std::unique_lock<std::mutex> lck(lock);
        AWE_MODULE_DEBUG("cond",
                         "***************************notify after lock this %p",
                         this);
        if ( stat == lt_condition_stat_waiting )
        {
            AWE_MODULE_DEBUG("cond",
                             "+-+-+-is_to_wait before notify_one this %p",
                             this);
            stat = lt_condition_stat_notified;
            cond.notify_one();
            AWE_MODULE_DEBUG("cond",
                             "+-+-+-is_to_wait after notify_one this %p", this);
        }
        else if ( stat == lt_condition_stat_notstart )
        {
            AWE_MODULE_DEBUG("cond", "+-+-+-notified is being set this %p",
                             this);
            stat = lt_condition_stat_notified;
        }
        else
        {
            AWE_MODULE_DEBUG("cond", "!!!!!!!!!!!!!!Too many notify this %p",
                             this);
            abort();
        }
    }
    
    const lt_data_t &get_data() const
    {
        return _data;
    }
    
    virtual ~lt_condition()
    {
        AWE_MODULE_DEBUG("cond",
                         "XXXXXXXXXXXXXXXX    ~lt_condition   XXXXXXXXXXXXXXXXXXX this %p",
                         this);
    }
};


#endif //SERVER_LT_CONDITION_H
