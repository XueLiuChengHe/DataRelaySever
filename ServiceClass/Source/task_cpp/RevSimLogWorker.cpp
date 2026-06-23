#include "task_h/RevSimLogWorker.h"


RevSimLogWorker::RevSimLogWorker(unsigned short Port, QObject *parent)
    : QObject(parent)
    , httpFileSever(nullptr)
    , Port(Port)
{

}

RevSimLogWorker::~RevSimLogWorker()
{
    if(this->httpFileSever != nullptr){
        this->httpFileSever->deleteLater();
        this->httpFileSever = nullptr;
    }
}

void RevSimLogWorker::working()
{
    if(this->httpFileSever == nullptr){
        this->httpFileSever = new HttpSever(this->Port);
        QObject::connect(this->httpFileSever,&HttpSever::simLogRev,this,&RevSimLogWorker::simLogPaurseFinished);
    }
}
