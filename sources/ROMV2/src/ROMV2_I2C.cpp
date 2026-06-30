#include <ROMV2_I2C.h>

/// ----------------------------
/// Définition du membre statique
/// ----------------------------
SemaphoreHandle_t ROMV2_I2C::_mutex = nullptr;

/// <summary>
/// Crée le mutex (idempotent : ne fait rien s'il existe déjà).
/// </summary>
void ROMV2_I2C::Init()
{
    if (_mutex == nullptr)
        _mutex = xSemaphoreCreateMutex();
}

/// <summary>
/// Renvoie le handle du mutex.
/// </summary>
SemaphoreHandle_t ROMV2_I2C::Handle()
{
    return _mutex;
}