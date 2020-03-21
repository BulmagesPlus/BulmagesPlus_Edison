#include "blapimessage.h"

BlApiMessage::BlApiMessage()
{
}


BlApiMessage::BlApiMessage(const BlApiMessage &other)
{
  id = other.id;
  connection = other.connection;
  payload = other.payload;
  errorCode = other.errorCode;
  errorText = other.errorText;
}


BlApiMessage::~BlApiMessage()
{
}

