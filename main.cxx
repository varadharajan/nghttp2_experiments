#include <iostream>
#include <thread>
#include <boost/thread/thread.hpp>
#include <chrono>
#include <algorithm>
#include <nghttp2/asio_http2_server.h>

using namespace std;
using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

void sayHello(const response * res, int i) {
    cout << "Handling request : " << i << endl;
    this_thread::sleep_for(chrono::seconds(1));
    cout << "done : " << i << endl;
    if(res != nullptr) {
    res -> write_head(200);
    res -> end("hello, world\n");
    }
}


int main(int argc, char const *argv[])
{
    boost::system::error_code ec;
    boost::asio::io_service io_service;

    auto i = 0;

    boost::thread_group worker_threads;
    {
        std::auto_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
        for(int x = 0; x < 100; ++x)
        {
            cout << "Starting thread : " << x << endl;
            worker_threads.create_thread(
                boost::bind(&boost::asio::io_service::run, &io_service));
        }
        http2 server;

        server.handle("/", [&io_service, &i](const request &req, const response &res) {
            ++i;
            io_service.post(std::bind(&sayHello, &res, i));
        });


        cout << "Starting server.." << endl;
        if (server.listen_and_serve(ec, "localhost", "3000")) {
            std::cerr << "error: " << ec.message() << std::endl;
        }
        while(1);
    }

    worker_threads.join_all();


    return 0;
}