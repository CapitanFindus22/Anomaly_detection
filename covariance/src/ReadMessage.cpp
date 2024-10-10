#include "main.hpp"

extern std::atomic<size_t> done;
extern std::atomic<bool> running;
extern size_t windowLength;
std::mutex redisMutex;

// Read a message from the stream named Streamname
void ReadMessage(redisContext *c, std::string StreamName, Con2DB db, int id, std::deque<float>& arr)
{
  float val;
  std::string strValue;
  redisReply *r;

  arr.clear();

  while (1)
  {

    /*Since this function is used for the threads and redisContext* c
      isn't thread safe the mutex is essential
    */
    {
      std::lock_guard<std::mutex> lock(redisMutex);

      // Read
      r = RedisCommand(c, "XREADGROUP GROUP reader r2 BLOCK %d COUNT 1 STREAMS %s >",
                       BLOCK, StreamName.c_str());

      assertReplyType(c, r, REDIS_REPLY_ARRAY);
    }

    strValue = r->element[0]->element[1]->element[0]->element[1]->element[1]->str;

    freeReplyObject(r);

    val = std::stof(strValue);

    arr.push_back(val);

    if (arr.size() > windowLength)
    {
      arr.pop_front();
    }

    if (arr.size() == windowLength)
    {
      done.fetch_add(1,std::memory_order_relaxed);
      while(done.load()>0){std::this_thread::yield();}
    }
  }
}
