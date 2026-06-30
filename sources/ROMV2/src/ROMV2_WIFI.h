// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_WIFI : Gestion WiFi (STA + AP) avec portail web de configuration
/// </summary>
class ROMV2_WIFI
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_WIFI();

    /// <summary>
    /// Initialisation — charge les credentials depuis les Preferences et démarre en mode STA
    /// </summary>
    /// <param name="appType"></param>
    /// <param name="dataWifi"></param>
    /// <param name="dataMqtt"></param>
    void Init(APP_TYPE appType, DataWifi* dataWifi, DataMqtt* dataMqtt);

    /// <summary>
    /// Démarre la tâche FreeRTOS en charge du Wifi sur le coeur 2
    /// </summary>
    void StartTask();

    /// <summary>
    /// Traitements cadencés : gère le serveur web en mode AP, surveille la connexion STA
    /// </summary>
    void Loop();

    /// <summary>
    /// Bascule en mode AP : arrête le STA, démarre le point d'accès + serveur web
    /// </summary>
    void StartAP();

    /// <summary>
    /// Annule le mode AP et retourne en mode STA avec les credentials existants
    /// </summary>
    void CancelAP();

    /// <summary>
    /// Reset les credentials en Preferences
    /// </summary>
    void ResetCredentials();

    /// <summary>
    /// Reset de la variable locale _configReceived
    /// </summary>
    void FlushConfigReceived();

private:
    // Fonctions internes
    void StartSTA();
    void StopAP();
    void HandleWebClient();
    void ServeConfigPage(WiFiClient& client);
    void ServeSuccess(WiFiClient& client);
    String BuildConfigPage();
    String BuildSuccessPage();
    void SaveCredentials(const String& ssid, const String& password, const String& brokerIp, int brokerPort);
    void LoadCredentials();
    String UrlDecode(const String& encoded);
    static void TaskWrapper(void* pvParameters);
    void TaskLoop();
    void DoAction();
    void DoStartSTA();
    void DoStartAP();
    void DoCancelAP();
    void DoResetCredentials();

    // Membres internes
    SemaphoreHandle_t   _mutex;
    APP_TYPE            _appType                = APP_ROMV2;
    String              _apSSID                 = WIFI_AP_SSID_ROMV2;
    ACTION_WIFI         _nextAction             = ACTION_WIFI_NONE;
    DataWifi*           _dataWifi               = nullptr;
    DataMqtt*           _dataMqtt               = nullptr;
    WiFiServer*         _webServer              = nullptr;
    char                _ip[20]                 = "";
    char                _ipLocal[20]            = "";
    char                _ssid[64]               = "";
    char                _ssidLocal[64]          = "";
    char                _password[64]           = "";
    char                _passwordLocal[64]      = "";
    char                _brokerIp[40]           = "";
    char                _brokerIpLocal[40]      = "";
    int                 _brokerPort             = MQTT_DEFAULT_PORT;
    int                 _brokerPortLocal        = MQTT_DEFAULT_PORT;
    char                _apIp[20]               = "";
    char                _apIpLocal[20]          = "";
    bool                _apActive               = false;
    bool                _apActiveLocal          = false;
    bool                _connected              = false;
    bool                _connectedLocal         = false;
    bool                _configReceived         = false;
    bool                _configReceivedLocal    = false;
    WIFI_STATE          _wifiState              = WIFI_STATE_STA;
    WIFI_STATE          _wifiStateLocal         = WIFI_STATE_STA;

    unsigned long   _chronoWifiLoop         = 0;
};