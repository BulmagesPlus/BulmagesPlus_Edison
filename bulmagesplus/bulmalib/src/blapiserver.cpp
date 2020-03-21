#include <QThread>
#include <QtConcurrent>
#include <QWidget>
#include <QDebug>
#include <QTimer>
#include "blapiserver.h"
#include "blapiserverworker.h"


BlApiServer::BlApiServer(QObject *parent) : QObject(parent)
{
    QTimer::singleShot(0, this, SLOT(startApiServer()));
}


int BlApiServer::activeJobs()
{
  return m_activeJobs;
}


void BlApiServer::startApiServer()
{
    m_apiServerThread = new QThread;
    m_apiServerWorker = new BlApiServerWorker(this);
    m_apiServerWorker->moveToThread(m_apiServerThread);
    connect(m_apiServerThread, SIGNAL (started()), m_apiServerWorker, SLOT (process()));
    connect(m_apiServerWorker, SIGNAL (finished()), m_apiServerThread, SLOT (quit()));
    connect(m_apiServerWorker, SIGNAL (finished()), m_apiServerWorker, SLOT (deleteLater()));
    connect(m_apiServerThread, SIGNAL (finished()), m_apiServerThread, SLOT (deleteLater()));
    connect(m_apiServerWorker, SIGNAL (inputMessage(BlApiMessage)), this, SLOT (startApiJob(BlApiIMessage)));
    connect(this, SIGNAL (outputMessage(BlApiMessage)), m_apiServerWorker, SLOT (outputMessage(BlApiMessage)));
    m_apiServerThread->start();
}


void BlApiServer::startApiJob(BlApiMessage apiMessage)
{
    m_jobs++;

    QtConcurrent::run([=](){

      qDebug() << apiMessage.payload;

      qDebug() << "go to output1";

      for (int i = 0; i < 2; i++)
      {
        qDebug() << "toc";
        sleep(10);
      } // end for

      function();

      qDebug() << "go to output2";
      BlApiMessage apiMessageOut;
      apiMessageOut.connection = apiMessage.connection;
      apiMessageOut.payload = QString("aaaa");
      emit outputMessage(apiMessageOut);
      qDebug() << "go to output3";
      m_jobs--;
    });

}


void BlApiServer::function()
{
  qDebug() << "call function";
}

