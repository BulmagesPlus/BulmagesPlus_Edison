#ifndef BLAPIMESSAGE_H
#define BLAPIMESSAGE_H

#include <QString>
#include <QMetaType>
#include "bldefs.h"

class BL_EXPORT BlApiMessage
{
    public:
      BlApiMessage();
      BlApiMessage(const BlApiMessage &other);
      ~BlApiMessage();

      unsigned int id {0};
      void *connection {nullptr};
      QString payload {""};
      int errorCode {0};
      QString errorText {""};
};

Q_DECLARE_METATYPE(BlApiMessage);

#endif
