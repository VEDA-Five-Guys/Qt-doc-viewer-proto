#include "main_window.h"
#include "websocket_server.h"

#include <QApplication>
#include <QThread>
#include <QRegularExpression>

void manual(Main_Window *w, const QString &message);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Main_Window w;
    w.show();

    net::io_context ioc;

    WebSocketServer *server = new WebSocketServer(ioc, tcp::endpoint{ net::ip::make_address("0.0.0.0"), 8080 });

    QThread *serverThread = new QThread;
    server->moveToThread(serverThread);

    // ioc 캡쳐 문제...?
    QObject::connect(serverThread, &QThread::started, [&ioc]() {
        ioc.run();
    });

    QObject::connect(server, &WebSocketServer::message_received, &w, [&w](const QString &message){
        manual(&w, message);
    });

    serverThread->start();

    return app.exec();
}

void manual(Main_Window *w, const QString &message){
    static const QRegularExpression re("^(\\w+)\\((\\d+), (\\d+)\\)$");
    QRegularExpressionMatch match = re.match(message);
    if(match.hasMatch()){
        QString str = match.captured(1);
        w->set_paint_mode(str == "pointing" ? 0 : (str == "drawing" ? 1 : -1));
        qDebug() << str;

        int x = match.captured(2).toInt();
        int y = match.captured(3).toInt();
        w->set_pos(x, y);

        return;
    }

    if(message == "left"){
        w->action_prev_page_triggered();
    }
    else if(message == "right"){
        w->action_next_page_triggered();
    }
    else if(message == "ON"){
        w->action_full_screen_triggered();
    }
    else if(message == "OFF"){
        w->restore_from_full_screen();
    }
}
