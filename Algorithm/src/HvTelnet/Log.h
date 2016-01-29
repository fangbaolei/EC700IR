#ifndef LOG_H_
#define LOG_H_

#include <pthread.h>
#include <string>
#include <list>

#ifndef OUT
#define OUT
#endif

#ifndef IN
#define IN
#endif

typedef struct tag_LogData {
    int nLevel;
    std::string strData;
} LogData;

class Log
{
public:
    Log();
    //Log(unsigned int nBufMax);
    virtual ~Log();

    //
    void Write(int nLevel, char* szData, bool bLock = true);

    // pPos 为输入输出参数指针，输入值为要读的数据起始下标。从头开始时为0
    //      读完后置该参数为下一条数据起始位置
    // 返回值 读到的数据长度
    unsigned int Read(OUT int* pLevel, OUT char* szData, IN OUT unsigned int* pPos);

    // lock write
    void Lock();

    // unlock write
    void Unlock();

protected:
    std::list<LogData> m_LogData;
    pthread_mutex_t m_WriteMutex; // m_LogData读写互斥
};

#endif /* LOG_H_ */
