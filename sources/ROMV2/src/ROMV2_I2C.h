#pragma once

// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_I2C : Verrou partagé du bus I2C (Wire).
/// Protège le bus contre les accès concurrents entre la tâche TSL2591 (coeur 0)
/// et les lectures BME280 / MLX90614 / ADXL345 effectuées dans Loop() (coeur 1),
/// qui s'exécutent sur des coeurs différents et peuvent donc viser le bus
/// véritablement en parallèle.
/// </summary>
class ROMV2_I2C
{
public:
    /// <summary>
    /// Crée le mutex (idempotent).
    /// </summary>
    static void Init();

    /// <summary>
    /// Renvoie le handle du mutex (utilisé par le garde I2CLock).
    /// </summary>
    static SemaphoreHandle_t Handle();

private:
    // Membre statique : un seul bus I2C => un seul verrou pour tout le projet
    static SemaphoreHandle_t _mutex;
};

/// <summary>
/// Garde RAII : prend le verrou I2C à la construction, le rend automatiquement
/// à la destruction (fin de scope). Évite tout xSemaphoreGive() oublié, y compris
/// sur un return anticipé.
///
/// Usage :
///   {
///       I2CLock lock(pdMS_TO_TICKS(15));
///       if (!lock.ok()) return;        // bus occupé -> on saute la lecture
///       ... transaction(s) I2C ...
///   }                                  // verrou rendu ici, automatiquement
/// </summary>
class I2CLock
{
public:
    /// <summary>
    /// Tente de prendre le verrou avec un timeout. ok() indique le succès.
    /// </summary>
    /// <param name="timeout">Délai d'attente max (ex. pdMS_TO_TICKS(15))</param>
    explicit I2CLock(TickType_t timeout)
    {
        // Défensif : si Init() n'a pas encore tourné, on considère le verrou non pris
        SemaphoreHandle_t handle = ROMV2_I2C::Handle();
        _taken = (handle != nullptr) && (xSemaphoreTake(handle, timeout) == pdTRUE);
    }

    /// <summary>
    /// Libère le verrou s'il avait été pris.
    /// </summary>
    ~I2CLock()
    {
        if (_taken)
            xSemaphoreGive(ROMV2_I2C::Handle());
    }

    /// <summary>
    /// True si le verrou a bien été pris dans le délai imparti.
    /// </summary>
    bool ok() const { return _taken; }

    // Un verrou ne se copie pas
    I2CLock(const I2CLock&) = delete;
    I2CLock& operator=(const I2CLock&) = delete;

private:
    bool _taken;
};
