#include <ROMV2_BLE.h>

// Variable globale permettant la gestion de l'état de connexion du BLE
bool bleDeviceConnected = false;
bool shouldStartAdvertising = false;
BLEDescriptor _bleCharacteristicTemperatureDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor _bleCharacteristicHumidityDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor _bleCharacteristicPressureDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor _bleCharacteristicDewPointDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor _bleCharacteristicCloudCoverDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor _bleCharacteristicSkyBrightnessDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor _bleCharacteristicSkyQualityDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor _bleCharacteristicSkyTemperatureDescriptor(BLEUUID((uint16_t)0x2901));

/// <summary>
/// Callback permettant la gestion de la connection du service BLE
/// </summary>
class ROMV2_BLEServiceCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* bleServer) {
        // MAJ des Flags
        bleDeviceConnected = true;

        // Trace
        debugln(F(""));
        debugln(F("[BLUETOOTH] Device connected"));
    };

    void onDisconnect(BLEServer* bleServer) {
        // MAJ des Flags
        bleDeviceConnected = false;

        // Positionnement du flag de redémarrage du mode "Advertising" aux clients BLE
        shouldStartAdvertising = true;

        // Trace
        debugln(F(""));
        debugln(F("[BLUETOOTH] Device disconnected"));
    }
};

/// <summary>
/// Constructeur
/// </summary>
ROMV2_BLE::ROMV2_BLE()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="bluetoothConnected"></param>
/// <param name="dataEnvironment"></param>
/// <param name="dataLuminosity"></param>
/// <param name="dataSkyState"></param>
void ROMV2_BLE::Init(bool* bluetoothConnected, DataSensorEnvironment* dataEnvironment, DataSensorLuminosity* dataLuminosity, DataSensorSkyState* dataSkyState)
{
    // Valorisation des champs internes
    _bluetoothConnected = bluetoothConnected;
    _dataEnvironment = dataEnvironment;
    _dataLuminosity = dataLuminosity;
    _dataSkyState = dataSkyState;

    // Trace
    debugln(F(""));
    debugln(F("[BLUETOOTH] Starting initialisation"));

    // Initialisation Bluetooth
    BLEDevice::init(_deviceName);
    _bleServer = BLEDevice::createServer();
    _bleServer->setCallbacks(new ROMV2_BLEServiceCallbacks());
    _bleService = _bleServer->createService(BLEUUID(BLE_SERVICE_UUID), BLE_CHARACTERISTIC_NUMHANDLES_COUNT);

    // Création des caractéristiques BLE
    
    // Température : Temperature
    _bleCharacteristicTemperature = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_TEMPERATURE, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicTemperatureDescriptor.setValue("Temperature [Celsius]");
    _bleCharacteristicTemperature->addDescriptor(&_bleCharacteristicTemperatureDescriptor);

    // Humidité : Humidity
    _bleCharacteristicHumidity = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_HUMIDITY, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicHumidityDescriptor.setValue("Humidite [pct]");
    _bleCharacteristicHumidity->addDescriptor(&_bleCharacteristicHumidityDescriptor);

    // Pression atmosphérique : Pressure
    _bleCharacteristicPressure = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_PRESSURE, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicPressureDescriptor.setValue("Pressure [Pa]");
    _bleCharacteristicPressure->addDescriptor(&_bleCharacteristicPressureDescriptor);

    // Point de rosée : DewPoint
    _bleCharacteristicDewPoint = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_DEWPOINT, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicDewPointDescriptor.setValue("DewPoint [Celsius]");
    _bleCharacteristicDewPoint->addDescriptor(&_bleCharacteristicDewPointDescriptor);

    // Couverture du Ciel : CloudCover
    _bleCharacteristicCloudCover = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_CLOUDCOVER, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicCloudCoverDescriptor.setValue("CloudCover [pct]");
    _bleCharacteristicCloudCover->addDescriptor(&_bleCharacteristicCloudCoverDescriptor);

    // Luminosité : SkyBrightness
    _bleCharacteristicSkyBrightness = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_SKYBRIGHTNESS, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicSkyBrightnessDescriptor.setValue("SkyBrightness [lux]");
    _bleCharacteristicSkyBrightness->addDescriptor(&_bleCharacteristicSkyBrightnessDescriptor);

    // Qualité du ciel : SkyQuality
    _bleCharacteristicSkyQuality = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_SKYQUALITY, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicSkyQualityDescriptor.setValue("SkyQuality [Mg/Asec^2]");
    _bleCharacteristicSkyQuality->addDescriptor(&_bleCharacteristicSkyQualityDescriptor);

    // Température du ciel : SkyTemperature
    _bleCharacteristicSkyTemperature = _bleService->createCharacteristic(BLE_CHARACTERISTIC_UUID_SKYTEMPERATURE, BLECharacteristic::PROPERTY_NOTIFY);
    _bleCharacteristicSkyTemperatureDescriptor.setValue("SkyTemperature [Celsius]");
    _bleCharacteristicSkyTemperature->addDescriptor(&_bleCharacteristicSkyTemperatureDescriptor);

    // Démarrage du Service BLE
    _bleService->start();

    debugln(F("[BLUETOOTH] Ending initialisation"));

    // Démarrage du mode "Advertising" aux clients BLE
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    _bleServer->getAdvertising()->start();

    // MAJ des flags
    *_bluetoothConnected = false;

    // Trace
    debugln(F(""));
    debugln(F("[BLUETOOTH] Started advertising. Waiting a client connection to notify..."));
}

