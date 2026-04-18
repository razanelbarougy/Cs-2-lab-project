#include <iostream>
#include <string_view>
#include <string>
#include <map>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

std::map<std::string, std::shared_ptr<tcp::socket>> connectedUsers;

std::string extractField(const std::string& message, const std::string& fieldName) {
std::string key = "\"" + fieldName + "\":\"";
size_t start = message.find(key);

if (start == std::string::npos) {
return "";
}

start += key.length();
size_t end = message.find("\"", start);

if (end == std::string::npos) {
return "";
}

return message.substr(start, end - start);
}

std::string extractReceiver(const std::string& message) {
std::string key = "\"receiver\":\"";
size_t start = message.find(key);

if (start == std::string::npos) {
return "";
}

start += key.length();
size_t end = message.find("\"", start);

if (end == std::string::npos) {
return "";
}

return message.substr(start, end - start);
}

// Coroutine to handle an individual client's connection
awaitable<void> handle_client(std::shared_ptr<tcp::socket> socket) {
char data[1024];
std::string currentUser = "";

while (true) {
auto [ec, bytes_read] = co_await socket->async_read_some(
boost::asio::buffer(data),
as_tuple(use_awaitable)
);

if (!ec) {
std::string message(data, bytes_read);

std::cout << "Server received: "
<< std::string_view(data, bytes_read) << "\n";

std::string type = extractField(message, "type");
std::string sender = extractField(message, "sender");

if (type == "login") {
connectedUsers[sender] = socket;
currentUser = sender;
std::cout << sender << " logged in.\n";
}
else if (type == "sendMessage") {
for (auto& pair : connectedUsers) {
const std::string& username = pair.first;
std::shared_ptr<tcp::socket> otherSocket = pair.second;

if (username != sender && otherSocket && otherSocket->is_open()) {
std::string outgoing = message;

if (!outgoing.empty() && outgoing.back() != '\n') {
outgoing += '\n';
}

co_await boost::asio::async_write(
*otherSocket,
boost::asio::buffer(outgoing),
use_awaitable
);
}
}
}
else if (type == "privateMessage") {
std::string receiver = extractReceiver(message);

auto it = connectedUsers.find(receiver);
if (it != connectedUsers.end() && it->second && it->second->is_open()) {
std::string outgoing = message;

if (!outgoing.empty() && outgoing.back() != '\n') {
outgoing += '\n';
}

co_await boost::asio::async_write(
*(it->second),
boost::asio::buffer(outgoing),
use_awaitable
);
}
}
}
else {
if (ec == boost::asio::error::eof) {
std::cout << "Client disconnected.\n";
}
else {
std::cout << "Read error: " << ec.message() << "\n";
}

if (!currentUser.empty()) {
connectedUsers.erase(currentUser);
std::cout << currentUser << " removed from connected users.\n";
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

auto clientSocket = std::make_shared<tcp::socket>(std::move(socket));
co_spawn(io_ctx, handle_client(clientSocket), detached);
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
