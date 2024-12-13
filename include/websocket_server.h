#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <QObject>
#include <memory>
#include <string>
#include <iostream>
#include <QDebug>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class Session : public QObject, public std::enable_shared_from_this<Session> {
    Q_OBJECT

    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;

public:
    explicit Session(tcp::socket socket, QObject *parent = nullptr)
        : QObject(parent), ws_(std::move(socket)) {}

    void run() {
        ws_.async_accept([self = shared_from_this()](beast::error_code ec) {
            if (!ec) {
                self->do_read();
            }
        });
    }

signals:
    void message_received(const QString &message);

private:
    void do_read() {
        ws_.async_read(buffer_, [self = shared_from_this()](beast::error_code ec, size_t bytes_transferred){
            boost::ignore_unused(bytes_transferred);
            if (!ec) {
                std::string msg = beast::buffers_to_string(self->buffer_.data());
                emit self->message_received(QString::fromStdString(msg));
                self->buffer_.consume(self->buffer_.size());
                self->do_read();
            } else if (ec == websocket::error::closed) {
                qDebug() << "Connection closed\n";
            } else {
                std::cerr << "Read error : " << ec.message() << std::endl;

            }
        });
    }
};

class WebSocketServer : public QObject {
    Q_OBJECT
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    WebSocketServer(net::io_context& ioc, tcp::endpoint endpoint, QObject *parent = nullptr)
        : QObject(parent), ioc_(ioc), acceptor_(ioc) {
        beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if (ec) throw beast::system_error(ec);

        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) throw beast::system_error(ec);

        acceptor_.bind(endpoint, ec);
        if (ec) throw beast::system_error(ec);

        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec) throw beast::system_error(ec);

        do_accept();
    }

signals:
    void message_received(const QString &message);

private:
    void do_accept() {
        acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket));
                connect(session.get(), &Session::message_received, this, &WebSocketServer::message_received);
                session->run();
            }

            do_accept();
        });
    }
};


#endif // WEBSOCKET_SERVER_H