/// <summary>
/// Update des caractéristiques BLE et Notification aux clients BLE
/// </summary>
void ROMV2_BLE::Notify()
{
    if (millis() > _chronoReadBluetoothLE + BLE_NOTIFY_INTERVAL)
    {
        // Relance l'Advertising après déconnection
        if (shouldStartAdvertising) {
            // Petit délai pour laisser le BLE stack se stabiliser
            delay(500);
            BLEDevice::startAdvertising();
            shouldStartAdvertising = false;

            // Trace
            debugln(F(""));
            debugln(F("[BLUETOOTH] Started advertising. Waiting a client connection to notify..."));
        }
        // Si connecté, on lance les notifications
        else if (bleDeviceConnected)
        {
            debugln(F(""));
            debugln(F("[BLUETOOTH] Starting BLE notifications"));

            // Température
            sprintf(characteristicBuffer, "%.2f", _dataEnvironment->temperature);
            _bleCharacteristicTemperature->setValue(characteristicBuffer);
            _bleCharacteristicTemperature->notify();
            debug(F("[BLUETOOTH] Notify Temperature : "));
            debugln(characteristicBuffer);

            // Humidité
            sprintf(characteristicBuffer, "%.2f", _dataEnvironment->humidite);
            _bleCharacteristicHumidity->setValue(characteristicBuffer);
            _bleCharacteristicHumidity->notify();
            debug(F("[BLUETOOTH] Notify Humidity : "));
            debugln(characteristicBuffer);

            // Pression atmosphérique
            sprintf(characteristicBuffer, "%.2f", _dataEnvironment->pression);
            _bleCharacteristicPressure->setValue(characteristicBuffer);
            _bleCharacteristicPressure->notify();
            debug(F("[BLUETOOTH] Notify Pressure : "));
            debugln(characteristicBuffer);

            // Point de rosée
            sprintf(characteristicBuffer, "%.2f", _dataEnvironment->dewPoint);
            _bleCharacteristicDewPoint->setValue(characteristicBuffer);
            _bleCharacteristicDewPoint->notify();
            debug(F("[BLUETOOTH] Notify DewPoint : "));
            debugln(characteristicBuffer);

            // Couverture du ciel
            sprintf(characteristicBuffer, "%.d", (int)_dataSkyState->cloudCover);
            _bleCharacteristicCloudCover->setValue(characteristicBuffer);
            _bleCharacteristicCloudCover->notify();
            debug(F("[BLUETOOTH] Notify CloudCover : "));
            debugln(characteristicBuffer);

            // Luminosité : SkyBrightness
            sprintf(characteristicBuffer, "%.5f", _dataLuminosity->luxAverage);
            _bleCharacteristicSkyBrightness->setValue(characteristicBuffer);
            _bleCharacteristicSkyBrightness->notify();
            debug(F("[BLUETOOTH] Notify SkyBrightness : "));
            debugln(characteristicBuffer);

            // Qualité du ciel : SkyQuality
            sprintf(characteristicBuffer, "%.5f", _dataLuminosity->sqm);
            _bleCharacteristicSkyQuality->setValue(characteristicBuffer);
            _bleCharacteristicSkyQuality->notify();
            debug(F("[BLUETOOTH] Notify SkyQuality : "));
            debugln(characteristicBuffer);

            // Température du ciel : SkyTemperature
            sprintf(characteristicBuffer, "%.2f", _dataSkyState->tempObject);
            _bleCharacteristicSkyTemperature->setValue(characteristicBuffer);
            _bleCharacteristicSkyTemperature->notify();
            debug(F("[BLUETOOTH] Notify SkyTemperature : "));
            debugln(characteristicBuffer);
        }

        // Actualisation flag et compteurs
        *_bluetoothConnected = bleDeviceConnected;
        _chronoReadBluetoothLE = millis();
    }
}
