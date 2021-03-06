#ifndef TEST_COMMUNICATE_LT_CLIENT_SERVICE_H
#define TEST_COMMUNICATE_LT_CLIENT_SERVICE_H

#include "lt_function/thread_pool.hpp"
#include "lt_session_cli_safe.h"
#include "lt_thread_server.h"
#include "lt_function/lt_safe_set.h"

class lt_client_service
        : public lt_session_callback,
          public ServerSan_Algo::algo_obj
{
    std::mutex                m;
    boost::asio::io_service   *io_service;
    //lt_safe_set<lt_data_t *> rcvdata_set;
    data_channel::thread_pool pool;
    //std::atomic_bool is_connect;
public:
    void from_json_obj(const json_obj &obj) override;
    
    json_obj to_json_obj() const override;

public:
    
    virtual ~lt_client_service() = default;
    
    lt_client_service(boost::asio::io_service *_io_service,
                      unsigned short port);
    
    void set_ioservice(boost::asio::io_service *_io_service);
    
    int snd(lt_session_cli_safe *sess, lt_data_t *data);

private:
    void
    rcv_done_nolock(lt_session *sess, lt_data_t *received_data, int error);
    
    void
    rcv_done(lt_session *sess, lt_data_t *received_data, int error) override;
    
    void
    rcv_done_inthread(lt_session *sess, lt_data_t *received_data, int error);
    
    void snd_done_inthread(lt_session *sess, lt_data_t *sent_data, int error);
    
    void snd_done(lt_session *sess, lt_data_t *sent_data, int error) override;

protected:
    virtual void disconnected(lt_session *sess) override;
    
    virtual void connected(lt_session *sess) override;

public:
    virtual void
    handler_by_output(lt_session_cli_safe *sess, lt_data_t *received_data) = 0;
    
    virtual void
    handler_by_input(lt_session_cli_safe *sess, lt_data_t *sent_data,
                     int error) = 0;
    
    virtual void handler_rcvd(lt_session_cli_safe *sess) = 0;
    
};


#endif
