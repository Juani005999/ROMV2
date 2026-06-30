#pragma once

// Include des librairies
#include <ROMV2_DISPLAY_HEADER.h>
#include <ROMV2_DISPLAY_HOME.h>
#include <ROMV2_DISPLAY_LUX.h>
#include <ROMV2_DISPLAY_ENVIRONMENT.h>
#include <ROMV2_DISPLAY_NETWORK.h>
#include <ROMV2_DISPLAY_WIFI.h>
#include <ROMV2_DISPLAY_TSL2591_CALIBRATION.h>

/// <summary>
/// Objet ROMV2_TFT_COMMON : Classe de base des objest applicatif permettant le pilotage de l'écran TFT
/// Instanciée via les enfants ROMV2_APP et SQMLITE_APP
/// </summary>
class ROMV2_TFT_COMMON
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_TFT_COMMON();

    /// <summary>
    /// Destructeur virtuel
    /// </summary>
    virtual ~ROMV2_TFT_COMMON() = default;

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="appType"></param>
    /// <param name="dataEnvironment"></param>
    /// <param name="dataLuminosity"></param>
    /// <param name="dataSkyState"></param>
    /// <param name="dataAcceleration"></param>
    /// <param name="dataGPS"></param>
    /// <param name="dataWifi"></param>
    /// <param name="dataMqtt"></param>
    /// <param name="bluetoothConnected"></param>
    /// <param name="networkMode"></param>
    void Init(APP_TYPE appType,
                DataSensorEnvironment* dataEnvironment,
                DataSensorLuminosity* dataLuminosity,
                DataSensorSkyState* dataSkyState,
                DataSensorAcceleration* dataAcceleration,
                DataSensorGPS* dataGPS,
                DataWifi* dataWifi,
                DataMqtt* dataMqtt,
                bool* bluetoothConnected,
                NETWORK_MODE* networkMode);

    /// <summary>
    /// Met à jour l'affichage en cours
    /// </summary>
    /// <param name="currentDisplayScreenType"></param>
    /// <param name="displayHomeType"></param>
    void SetCurrentDisplay(DISPLAY_SCREEN_TYPE currentDisplayScreenType, DISPLAY_HOME_TYPE displayHomeType);

    /// <summary>
    /// Affiche l'icone d'état de lecture de la luminosité
    /// </summary>
    void SetLuminosityIcon(bool on = false);

    /// <summary>
    /// Met à jour la valeur courante de la calibration du TSL2591
    /// </summary>
    void SetNewCalibrationValue(int newCalibrationValue);

    /// <summary>
    /// Modification du réseau en fonction de la direction (Up / Down)
    /// </summary>
    void SetNewNetworkMode(bool directionUp);

    /// <summary>
    /// Augmente la calibration du TSL2591 de 0.01 si < TSL2591_CALIBRATION_MAX_VALUE
    /// </summary>
    void IncreaseCalibrationValue();

    /// <summary>
    /// Diminue la calibration du TSL2591 de 0.01 si > TSL2591_CALIBRATION_MIN_VALUE
    /// </summary>
    void DecreaseCalibrationValue();

    /// <summary>
    /// Renvoi la nouvelle valeur de la calibration du TSL2591
    /// </summary>
    int GetNewCalibrationValue();

    /// <summary>
    /// Sauvegarde les credentials réseau en Preferences
    /// </summary>
    void SaveNetworkCredentials();

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    virtual void ForceRedraw() {}

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    virtual void UpdateDisplay() {}

  protected:
    /// <summary>
    /// Affichage de l'écran d'initialisation de l'application
    /// </summary>
    virtual void DisplaySplashApp() {}

    /// <summary>
    /// Initialisation des écrans propres au modèle.
    /// </summary>
    virtual void InitModelDisplays() {}

    // Instanciation des objets internes
    TFT_eSPI                            _tft;                                                               // Ecran TFT SD7735
    ROMV2_DISPLAY_HEADER                _displayHeader              = ROMV2_DISPLAY_HEADER();               // Objet pour l'affichage du Header
    ROMV2_DISPLAY_HOME                  _displayHome                = ROMV2_DISPLAY_HOME();                 // Objet pour l'affichage du panneau Home
    ROMV2_DISPLAY_LUX                   _displayLux                 = ROMV2_DISPLAY_LUX();                  // Objet pour l'affichage du panneau Lux
    ROMV2_DISPLAY_ENVIRONMENT           _displayEnvironment         = ROMV2_DISPLAY_ENVIRONMENT();          // Objet pour l'affichage du panneau Environnement
    ROMV2_DISPLAY_NETWORK               _displayNetwork             = ROMV2_DISPLAY_NETWORK();              // Objet pour l'affichage du panneau de choix de réseau
    ROMV2_DISPLAY_WIFI                  _displayWifi                = ROMV2_DISPLAY_WIFI();                 // Objet pour l'affichage du panneau de réglage du Wifi
    ROMV2_DISPLAY_TSL2591_CALIBRATION   _displayTSL2591Calibration  = ROMV2_DISPLAY_TSL2591_CALIBRATION();  // Objet pour l'affichage du panneau Calibration du TSL2591

    // Membres internes
    APP_TYPE                    _appType                    = APP_ROMV2;
    DISPLAY_SCREEN_TYPE         _currentDisplayScreenType   = DISPLAY_HOME;
    DISPLAY_SCREEN_TYPE         _lastDisplayScreenType      = DISPLAY_HOME;
    DISPLAY_HOME_TYPE           _displayHomeType            = DISPLAY_HOME_SQM;
    DataSensorEnvironment*      _dataEnvironment            = nullptr;
    DataSensorLuminosity*       _dataLuminosity             = nullptr;
    DataSensorSkyState*         _dataSkyState               = nullptr;
    DataSensorAcceleration*     _dataAcceleration           = nullptr;
    DataSensorGPS*              _dataGPS                    = nullptr;
    DataWifi*                   _dataWifi                   = nullptr;
    DataMqtt*                   _dataMqtt                   = nullptr;
    bool*                       _bluetoothConnected         = nullptr;
    NETWORK_MODE*               _networkMode                = nullptr;

    // Chronos
    long _chronoDisplay;

  private:
    // Fonctions
};