#ifndef BLAPISERVER_H
#define BLAPISERVER_H

#include <QObject>
#include <QString>
#include <ixwebsocket/IXWebSocketServer.h>
#include "blapimessage.h"
#include "bldefs.h"

class BlApiServer;


class BL_EXPORT BlApiServerWorker : public QObject
{
  Q_OBJECT

  BlApiServer *m_apiServer {nullptr};
  ix::WebSocketServer *m_webSocketServer {nullptr};

  public:
    BlApiServerWorker(BlApiServer *apiServer);
    ~BlApiServerWorker();
    BlApiServer *apiServer();
    ix::WebSocketServer *webSocketServer();

  public slots:
    void process();
    void outputMessage(BlApiMessage apiMessage);

  signals:
    void finished();
    void error(QString err);
    void inputMessage(BlApiMessage apiMessage);
};

#endif
