#include "main.hpp"

// Read a message from the stream named Streamname
std::string ReadMessage(redisContext *c, std::string StreamName)
{
  std::string result;
  redisReply *r;

  // Read
  r = RedisCommand(c, "XREADGROUP GROUP monitor monitor BLOCK %d COUNT 1 NOACK STREAMS %s >",
                    BLOCK, StreamName.c_str());

  assertReplyType(c, r, REDIS_REPLY_ARRAY);
  result = r->element[0]->element[1]->element[0]->element[1]->element[1]->str;
  freeReplyObject(r);
  return result;
}

std::string ReadInfo(redisContext *c, std::string groupName)
{
    // Stream0 is used to receive various information, in this case the number of streams
    redisReply *r = RedisCommand(c, "XREADGROUP GROUP %s monitor BLOCK %d COUNT 1 STREAMS %s >",
                                 groupName.c_str(),BLOCK, "INFOSTREAM");
    assertReplyType(c,r,REDIS_REPLY_ARRAY);
    std::string result = r->element[0]->element[1]->element[0]->element[1]->element[1]->str;
    std::string ID = r->element[0]->element[1]->element[0]->element[0]->str;
    freeReplyObject(r);

    r = RedisCommand(c, "XACK %s mean %s","INFOSTREAM",ID.c_str());
    assertReply(c,r);
    freeReplyObject(r);

    return result;
}

void SendMessage(redisContext *c, std::string arr, std::string StreamName)
{

    redisReply *r = RedisCommand(c, "XADD %s * value %s", StreamName.c_str(), arr.c_str());
    assertReplyType(c, r, REDIS_REPLY_STRING);
    freeReplyObject(r);
  
}