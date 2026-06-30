// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_HEADER : Objet applicatif permettant l'affichage du header
/// </summary>
class ROMV2_DISPLAY_HEADER
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_HEADER();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="appType"></param>
    /// <param name="tft"></param>
    /// <param name="dataEnvironment"></param>
    /// <param name="dataGPS"></param>
    /// <param name="dataWifi"></param>
    /// <param name="dataMqtt"></param>
    /// <param name="bluetoothConnected"></param>
    /// <param name="networkMode"></param>
    void Init(APP_TYPE appType,
        TFT_eSPI * tft,
        DataSensorEnvironment* dataEnvironment,
        DataSensorGPS* dataGPS,
        DataWifi* dataWifi,
        DataMqtt* dataMqtt,
        bool* bluetoothConnected,
        NETWORK_MODE* networkMode);

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay();

    /// <summary>
    /// Met à jour l'affichage en cours
    /// </summary>
    /// <param name="currentDisplayScreenType"></param>
    void SetCurrentDisplay(DISPLAY_SCREEN_TYPE currentDisplayScreenType);

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw();

    /// <summary>
    /// Affiche l'icone d'état de lecture de la luminosité
    /// </summary>
    /// <param name="on"></param>
    void SetLuminosityIcon(bool on = false);

  private:
    // Fonctions
    void ClearDisplay();
    void DisplayTitle();
    void DisplayDate();
    void DisplayMainIcon();
    void DisplayGPSIcon();
    void DisplayNetworkIcon();
    void DisplayBluetoothIcon();
    void DisplayWifiIcon();
    void DisplayOfflineIcon();

    // Instanciation des objets internes
    TFT_eSPI * _tft;

    // Membres internes
    APP_TYPE                _appType                    = APP_ROMV2;
    DISPLAY_SCREEN_TYPE     _currentDisplayScreenType   = DISPLAY_HOME;
    bool                    _forceRedraw                = true;
    DataSensorEnvironment*  _dataEnvironment            = nullptr;
    DataSensorGPS*          _dataGPS                    = nullptr;
    DataWifi*               _dataWifi                   = nullptr;
    DataMqtt*               _dataMqtt                   = nullptr;
    bool*                   _bluetoothConnected         = nullptr;
    NETWORK_MODE*           _networkMode                = nullptr;
    bool                    _iconGPSLastState           = false;
    bool                    _iconBluetoothLastState     = false;
    bool                    firstLuminosityIcon         = true;
    bool                    lastLuminosityIconState     = false;
    WIFI_STATE              lastWifiState               = WIFI_STATE_STA;
    bool                    lastWifiConnectedState      = false;
    bool                    lastMqttConnectedState      = false;
};