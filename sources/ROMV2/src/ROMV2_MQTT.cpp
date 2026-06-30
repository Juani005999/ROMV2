#include <ROMV2_MQTT.h>

// Définition des constantes statiques de la classe
const char* ROMV2_MQTT::TOPIC_ROMV2_ENV_TEMPERATURE     = "romv2/environment/temperature";
const char* ROMV2_MQTT::TOPIC_ROMV2_ENV_HUMIDITY        = "romv2/environment/humidity";
const char* ROMV2_MQTT::TOPIC_ROMV2_ENV_PRESSURE        = "romv2/environment/pressure";
const char* ROMV2_MQTT::TOPIC_ROMV2_ENV_DEWPOINT        = "romv2/environment/dewpoint";
const char* ROMV2_MQTT::TOPIC_ROMV2_LUM_LUX             = "romv2/luminosity/lux";
const char* ROMV2_MQTT::TOPIC_ROMV2_LUM_SQM             = "romv2/luminosity/sqm";
const char* ROMV2_MQTT::TOPIC_ROMV2_LUM_BORTLE          = "romv2/luminosity/bortle";
const char* ROMV2_MQTT::TOPIC_ROMV2_SKY_TEMP_AMBIENT    = "romv2/sky/temp_ambient";
const char* ROMV2_MQTT::TOPIC_ROMV2_SKY_TEMP_OBJECT     = "romv2/sky/temp_object";
const char* ROMV2_MQTT::TOPIC_ROMV2_SKY_CLOUD_COVER     = "romv2/sky/cloud_cover";
const char* ROMV2_MQTT::TOPIC_ROMV2_SKY_STATE           = "romv2/sky/state";
const char* ROMV2_MQTT::TOPIC_ROMV2_ACCEL_X             = "romv2/acceleration/x";
const char* ROMV2_MQTT::TOPIC_ROMV2_ACCEL_Y             = "romv2/acceleration/y";
const char* ROMV2_MQTT::TOPIC_ROMV2_ACCEL_Z             = "romv2/acceleration/z";
const char* ROMV2_MQTT::TOPIC_ROMV2_GPS_LATITUDE        = "romv2/gps/latitude";
const char* ROMV2_MQTT::TOPIC_ROMV2_GPS_LONGITUDE       = "romv2/gps/longitude";
const char* ROMV2_MQTT::TOPIC_ROMV2_GPS_ALTITUDE        = "romv2/gps/altitude";
const char* ROMV2_MQTT::TOPIC_ROMV2_GPS_SATELLITES      = "romv2/gps/satellites";
const char* ROMV2_MQTT::TOPIC_ROMV2_GPS_FIX             = "romv2/gps/fix";
const char* ROMV2_MQTT::TOPIC_SQMLITE_ENV_TEMPERATURE   = "sqmlite/environment/temperature";
const char* ROMV2_MQTT::TOPIC_SQMLITE_ENV_HUMIDITY      = "sqmlite/environment/humidity";
const char* ROMV2_MQTT::TOPIC_SQMLITE_ENV_PRESSURE      = "sqmlite/environment/pressure";
const char* ROMV2_MQTT::TOPIC_SQMLITE_ENV_DEWPOINT      = "sqmlite/environment/dewpoint";
const char* ROMV2_MQTT::TOPIC_SQMLITE_LUM_LUX           = "sqmlite/luminosity/lux";
const char* ROMV2_MQTT::TOPIC_SQMLITE_LUM_SQM           = "sqmlite/luminosity/sqm";
const char* ROMV2_MQTT::TOPIC_SQMLITE_LUM_BORTLE        = "sqmlite/luminosity/bortle";

/// <summary>
/// Constructeur
/// </summary>
ROMV2_MQTT::ROMV2_MQTT() : _mqttClient(_wifiClient)
{
}

