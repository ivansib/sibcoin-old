#include "callbackdbforgui.h"

CallBackDbForGui *CallBackDbForGui::pSingleton = nullptr;
int CallBackDbForGui::nCounter = 0;

CallBackDbForGui::CallBackDbForGui()
{
    qRegisterMetaType<TypeTable>("TypeTable");
    qRegisterMetaType<TypeTableOperation>("TypeTableOperation");
    qRegisterMetaType<StatusTableOperation>("StatusTableOperation");
}

void CallBackDbForGui::finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    this->tableOperationFinished(table, operation, status);
}
