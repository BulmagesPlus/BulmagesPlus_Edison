#ifndef BLAPISERVER_H
#define BLAPISERVER_H

#include <QObject>
#include "blapimessage.h"
#include "bldefs.h"

class QWidget;
class QPushButton;
class QThread;
class BlApiServerWorker;


class BL_EXPORT BlApiServer : public QObject
{
  Q_OBJECT

  private:
    QPushButton *m_pushButton1 {nullptr};
    QThread* m_apiServerThread {nullptr};
    QThread* m_apiJobThread {nullptr}; // TODO. more than one
    BlApiServerWorker* m_apiServerWorker {nullptr};

    int m_activeJobs {0};
    void function();

  signals:
    void outputMessage(APIMessage apiMessage);

  public:
    BlApiServer(QObject *parent = nullptr);
    int activeJobs();

  private slots:
    void startApiServer();
    void startApiJob(BlApiMessage apiMessage);
};

#endif