/// <summary>
/// Initialisation
/// </summary>
void ROMV2_MQTT::Init(APP_TYPE appType,
    DataWifi* dataWifi,
    DataMqtt* dataMqtt,
    DataSensorEnvironment* dataEnvironment,
    DataSensorLuminosity* dataLuminosity,
    DataSensorSkyState* dataSkyState,
    DataSensorAcceleration* dataAcceleration,
    DataSensorGPS* dataGPS)
{
    // Valorisation des membres internes
    _appType = appType;
    _dataWifi = dataWifi;
    _dataMqtt = dataMqtt;
    _dataEnvironment = dataEnvironment;
    _dataLuminosity = dataLuminosity;
    _dataSkyState = dataSkyState;
    _dataAcceleration = dataAcceleration;
    _dataGPS = dataGPS;

    // Chargement de la config broker depuis les Preferences
    LoadBrokerConfig();

    // Configuration du _mqttClient
    _mqttClient.setKeepAlive(60);       // 60 s au lieu de 15 s par défaut
    _mqttClient.setSocketTimeout(30);   // 30 s au lieu de 15 s par défaut

    debugln(F(""));
    debugln(F("[MQTT] Initialisation"));
    debug(F("[MQTT] Broker : "));
    debug(_dataMqtt->brokerIp);
    debug(F(":"));
    debugln(_dataMqtt->brokerPort);
}

/// <summary>
/// Traitements cadencés
/// </summary>
void ROMV2_MQTT::Loop()
{
    // Traitement cadencé du MQTT (reconnexion + publication)
    if (millis() - _chronoMqttLoop >= MQTT_LOOP_INTERVAL)
    {
        // Pas de tentative si WiFi non connecté ou broker non configuré
        if (!_dataWifi->connected || strlen(_dataMqtt->brokerIp) == 0)
        {
            // Cadencement du chrono même si non connecté au Wifi ou au Broker
            _chronoMqttLoop = millis();

            // Retour sans traitement si non connecté
            return;
        }

        // Maintien de la connexion MQTT
        if (!_mqttClient.connected())
        {
            _dataMqtt->connected = false;

            // Tentative de reconnexion sur intervalle
            if (millis() - _chronoReconnect >= MQTT_RECONNECT_INTERVAL)
            {
                Connect();
                _chronoReconnect = millis();
            }
        }
        else
        {
            _dataMqtt->connected = true;
            _mqttClient.loop();

            // Publication périodique
            if (millis() - _chronoPublish >= MQTT_PUBLISH_INTERVAL)
            {
                PublishAll();
                _chronoPublish = millis();
            }
        }

        // Actualisation Chrono
        _chronoMqttLoop = millis();
    }
}

/// <summary>
/// Connexion au broker MQTT
/// </summary>
bool ROMV2_MQTT::Connect()
{
    debugln(F(""));
    debug(F("[MQTT] Connexion au broker : "));
    debug(_dataMqtt->brokerIp);
    debug(F(":"));
    debugln(_dataMqtt->brokerPort);

    if (strlen(_dataMqtt->brokerIp) > 0)
    {
        _mqttClient.setServer(_dataMqtt->brokerIp, _dataMqtt->brokerPort);

        if (_mqttClient.connect(_appType == APP_SQMLITE ? MQTT_CLIENT_ID_SQMLITE : MQTT_CLIENT_ID_ROMV2))
        {
            _dataMqtt->connected = true;
            debugln(F("[MQTT] Connecte"));
            return true;
        }
        else
        {
            _dataMqtt->connected = false;
            debug(F("[MQTT] Echec connexion, rc="));
            debugln(_mqttClient.state());
            return false;
        }
    }
    else
    {
        _dataMqtt->connected = false;
        debugln(F("[MQTT] Echec connexion, aucun Broker defini"));
        return false;
    }
}

