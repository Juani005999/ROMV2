#pragma once

// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_MQTT : Publication des données capteurs vers un broker Mosquitto via WiFi
/// </summary>
class ROMV2_MQTT
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_MQTT();

    /// <summary>
    /// Initialisation — charge la config broker depuis les Preferences
    /// </summary>
    /// <param name="appType">Type d'application (ROMV2 / SQMLite)</param>
    /// <param name="dataWifi">Pointeur sur les données WiFi (état connexion)</param>
    /// <param name="dataMqtt">Pointeur sur les données MQTT</param>
    /// <param name="dataEnvironment">Pointeur sur les données BME280</param>
    /// <param name="dataLuminosity">Pointeur sur les données TSL2591</param>
    /// <param name="dataSkyState">Pointeur sur les données MLX90614</param>
    /// <param name="dataAcceleration">Pointeur sur les données ADXL345</param>
    /// <param name="dataGPS">Pointeur sur les données GPS</param>
    void Init(APP_TYPE appType,
        DataWifi* dataWifi,
        DataMqtt* dataMqtt,
        DataSensorEnvironment* dataEnvironment,
        DataSensorLuminosity* dataLuminosity,
        DataSensorSkyState* dataSkyState,
        DataSensorAcceleration* dataAcceleration,
        DataSensorGPS* dataGPS);

    /// <summary>
    /// Traitements cadencés : reconnexion si nécessaire + publication périodique
    /// </summary>
    void Loop();

    /// <summary>
    /// Sauvegarde la config broker en Preferences
    /// </summary>
    void SaveBrokerConfig(const String& brokerIp, int brokerPort);

    /// <summary>
    /// Reset la config broker en Preferences
    /// </summary>
    void ResetBrokerConfig();

private:
    // Fonctions internes
    void LoadBrokerConfig();
    bool Connect();
    void PublishAll();
    void PublishFloat(const char* topic, float value, int decimals = 2);
    void PublishDouble(const char* topic, double value, int decimals = 4);
    void PublishInt(const char* topic, int value);
    void PublishBool(const char* topic, bool value);
    void PublishString(const char* topic, const char* value);

    // Membres internes
    WiFiClient              _wifiClient;
    PubSubClient            _mqttClient;
    APP_TYPE                _appType            = APP_ROMV2;
    DataWifi*               _dataWifi           = nullptr;
    DataMqtt*               _dataMqtt           = nullptr;
    DataSensorEnvironment*  _dataEnvironment    = nullptr;
    DataSensorLuminosity*   _dataLuminosity     = nullptr;
    DataSensorSkyState*     _dataSkyState       = nullptr;
    DataSensorAcceleration* _dataAcceleration   = nullptr;
    DataSensorGPS*          _dataGPS            = nullptr;
    unsigned long           _chronoPublish      = 0;
    unsigned long           _chronoReconnect    = 0;
    unsigned long           _chronoMqttLoop     = 0;

    // Constantes internes
    static const char* TOPIC_ROMV2_ENV_TEMPERATURE;
    static const char* TOPIC_ROMV2_ENV_HUMIDITY;
    static const char* TOPIC_ROMV2_ENV_PRESSURE;
    static const char* TOPIC_ROMV2_ENV_DEWPOINT;
    static const char* TOPIC_ROMV2_LUM_LUX;
    static const char* TOPIC_ROMV2_LUM_SQM;
    static const char* TOPIC_ROMV2_LUM_BORTLE;
    static const char* TOPIC_ROMV2_SKY_TEMP_AMBIENT;
    static const char* TOPIC_ROMV2_SKY_TEMP_OBJECT;
    static const char* TOPIC_ROMV2_SKY_CLOUD_COVER;
    static const char* TOPIC_ROMV2_SKY_STATE;
    static const char* TOPIC_ROMV2_ACCEL_X;
    static const char* TOPIC_ROMV2_ACCEL_Y;
    static const char* TOPIC_ROMV2_ACCEL_Z;
    static const char* TOPIC_ROMV2_GPS_LATITUDE;
    static const char* TOPIC_ROMV2_GPS_LONGITUDE;
    static const char* TOPIC_ROMV2_GPS_ALTITUDE;
    static const char* TOPIC_ROMV2_GPS_SATELLITES;
    static const char* TOPIC_ROMV2_GPS_FIX;
    static const char* TOPIC_SQMLITE_ENV_TEMPERATURE;
    static const char* TOPIC_SQMLITE_ENV_HUMIDITY;
    static const char* TOPIC_SQMLITE_ENV_PRESSURE;
    static const char* TOPIC_SQMLITE_ENV_DEWPOINT;
    static const char* TOPIC_SQMLITE_LUM_LUX;
    static const char* TOPIC_SQMLITE_LUM_SQM;
    static const char* TOPIC_SQMLITE_LUM_BORTLE;
};
