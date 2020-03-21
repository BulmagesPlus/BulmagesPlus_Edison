#include <memory>
#include <QDebug>
#include "blapiserverworker.h"
#include "blapiserver.h"


BlApiServerWorker::BlApiServerWorker(BlApiServer *apiServer) : m_apiServer = apiServer
{
}


BlApiServerWorker::~BlApiServerWorker()
{
}


BlApiServer *BlApiServerWorker::apiServer()
{
    return m_apiServer;
}


void BlApiServerWorker::outputMessage(BlApiMessage apiMessage)
{
    auto webSocket {static_cast<ix::WebSocket *>(apiMessage.connection)};

    qDebug() << "oooutput";
    webSocket->send(apiMessage.payload.toStdString());
}


ix::WebSocketServer *BlApiServerWorker::webSocketServer()
{
    return m_webSocketServer;
}


void BlApiServerWorker::process()
{
    // Process. Start processing data.
    // allocate resources using new here
    qDebug("Running server");


    // Run a server on localhost at a given port.
    // Bound host name, max connections and listen backlog can also be passed in as parameters.
//    ix::WebSocketServer server(9990, "0.0.0.0");
    m_webSocketServer = new ix::WebSocketServer(9990, "0.0.0.0");


    //ix::SocketTLSOptions tlsOptions;
    //tlsOptions.certFile = "./cert/correcaminos.crt";
    //tlsOptions.keyFile =  "./cert/correcaminos.key";
    //tlsOptions.caFile =   "./cert/correcaminos_ca.pem";

    //server.setTLSOptions(tlsOptions);

    m_webSocketServer->setOnConnectionCallback(
        [this](std::shared_ptr<ix::WebSocket> webSocket,
                  std::shared_ptr<ix::ConnectionState> connectionState)
        {
            webSocket->setOnMessageCallback(
                [webSocket, connectionState, this](const ix::WebSocketMessagePtr msg)
                {
                    if (msg->type == ix::WebSocketMessageType::Open)
                    {
                        qDebug() << "New connection";

                        // A connection state object is available, and has a default id
                        // You can subclass ConnectionState and pass an alternate factory
                        // to override it. It is useful if you want to store custom
                        // attributes per connection (authenticated bool flag, attributes, etc...)
                        //qDebug() << "id: " << connectionState->getId();

                        // The uri the client did connect to.
                        //qDebug() << "Uri: " << msg->openInfo.uri;

                        qDebug() << "Headers:";
                        for (auto it : msg->openInfo.headers)
                        {
                            //qDebug() << it.first << ": " << it.second;
                        }
                    }
                    else if (msg->type == ix::WebSocketMessageType::Close)
                    {
                        qDebug() << "Close connection";
                    }
                    else if (msg->type == ix::WebSocketMessageType::Message)
                    {
                        // For an echo server, we just send back to the client whatever was received by the server
                        // All connected clients are available in an std::set. See the broadcast cpp example.
                        // Second parameter tells whether we are sending the message in binary or text mode.
                        // Here we send it in the same mode as it was received.
//                    webSocket->send(msg->str, msg->binary);
                        qDebug() << QString::fromStdString(msg->str) << bgpApp()->jobs();
                        /// TODO: Call main thread function and return value then send back to
                        /// websocket client.
                        /// If the request is too quickly will send an error to client.

                        /// check status
//                        if (bgpApp()->jobs() == 0)
                        if (bgpApp()->jobs() < 3)
                        {
                          BlApiMessage apiMessage;
                          apiMessage.connection = webSocket.get();
                          apiMessage.payload = QString::fromStdString(msg->str);
                          emit inputMessage(apiMessage);
                          webSocket->send("received", msg->binary);
                        }
                        else
                        {
                          webSocket->send("Too quickly", msg->binary);
                        } // end if
                    }
                }
            );
        }
    );

    auto res = m_webSocketServer->listen();
    if (!res.first)
    {
        // Error handling
        return;
    }

    // Run the server in the background. Server can be stoped by calling server.stop()
    m_webSocketServer->start();
    qDebug() << "started";
}