/// <summary>
/// Publication de toutes les mesures
/// </summary>
void ROMV2_MQTT::PublishAll()
{
    debugln(F(""));
    debugln(F("[MQTT] Publication des donnees"));

    if (_dataMqtt->connected)
    {
        // --- Environnement (BME280) ---
        if (!isnan(_dataEnvironment->temperature))
            PublishFloat(_appType == APP_SQMLITE ? TOPIC_SQMLITE_ENV_TEMPERATURE : TOPIC_ROMV2_ENV_TEMPERATURE, _dataEnvironment->temperature);

        if (!isnan(_dataEnvironment->humidite))
            PublishFloat(_appType == APP_SQMLITE ? TOPIC_SQMLITE_ENV_HUMIDITY : TOPIC_ROMV2_ENV_HUMIDITY, _dataEnvironment->humidite);

        if (!isnan(_dataEnvironment->pression))
            PublishFloat(_appType == APP_SQMLITE ? TOPIC_SQMLITE_ENV_PRESSURE : TOPIC_ROMV2_ENV_PRESSURE, _dataEnvironment->pression, 1);

        if (!isnan(_dataEnvironment->dewPoint))
            PublishFloat(_appType == APP_SQMLITE ? TOPIC_SQMLITE_ENV_DEWPOINT : TOPIC_ROMV2_ENV_DEWPOINT, _dataEnvironment->dewPoint);

        // --- Luminosité (TSL2591) ---
        if (!isnan(_dataLuminosity->lux))
            PublishFloat(_appType == APP_SQMLITE ? TOPIC_SQMLITE_LUM_LUX : TOPIC_ROMV2_LUM_LUX, _dataLuminosity->lux, 4);

        if (!isnan(_dataLuminosity->sqm))
            PublishDouble(_appType == APP_SQMLITE ? TOPIC_SQMLITE_LUM_SQM : TOPIC_ROMV2_LUM_SQM, _dataLuminosity->sqm, 4);

        if (!isnan(_dataLuminosity->bortle))
            PublishFloat(_appType == APP_SQMLITE ? TOPIC_SQMLITE_LUM_BORTLE : TOPIC_ROMV2_LUM_BORTLE, _dataLuminosity->bortle, 1);

        if (_appType == APP_ROMV2)
        {
            // --- État du ciel (MLX90614) ---
            if (!isnan(_dataSkyState->tempAmbient))
                PublishFloat(TOPIC_ROMV2_SKY_TEMP_AMBIENT, _dataSkyState->tempAmbient);

            if (!isnan(_dataSkyState->tempObject))
                PublishFloat(TOPIC_ROMV2_SKY_TEMP_OBJECT, _dataSkyState->tempObject);

            if (!isnan(_dataSkyState->cloudCover))
                PublishFloat(TOPIC_ROMV2_SKY_CLOUD_COVER, _dataSkyState->cloudCover, 1);

            // État du ciel en texte
            const char* skyStateStr = "UNKNOWN";
            switch (_dataSkyState->skyState)
            {
            case SKY_STATE_CLEAR:           skyStateStr = "CLEAR";          break;
            case SKY_STATE_PARTLY_CLOUDY:   skyStateStr = "PARTLY_CLOUDY";  break;
            case SKY_STATE_CLOUDY:          skyStateStr = "CLOUDY";         break;
            default:                                                         break;
            }
            PublishString(TOPIC_ROMV2_SKY_STATE, skyStateStr);

            // --- Accélération (ADXL345) ---
            if (!isnan(_dataAcceleration->x))
                PublishFloat(TOPIC_ROMV2_ACCEL_X, _dataAcceleration->x, 3);

            if (!isnan(_dataAcceleration->y))
                PublishFloat(TOPIC_ROMV2_ACCEL_Y, _dataAcceleration->y, 3);

            if (!isnan(_dataAcceleration->z))
                PublishFloat(TOPIC_ROMV2_ACCEL_Z, _dataAcceleration->z, 3);

            // --- GPS ---
            if (strlen(_dataGPS->gpsLatitude) > 0)
                PublishString(TOPIC_ROMV2_GPS_LATITUDE, _dataGPS->gpsLatitude);

            if (strlen(_dataGPS->gpsLongitude) > 0)
                PublishString(TOPIC_ROMV2_GPS_LONGITUDE, _dataGPS->gpsLongitude);

            if (strlen(_dataGPS->gpsAltitude) > 0)
                PublishString(TOPIC_ROMV2_GPS_ALTITUDE, _dataGPS->gpsAltitude);

            if (strlen(_dataGPS->gpsSatellites) > 0)
                PublishString(TOPIC_ROMV2_GPS_SATELLITES, _dataGPS->gpsSatellites);

            PublishBool(TOPIC_ROMV2_GPS_FIX, _dataGPS->gpsFix);
        }
    }
    else
    {
        debugln(F("[MQTT] Broker non connecte"));
    }
}

