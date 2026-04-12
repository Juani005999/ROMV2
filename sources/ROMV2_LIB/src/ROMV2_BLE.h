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
    /// <param name="bluetoothConnected"></param>
    /// <param name="dataEnvironment"></param>
    /// <param name="dataLuminosity"></param>
    /// <param name="dataSkyState"></param>
    void Init(bool* bluetoothConnected, DataSensorEnvironment* dataEnvironment, DataSensorLuminosity* dataLuminosity, DataSensorSkyState* dataSkyState);

    /// <summary>
    /// Update des caractéristiques BLE et Notification aux clients BLE
    /// </summary>
    void Notify();

  private:
    // Fonctions

    // Instanciation des objets internes
    BLEServer* _bleServer = NULL;
    BLEService* _bleService = NULL;
    BLECharacteristic* _bleCharacteristicTemperature;
    BLECharacteristic* _bleCharacteristicHumidity;
    BLECharacteristic* _bleCharacteristicPressure;
    BLECharacteristic* _bleCharacteristicDewPoint;
    BLECharacteristic* _bleCharacteristicCloudCover;
    BLECharacteristic* _bleCharacteristicSkyBrightness;
    BLECharacteristic* _bleCharacteristicSkyQuality;
    BLECharacteristic* _bleCharacteristicSkyTemperature;

    // Membres internes
    String _deviceName = BLE_DEVICE_NAME;
    DataSensorEnvironment* _dataEnvironment;
    DataSensorLuminosity* _dataLuminosity;
    DataSensorSkyState* _dataSkyState;
    bool* _bluetoothConnected;
    char characteristicBuffer[128];

    // Chronos
    long _chronoReadBluetoothLE;                                               // Chrono pour lecture de l'état du Bluetooth
};