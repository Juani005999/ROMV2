#pragma once

// Include des Objets et Librairies
#include <ROMV2_APP_CONFIG.h>
#include <ROMV2_BME280.h>
#include <ROMV2_TSL2591.h>
#include <ROMV2_MLX90614.h>
#include <ROMV2_ADXL345.h>
#include <ROMV2_GPS.h>
#include <ROMV2_BLE.h>
#include <ROMV2_TFT.h>


/// <summary>
/// Objet ROMV2_APP : Objet applicatif permettant le pilotage complet de l'application ROMV2_APP
/// </summary>
class ROMV2_APP : JUANITO_APP
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_APP();

    /// <summary>
    /// Initialisation de l'application
    /// </summary>
    void Init();

    /// <summary>
    /// Traitements cadencés
    /// </summary>
    void Loop();

  private:
    // Fonctions
    void ReadJoystickState();
    void UpdateCurrentDisplay();

    // Instanciation des objets internes
    ROMV2_BME280 _bme280 = ROMV2_BME280();
    ROMV2_TSL2591 _tsl2591 = ROMV2_TSL2591();
    ROMV2_MLX90614 _mlx90614 = ROMV2_MLX90614();
    ROMV2_ADXL345 _adxl345 = ROMV2_ADXL345();
    ROMV2_GPS _gps = ROMV2_GPS();
    ROMV2_BLE _bluetoothLE = ROMV2_BLE();
    ROMV2_TFT _tft = ROMV2_TFT();

    // Membres internes
    DISPLAY_SCREEN_TYPE _currentDisplayScreenType = DISPLAY_HOME;           // Ecran d'affichage courant
    ACTION_TYPE _currentAction = ACTION_NONE;                               // Action en cours
    DISPLAY_HOME_TYPE _displayHomeType = DISPLAY_HOME_LUX;                  // Mode d'affichage de l'écran Home
    DataSensorEnvironment _dataEnvironment;                                 // Données relatives au capteur d'envitonnement BME280
    DataSensorLuminosity _dataLuminosity;                                   // Données relatives au cateur de luminosité TSL2591
    DataSensorSkyState _dataSkyState;                                       // Données relatives au capteur d'état du ciel MLX90614
    DataSensorAcceleration _dataAcceleration;                               // Données relatives au capteur d'accélération ADXL345
    DataSensorGPS _dataGPS;                                                 // Données relatives au capteur GPS NEO 8M
    bool  _bluetoothConnected = false;                                      // Flag de connexion Bluetooth
    bool _joystickPressed;                                                  // Flag de clic sur le switch du Joystick
    bool _joystickMoved;                                                    // Flag de mouvement du Joystick

    // Chronos
    long _chronoReadJoystick;                                               // Chrono pour lecture de l'état du Joystick
    long _chronoJoystickButtonPressed;                                      // Chrono de début de clic sur le switch du Joystick
    long _chronoDisplayTimeout;                                             // Chrono de gestion de la mise en veille de l'affichage
};