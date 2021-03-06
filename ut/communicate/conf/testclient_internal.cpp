#include "testclient_internal.h"

namespace testclient
{

#define SERVER_PORT    7777

#include <lt_function/lt_function_error.h>

testclient_client::testclient_client(testclient_client_callback *_cb) : cb(_cb), sess(NULL)
{}

int testclient_client::connect(const std::string &ip)
{
    std::lock_guard<std::mutex> lck(m);
    if ( !sess )
    {
        sess = cb->get_session(ip);
        if ( !sess )
            return -RPC_ERROR_TYPE_CONNECT_FAIL;
    }
    try
    {
        sess->connect(ip, SERVER_PORT);
    } catch (...)
    {
        return -RPC_ERROR_TYPE_CONNECT_FAIL;
    }
    return RPC_ERROR_TYPE_OK;
}

void testclient_client::disconnect()
{
     std::lock_guard < std::mutex > lck(m);
    if (!sess) return;
    sess->disconnect();
    cb->put_session(sess);
    sess = NULL;
}
int testclient_client::test(IN const unsigned int &input_int, INOUT void *&internal_pri)
{
    if ( !sess )
        return -RPC_ERROR_TYPE_CONNECT_FAIL;
    return cb->snd(sess, boost::bind(&testclient_client::test_gendata, this, input_int,  internal_pri, _1));
}

int testclient_client::ping_internal(INOUT void *&internal_pri)
{
    int error_internal = 0;
    lt_condition _internal_sync_cond;
    cb->snd(sess, boost::bind(&testclient_client::ping_internal_gendata, this,  &_internal_sync_cond, internal_pri, _1));
    int err_internal = _internal_sync_cond.wait();
    if ( err_internal < 0 )
    {
        return err_internal;
    }
    const lt_data_t &res_data = _internal_sync_cond.get_data();
    unsigned char *buf = res_data.get_buf();
    error_internal = lt_data_translator::to_uint(buf);
    return err_internal | error_internal;
}

int testclient_client::test_gendata(IN const unsigned int &input_int, INOUT void *&internal_pri,lt_data_t *data)
{
    unsigned int func_type = server_function_callback_type_7777_test;
    data->_length = sizeof(func_type) + sizeof(unsigned int) + sizeof(void *) + sizeof(void *);
    data->realloc_buf();
    unsigned char *buf = data->get_buf();
    lt_data_translator::by_uint(func_type, buf);
    lt_data_translator::by_uint(input_int, buf);
    lt_data_translator::by_void_p(internal_pri, buf);
    return 0;
}

int testclient_client::ping_internal_gendata(lt_condition *_internal_sync_cond,INOUT void *&internal_pri, lt_data_t *data)
{
    unsigned int func_type = server_function_callback_type_7777_ping_internal;
    data->_length = sizeof(func_type) + sizeof(void *) + sizeof(void *);
    data->realloc_buf();
    unsigned char *buf = data->get_buf();
    lt_data_translator::by_uint(func_type, buf);
    lt_data_translator::by_void_p(_internal_sync_cond, buf);
    lt_data_translator::by_void_p(internal_pri, buf);
    return 0;
}

testclient_client_callback::testclient_client_callback(int thread_num, testclient_callback_handler *cb_handler) :
        cb_handler(cb_handler),
        server(thread_num),
        lt_session_cli_set(NULL, this),
        lt_client_service(NULL, SERVER_PORT)
{
    lt_session_cli_set::set_ioservice(server.get_io_service());
    lt_client_service::set_ioservice(server.get_io_service());
}

void testclient_client_callback::handler_by_output(lt_data_t *received_data)
{
    unsigned char *buf = received_data->get_buf();
    unsigned int func_type = lt_data_translator::to_uint(buf);
    switch ( func_type )
    {
        case client_function_callback_type_7777_test:
        {
            unsigned int output_int = lt_data_translator::to_uint(buf);
            unsigned int error_internal = lt_data_translator::to_uint(buf);
            void * internal_pri = lt_data_translator::to_void_p(buf);
            cb_handler->test_callback(output_int, internal_pri, error_internal);
        }
            break;
        case client_function_callback_type_7777_ping_internal:
        {
            unsigned int error_internal = lt_data_translator::to_uint(buf);
            void *internal_sync_cond_p = lt_data_translator::to_void_p(buf);
            void * internal_pri = lt_data_translator::to_void_p(buf);
            lt_data_t res_data(sizeof(error_internal));
            unsigned char *res_buf = res_data.get_buf();
            lt_data_translator::by_uint(error_internal, res_buf);
            lt_condition *_internal_sync_cond = (lt_condition *) internal_sync_cond_p;
            _internal_sync_cond->notify(res_data, error_internal);
        }
            break;
        default:
            abort();
    }
}

void testclient_client_callback::handler_by_input(lt_data_t *sent_data, int error_internal)
{
    unsigned char *buf = sent_data->get_buf();
    unsigned int func_type = lt_data_translator::to_uint(buf);
    switch ( func_type )
    {
        case server_function_callback_type_7777_test:
        {
            lt_data_translator::skip_uint(buf);
            unsigned int output_int;
            void * internal_pri = lt_data_translator::to_void_p(buf);
            cb_handler->test_callback(output_int, internal_pri, error_internal);
        }
            break;
        case server_function_callback_type_7777_ping_internal:
        {
            void *internal_sync_cond_p = lt_data_translator::to_void_p(buf);
            lt_data_t res_data;
            lt_condition *_internal_sync_cond = (lt_condition *) internal_sync_cond_p;
            void * internal_pri;
            _internal_sync_cond->notify(res_data, error_internal);
        }
            break;
        default:
            abort();
    }
}

void testclient_client_callback::disconnected(lt_session *sess)
{
    lt_client_service::disconnected(sess);
    cb_handler->disconnected(sess);
}

}