/// <summary>
/// Publie un float formaté
/// </summary>
void ROMV2_MQTT::PublishFloat(const char* topic, float value, int decimals)
{
    char buf[20];
    dtostrf(value, 1, decimals, buf);
    _mqttClient.publish(topic, buf);
}

/// <summary>
/// Publie un double formaté
/// </summary>
void ROMV2_MQTT::PublishDouble(const char* topic, double value, int decimals)
{
    char buf[20];
    dtostrf((float)value, 1, decimals, buf);
    _mqttClient.publish(topic, buf);
}

/// <summary>
/// Publie un entier
/// </summary>
void ROMV2_MQTT::PublishInt(const char* topic, int value)
{
    char buf[12];
    itoa(value, buf, 10);
    _mqttClient.publish(topic, buf);
}

/// <summary>
/// Publie un booléen (0 / 1)
/// </summary>
void ROMV2_MQTT::PublishBool(const char* topic, bool value)
{
    _mqttClient.publish(topic, value ? "1" : "0");
}

/// <summary>
/// Publie une chaîne de caractères
/// </summary>
void ROMV2_MQTT::PublishString(const char* topic, const char* value)
{
    _mqttClient.publish(topic, value);
}

/// <summary>
/// Sauvegarde la config broker en Preferences
/// </summary>
void ROMV2_MQTT::SaveBrokerConfig(const String& brokerIp, int brokerPort)
{
    Preferences prefs;
    prefs.begin(PREF_MQTT_NAMESPACE, false);
    prefs.putString(PREF_MQTT_KEY_BROKER_IP, brokerIp);
    prefs.putInt(PREF_MQTT_KEY_BROKER_PORT, brokerPort);
    prefs.end();

    strncpy(_dataMqtt->brokerIp, brokerIp.c_str(), sizeof(_dataMqtt->brokerIp) - 1);
    _dataMqtt->brokerPort = brokerPort;

    // Mise à jour du serveur MQTT avec la nouvelle config
    _mqttClient.setServer(_dataMqtt->brokerIp, _dataMqtt->brokerPort);
    _mqttClient.disconnect();

    debug(F("[MQTT] Config broker sauvegardee : "));
    debug(brokerIp);
    debug(F(":"));
    debugln(brokerPort);
}

/// <summary>
/// Charge la config broker depuis les Preferences
/// </summary>
void ROMV2_MQTT::LoadBrokerConfig()
{
    Preferences prefs;
    prefs.begin(PREF_MQTT_NAMESPACE, true);
    String brokerIp = prefs.getString(PREF_MQTT_KEY_BROKER_IP, "");
    int brokerPort = prefs.getInt(PREF_MQTT_KEY_BROKER_PORT, MQTT_DEFAULT_PORT);
    prefs.end();

    strncpy(_dataMqtt->brokerIp, brokerIp.c_str(), sizeof(_dataMqtt->brokerIp) - 1);
    _dataMqtt->brokerPort = brokerPort;

    debug(F("[MQTT] Chargement config broker : "));
    debug(brokerIp);
    debug(F(":"));
    debugln(brokerPort);
}

/// <summary>
/// Reset la config broker en Preferences
/// </summary>
void ROMV2_MQTT::ResetBrokerConfig()
{
    // Reset des préférences
    Preferences prefs;
    prefs.begin(PREF_MQTT_NAMESPACE, false);
    prefs.putString(PREF_MQTT_KEY_BROKER_IP, "");
    prefs.putInt(PREF_MQTT_KEY_BROKER_PORT, MQTT_DEFAULT_PORT);
    prefs.end();

    strcpy(_dataMqtt->brokerIp, "");
    _dataMqtt->brokerPort = MQTT_DEFAULT_PORT;
    _mqttClient.disconnect();

    // Trace
    debugln(F(""));
    debugln(F("[WIFI] Reset Config broker effectue."));
}
