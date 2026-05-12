#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

std::map<std::string, std::shared_ptr<tcp::socket>> connectedUsers;
std::map<std::string, std::set<std::string>> rooms;

// Game state
std::map<std::string, std::shared_ptr<tcp::socket>> waitingPlayers;
std::map<std::string, std::vector<std::string>> gameBoards;
std::map<std::string, std::string> playerSymbols;
std::map<std::string, std::string> playerOpponents;
std::map<std::string, std::shared_ptr<tcp::socket>> playerSockets;

// Reaction game state
std::map<std::string, std::shared_ptr<tcp::socket>> reactionWaiting;
std::map<std::string, std::shared_ptr<tcp::socket>> reactionPlayers;
std::string reactionWinner;

std::string extractField(const std::string& message, const std::string& field) {
    std::string key = "\"" + field + "\":\"";
    size_t start = message.find(key);
    if (start == std::string::npos) return "";
    start += key.length();
    size_t end = message.find("\"", start);
    if (end == std::string::npos) return "";
    return message.substr(start, end - start);
}

std::string extractReceiver(const std::string& message) {
    return extractField(message, "receiver");
}

bool userExists(const std::string& username) {
    std::ifstream file("users.txt");
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string user, pass;
        if (std::getline(iss, user, ',') && std::getline(iss, pass)) {
            if (user == username) {
                return true;
            }
        }
    }
    return false;
}

bool validateUser(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string user, pass;
        if (std::getline(iss, user, ',') && std::getline(iss, pass)) {
            if (user == username && pass == password) {
                return true;
            }
        }
    }
    return false;
}

