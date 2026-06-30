#pragma once

// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_BLE : Objet applicatif permettant la gestion de la communication via Bluetooth Low Energy (BLE)
/// </summary>
class ROMV2_BLE
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_BLE();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="appType"></param>
    /// <param name="bluetoothConnected"></param>
    /// <param name="dataEnvironment"></param>
    /// <param name="dataLuminosity"></param>
    /// <param name="dataSkyState"></param>
    void Init(APP_TYPE appType,
        bool* bluetoothConnected,
        DataSensorEnvironment* dataEnvironment,
        DataSensorLuminosity* dataLuminosity,
        DataSensorSkyState* dataSkyState);

    /// <summary>
    /// Update des caractéristiques BLE et Notification aux clients BLE
    /// </summary>
    void Notify();

  private:
    // Fonctions

    // Instanciation des objets internes
    BLEServer*          _bleServer                          = nullptr;
    BLEService*         _bleService                         = nullptr;
    BLECharacteristic*  _bleCharacteristicTemperature       = nullptr;
    BLECharacteristic*  _bleCharacteristicHumidity          = nullptr;
    BLECharacteristic*  _bleCharacteristicPressure          = nullptr;
    BLECharacteristic*  _bleCharacteristicDewPoint          = nullptr;
    BLECharacteristic*  _bleCharacteristicCloudCover        = nullptr;
    BLECharacteristic*  _bleCharacteristicSkyBrightness     = nullptr;
    BLECharacteristic*  _bleCharacteristicSkyQuality        = nullptr;
    BLECharacteristic*  _bleCharacteristicSkyTemperature    = nullptr;

    // Membres internes
    APP_TYPE                _appType                        = APP_ROMV2;
    String                  _deviceName                     = BLE_ROMV2_DEVICE_NAME;
    String                  _deviceUUID                     = BLE_ROMV2_SERVICE_UUID;
    DataSensorEnvironment*  _dataEnvironment                = nullptr;
    DataSensorLuminosity*   _dataLuminosity                 = nullptr;
    DataSensorSkyState*     _dataSkyState                   = nullptr;
    bool*                   _bluetoothConnected             = nullptr;
    char                    characteristicBuffer[128];

    // Chronos
    long _chronoReadBluetoothLE;                                               // Chrono pour lecture de l'état du Bluetooth
};