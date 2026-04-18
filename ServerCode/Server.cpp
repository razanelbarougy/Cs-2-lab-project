#include <iostream>
#include <string_view>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// Coroutine to handle an individual client's connection
awaitable<void> handle_client(tcp::socket socket) {
	char data[1024];
while(true) {
	auto [ec, bytes_read] = co_await socket.async_read_some(
	boost::asio::buffer(data),
	as_tuple(use_awaitable)
	);

	if (!ec) {
		std::cout << "Server received: "
		<< std::string_view(data, bytes_read) << "\n";
	}
	else {
		if (ec == boost::asio::error::eof) {
			std::cout << "Client disconnected.\n";	
		}
		else {
			std::cout << "Read error: " << ec.message() << "\n";
		}
		break;
	}
	}
}
 
// Coroutine to listen for incoming connections
awaitable<void> listener() {
    // We grab a handle to the engine that is running this coroutine
    auto io_ctx = co_await boost::asio::this_coro::executor;

    tcp::acceptor acceptor(io_ctx, { tcp::v4(), 54321 });

    std::cout << "Server is listening on 127.0.0.1: port 54321...\n";

    while (true) {
        // Accept a new connection
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        if (!ec) {
            // Spawn a new coroutine to handle this specific client
	    std::cout << "Client connected.\n";
            co_spawn(io_ctx, handle_client(std::move(socket)), detached);
        }
        else {
            std::cout << "Accept error: " << ec.message() << "\n";
        }
    }
}



int main() {
    boost::asio::io_context io_context;

    // Start the main listener coroutine
    co_spawn(io_context, listener(), detached);

    // Run the event loop
    io_context.run();

    return 0;
}
