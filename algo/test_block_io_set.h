//
// Created by root on 19-2-12.
//

#ifndef DATASTOR_UTILS_TEST_BLOCK_IO_SET_H
#define DATASTOR_UTILS_TEST_BLOCK_IO_SET_H

#include <string>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>

namespace ServerSan_Algo
{
class test_block_io_set : public block_io_set
{
    std::string file_dir;
    std::map<json_obj, block_io *> obj_map;
public:
    test_block_io_set( std::string file_dir,  unsigned long _size_secs);
public:
    block_io *get_block(const json_obj & obj);
    
    void put_block(block_io *io) override;
};
};

#endif //DATASTOR_UTILS_TEST_BLOCK_IO_SET_H