bool registerUser(const std::string& username, const std::string& password) {
    if (userExists(username)) {
        return false;
    }

    std::ofstream file("users.txt", std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << username << "," << password << "\n";
    return true;
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
        std::string username = extractField(message, "username");
        std::string password = extractField(message, "password");

        std::string reply;

        if (validateUser(username, password)) {
            connectedUsers[username] = socket;
            currentUser = username;

            reply = R"({"type":"loginResult","status":"success","message":"Login successful."})";
            std::cout << username << " logged in.\n";
        } else {
            reply = R"({"type":"loginResult","status":"error","message":"Invalid username or password."})";
            std::cout << "Failed login attempt for user: " << username << "\n";
        }

        reply += "\n";

        co_await boost::asio::async_write(
            *socket,
            boost::asio::buffer(reply),
            use_awaitable
        );
}

    else if(type == "signup")
    {
            std::string username = extractField(message, "username");
            std::string password = extractField(message, "password");

            std::string reply;

            if (registerUser(username, password)) {
                
                connectedUsers[username] = socket;
                currentUser = username;
                
                reply = R"({"type":"signupResult","status":"success","message":"Signup successful."})";
                std::cout << username << " registered successfully.\n";
            } else {
                reply = R"({"type":"signupResult","status":"error","message":"Username already exists."})";
                std::cout << "Signup failed. Username already exists: " << username << "\n";
            }

            reply += "\n";

            co_await boost::asio::async_write(
                *socket,
                boost::asio::buffer(reply),
                use_awaitable
            );
    }
    else if(type == "logout")
    {
        std::string username = extractField(message, "username");
        if (!username.empty())
        {
            connectedUsers.erase(username);
            // Remove user from all rooms
            for (auto& roomPair : rooms) {
                roomPair.second.erase(username);
            }
            std::cout << username << " logged out.\n";
        }
        std::string reply = R"({"type":"logoutResult","status":"success","message":"Logged out."})";
        reply += "\n";
        co_await boost::asio::async_write(*socket,boost::asio::buffer(reply),use_awaitable);
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
        else if (type == "ticTacToeJoin") {
            std::string username = extractField(message, "sender");
            if (waitingPlayers.empty()) {
                waitingPlayers[username] = socket;
                std::string reply = R"({"type":"ticTacToeWait","message":"Waiting for opponent..."})";
                reply += "\n";
                co_await boost::asio::async_write(*socket, boost::asio::buffer(reply), use_awaitable);
            } else {
                auto it = waitingPlayers.begin();
                std::string opponent = it->first;
                std::shared_ptr<tcp::socket> opponentSocket = it->second;
                waitingPlayers.clear();

                std::vector<std::string> board(9, "");
                playerSymbols[username] = "O";
                playerSymbols[opponent] = "X";
                playerOpponents[username] = opponent;
                playerOpponents[opponent] = username;
                playerSockets[username] = socket;
                playerSockets[opponent] = opponentSocket;
                gameBoards[username] = board;
                gameBoards[opponent] = board;

                std::string msgX = R"({"type":"ticTacToeStart","payload":{"yourSymbol":"X","opponent":")" + username + R"(","turn":"X"}})";
                msgX += "\n";
                std::string msgO = R"({"type":"ticTacToeStart","payload":{"yourSymbol":"O","opponent":")" + opponent + R"(","turn":"X"}})";
                msgO += "\n";

                co_await boost::asio::async_write(*opponentSocket, boost::asio::buffer(msgX), use_awaitable);
                co_await boost::asio::async_write(*socket, boost::asio::buffer(msgO), use_awaitable);
            }
        }

        else if (type == "ticTacToeMove") {
            std::string username = extractField(message, "sender");
            std::string rowStr = extractField(message, "row");
            std::string colStr = extractField(message, "col");

            int row = std::stoi(rowStr);
            int col = std::stoi(colStr);

            int index = row * 3 + col;

            std::string symbol = playerSymbols[username];
            std::string opponent = playerOpponents[username];
            auto& board = gameBoards[username];
            board[index] = symbol;
            gameBoards[opponent] = board;

            std::string boardJson = "[";
            for (int i = 0; i < 9; i++) {
                boardJson += "\"" + board[i] + "\"";
                if (i < 8) boardJson += ",";
            }
            boardJson += "]";

            int wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
            bool won = false;
            for (auto& w : wins) {
                if (!board[w[0]].empty() && board[w[0]] == board[w[1]] && board[w[1]] == board[w[2]]) {
                    won = true; break;
                }
            }

            bool draw = !won;
            if (draw) {
                for (auto& cell : board) {
                    if (cell.empty()) { draw = false; break; }
                }
            }

            std::string nextTurn = (symbol == "X") ? "O" : "X";

            if (won) {
                std::string endMsg = R"({"type":"ticTacToeEnd","payload":{"winner":")" + username + R"("}})";
                endMsg += "\n";
                co_await boost::asio::async_write(*socket, boost::asio::buffer(endMsg), use_awaitable);
                co_await boost::asio::async_write(*playerSockets[opponent], boost::asio::buffer(endMsg), use_awaitable);
            } else if (draw) {
                std::string endMsg = R"({"type":"ticTacToeEnd","payload":{"winner":"draw"}})";
                endMsg += "\n";
                co_await boost::asio::async_write(*socket, boost::asio::buffer(endMsg), use_awaitable);
                co_await boost::asio::async_write(*playerSockets[opponent], boost::asio::buffer(endMsg), use_awaitable);
            } else {
                std::string updateMsg = R"({"type":"ticTacToeUpdate","payload":{"board":)" + boardJson + R"(,"turn":")" + nextTurn + R"("}})";
                updateMsg += "\n";
                co_await boost::asio::async_write(*socket, boost::asio::buffer(updateMsg), use_awaitable);
                co_await boost::asio::async_write(*playerSockets[opponent], boost::asio::buffer(updateMsg), use_awaitable);
            }
        }

        else if (type == "reactionGameJoin") {
            std::string username = extractField(message, "sender");
            reactionWaiting[username] = socket;
            reactionPlayers[username] = socket;

            if (reactionWaiting.size() >= 2) {
                std::string goMsg = R"({"type":"reactionGo"})";
                goMsg += "\n";
                for (auto& p : reactionWaiting) {
                    co_await boost::asio::async_write(*p.second, boost::asio::buffer(goMsg), use_awaitable);
                }
                reactionWaiting.clear();
            } else {
                std::string waitMsg = R"({"type":"reactionWait","message":"Waiting for opponent..."})";
                waitMsg += "\n";
                co_await boost::asio::async_write(*socket, boost::asio::buffer(waitMsg), use_awaitable);
            }
        }

        else if (type == "reactionResponse") {
            std::string username = extractField(message, "sender");
            if (reactionWinner.empty()) {
                reactionWinner = username;
                std::string endMsg = R"({"type":"reactionEnd","payload":{"winner":")" + username + R"("}})";
                endMsg += "\n";
                for (auto& p : reactionPlayers) {
                    co_await boost::asio::async_write(*p.second, boost::asio::buffer(endMsg), use_awaitable);
                }
                reactionPlayers.clear();
                reactionWinner = "";
            }
        }

        else if (type == "fetchScoreboard") {
            std::ifstream scoreFile("scores.txt");
            std::string line;
            std::string response = "{\"type\":\"scoreboardResponse\",\"payload\":[";
            bool first = true;
            while (std::getline(scoreFile, line)) {
                if (!line.empty()) {
                    if (!first) response += ",";
                    response += "\"" + line + "\"";
                    first = false;
                }
            }
            response += "]}\n";
            co_await boost::asio::async_write(*socket, boost::asio::buffer(response), use_awaitable);
        }


else if (type == "fetchOnlineUsers") {
std::string response = "{\"type\":\"onlineUsersResponse\",\"sender\":\"server\",\"payload\":[";

bool first = true;
for (const auto& pair : connectedUsers) {
if (!first) {
response += ",";
}

response += "\"" + pair.first + "\"";
first = false;
}

response += "]}\n";

co_await boost::asio::async_write(
*socket,
boost::asio::buffer(response),
use_awaitable
);
}
else if (type == "createRoom") {
std::string roomName = extractField(message, "roomName");
std::string username = extractField(message, "sender");

std::string reply;
if (rooms.find(roomName) == rooms.end()) {
rooms[roomName] = std::set<std::string>();
rooms[roomName].insert(username);
reply = R"({"type":"createRoomResult","status":"success","message":"Room created."})";
std::cout << username << " created room: " << roomName << "\n";
} else {
reply = R"({"type":"createRoomResult","status":"error","message":"Room already exists."})";
}

reply += "\n";
co_await boost::asio::async_write(*socket, boost::asio::buffer(reply), use_awaitable);
}
else if (type == "joinRoom") {
std::string roomName = extractField(message, "roomName");
std::string username = extractField(message, "sender");

std::string reply;
if (rooms.find(roomName) != rooms.end()) {
rooms[roomName].insert(username);
reply = R"({"type":"joinRoomResult","status":"success","message":"Joined room."})";
std::cout << username << " joined room: " << roomName << "\n";
} else {
reply = R"({"type":"joinRoomResult","status":"error","message":"Room does not exist."})";
}

reply += "\n";
co_await boost::asio::async_write(*socket, boost::asio::buffer(reply), use_awaitable);
}
else if (type == "leaveRoom") {
std::string roomName = extractField(message, "roomName");
std::string username = extractField(message, "sender");

std::string reply;
if (rooms.find(roomName) != rooms.end() && rooms[roomName].find(username) != rooms[roomName].end()) {
rooms[roomName].erase(username);
reply = R"({"type":"leaveRoomResult","status":"success","message":"Left room."})";
std::cout << username << " left room: " << roomName << "\n";
} else {
reply = R"({"type":"leaveRoomResult","status":"error","message":"Not in room or room does not exist."})";
}

reply += "\n";
co_await boost::asio::async_write(*socket, boost::asio::buffer(reply), use_awaitable);
}
else if (type == "roomMessage") {
std::string roomName = extractField(message, "roomName");
std::string username = extractField(message, "sender");

if (rooms.find(roomName) != rooms.end() && rooms[roomName].find(username) != rooms[roomName].end()) {
for (const std::string& user : rooms[roomName]) {
if (user != username) {
auto it = connectedUsers.find(user);
if (it != connectedUsers.end() && it->second && it->second->is_open()) {
std::string outgoing = message;

if (!outgoing.empty() && outgoing.back() != '\n') {
outgoing += '\n';
}
co_await boost::asio::async_write(*it->second, boost::asio::buffer(outgoing), use_awaitable);
}
}
}
}
}
else if (type == "fetchRooms") {
std::string response = "{\"type\":\"roomsResponse\",\"payload\":[";
bool first = true;
for (const auto& pair : rooms) {
if (!first) response += ",";
response += "\"" + pair.first + "\"";
first = false;
}
response += "]}\n";
co_await boost::asio::async_write(*socket, boost::asio::buffer(response), use_awaitable);
}
else if (type == "fetchRoomUsers") {
std::string roomName = extractField(message, "roomName");
std::string response = "{\"type\":\"roomUsersResponse\",\"room\":\"" + roomName + "\",\"payload\":[";
if (rooms.find(roomName) != rooms.end()) {
bool first = true;
for (const std::string& user : rooms[roomName]) {
if (!first) response += ",";
response += "\"" + user + "\"";
first = false;
}
}
response += "]}\n";
co_await boost::asio::async_write(*socket, boost::asio::buffer(response), use_awaitable);
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
// Remove user from all rooms
for (auto& roomPair : rooms) {
roomPair.second.erase(currentUser);
}
std::cout << currentUser << " removed from connected users.\n";
}

break;
}
}
}

// Coroutine to listen for incoming connections
awaitable<void> listener() {
auto io_ctx = co_await boost::asio::this_coro::executor;

tcp::acceptor acceptor(io_ctx, { tcp::v4(), 54321 });

std::cout << "Server is listening on 127.0.0.1: port 54321...\n";

while (true) {
auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

if (!ec) {
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

co_spawn(io_context, listener(), detached);

io_context.run();

return 0;
}
