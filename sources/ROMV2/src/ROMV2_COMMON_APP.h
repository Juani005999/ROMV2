#pragma once

// Include des Objets et Librairies
#include <ROMV2_APP_CONFIG.h>
#include <ROMV2_TFT_COMMON.h>
#include <ROMV2_BME280.h>
#include <ROMV2_TSL2591.h>
#include <ROMV2_BLE.h>
#include <ROMV2_WIFI.h>
#include <ROMV2_MQTT.h>

/// <summary>
/// Objet ROMV2_APP : Classe de base des applications ROMV2 et SQMLite
/// Classe abstraite : instanciée via les enfants ROMV2_APP et SQMLITE_APP
/// </summary>
class ROMV2_COMMON_APP : JUANITO_APP
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_COMMON_APP();

    /// <summary>
    /// Destructeur virtuel
    /// </summary>
    virtual ~ROMV2_COMMON_APP() { delete _tft; }

    /// <summary>
    /// Initialisation de l'application
    /// </summary>
    void Init();

    /// <summary>
    /// Traitements cadencés
    /// </summary>
    void Loop();

  protected:
    /// <summary>
    /// Initialisation des capteurs propres au modèle.
    /// </summary>
    virtual void InitModelSensors() {}

    /// <summary>
    /// Lectures cadencées des capteurs propres au modèle.
    /// </summary>
    virtual void LoopModelSensors() {}

    /// <summary>
    /// Lecture de l'état du Joystick
    /// </summary>
    virtual void ReadJoystickState() {}

    /// <summary>
    /// Renvoie l'ordre des écrans navigables au joystick, propre au modèle.
    /// count reçoit le nombre d'écrans du tableau retourné.
    /// Pure virtuelle : implémentée par chaque enfant (liste complète pour ROMV2,
    /// liste réduite sans IRTEMP/ACCELERATION/GPS pour SQMLite).
    /// </summary>
    virtual const DISPLAY_SCREEN_TYPE* GetScreenOrder(uint8_t& count) const = 0;

    // Instanciation des objets internes
    ROMV2_TFT_COMMON*   _tft            = nullptr;
    ROMV2_BME280        _bme280         = ROMV2_BME280();
    ROMV2_TSL2591       _tsl2591        = ROMV2_TSL2591();
    ROMV2_BLE           _bluetoothLE    = ROMV2_BLE();
    ROMV2_WIFI          _wifi           = ROMV2_WIFI();
    ROMV2_MQTT          _mqtt           = ROMV2_MQTT();

    // Membres internes
    APP_TYPE                _appType                    = APP_ROMV2;        // Application en cours : ROMV2 / SQMLite
    DISPLAY_SCREEN_TYPE     _currentDisplayScreenType   = DISPLAY_HOME;     // Ecran d'affichage courant
    ACTION_TYPE             _currentAction              = ACTION_NONE;      // Action en cours
    DISPLAY_HOME_TYPE       _displayHomeType            = DISPLAY_HOME_SQM; // Mode d'affichage de l'écran Home
    NETWORK_MODE            _networkMode                = NETWORK_NONE;     // Mode réseau
    DataSensorEnvironment   _dataEnvironment;                               // Données relatives au capteur d'envitonnement BME280
    DataSensorLuminosity    _dataLuminosity;                                // Données relatives au cateur de luminosité TSL2591
    DataSensorSkyState      _dataSkyState;                                  // Données relatives au capteur d'état du ciel MLX90614
    DataSensorAcceleration  _dataAcceleration;                              // Données relatives au capteur d'accélération ADXL345
    DataSensorGPS           _dataGPS;                                       // Données relatives au capteur GPS NEO 8M
    DataWifi                _dataWifi;                                      // Données relatives au WiFi
    DataMqtt                _dataMqtt;                                      // Données relatives au MQTT
    bool                    _bluetoothConnected         = false;            // Flag de connexion Bluetooth
    bool                    _joystickPressed;                               // Flag de clic sur le switch du Joystick
    bool                    _joystickMoved;                                 // Flag de mouvement du Joystick

    // Chronos
    unsigned long _chronoReadJoystick;                                      // Chrono pour lecture de l'état du Joystick
    unsigned long _chronoJoystickButtonPressed;                             // Chrono de début de clic sur le switch du Joystick
    unsigned long _chronoJoystickButtonMoved;                               // Chrono de début de mouvement du Joystick
    unsigned long _chronoDisplayTimeout;                                    // Chrono de gestion de la mise en veille de l'affichage

  private:
    // Fonctions
    void UpdateCurrentDisplay();
    void LoadCredentials();


    // Navigation des écrans pilotée par l'ordre propre au modèle (GetScreenOrder)
    void GoToAdjacentScreen(int direction);                                 // +1 = droite, -1 = gauche (circulaire)
    bool IsScreenVisible(DISPLAY_SCREEN_TYPE screen) const;                 // Gestion des cas particuliers : exple, l'écran WiFi n'apparaît qu'en mode WiFi
};