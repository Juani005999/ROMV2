#pragma once

// Include des librairies
#include <ROMV2_DISPLAY_HEADER.h>
#include <ROMV2_DISPLAY_HOME.h>
#include <ROMV2_DISPLAY_LUX.h>
#include <ROMV2_DISPLAY_ENVIRONMENT.h>
#include <ROMV2_DISPLAY_IRTEMP.h>
#include <ROMV2_DISPLAY_ACCELERATION.h>
#include <ROMV2_DISPLAY_GPS.h>
#include <ROMV2_DISPLAY_TSL2591_CALIBRATION.h>

/// <summary>
/// Objet ROMV2_TFT : Objet applicatif permettant le pilotage de l'écran TFT
/// </summary>
class ROMV2_TFT
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_TFT();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="dataEnvironment"></param>
    /// <param name="dataLuminosity"></param>
    /// <param name="dataSkyState"></param>
    /// <param name="dataAcceleration"></param>
    /// <param name="_dataGPS"></param>
    /// <param name="bluetoothConnected"></param>
    void Init(DataSensorEnvironment* dataEnvironment,
                DataSensorLuminosity* dataLuminosity,
                DataSensorSkyState* dataSkyState,
                DataSensorAcceleration* dataAcceleration,
                DataSensorGPS* dataGPS,
                bool* bluetoothConnected);

    /// <summary>
    /// Affichage de l'écran d'initialisation de l'application
    /// </summary>
    void DisplaySplashApp();

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay();

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw();

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

  private:
    // Fonctions

    // Instanciation des objets internes
    TFT_eSPI _tft = TFT_eSPI();                                                                             // Ecran TFT SD7735
    ROMV2_DISPLAY_HEADER _displayHeader = ROMV2_DISPLAY_HEADER();                                           // Objet pour l'affichage du Header
    ROMV2_DISPLAY_HOME _displayHome = ROMV2_DISPLAY_HOME();                                                 // Objet pour l'affichage du panneau Home
    ROMV2_DISPLAY_LUX _displayLux = ROMV2_DISPLAY_LUX();                                                    // Objet pour l'affichage du panneau Lux
    ROMV2_DISPLAY_ENVIRONMENT _displayEnvironment = ROMV2_DISPLAY_ENVIRONMENT();                            // Objet pour l'affichage du panneau Environnement
    ROMV2_DISPLAY_IRTEMP _displayIRTemp = ROMV2_DISPLAY_IRTEMP();                                           // Objet pour l'affichage du panneau Etat du ciel par température IR
    ROMV2_DISPLAY_ACCELERATION _displayAcceleration = ROMV2_DISPLAY_ACCELERATION();                         // Objet pour l'affichage du panneau Accélération (niveau à bulle)
    ROMV2_DISPLAY_GPS _displayGPS = ROMV2_DISPLAY_GPS();                                                    // Objet pour l'affichage du panneau GPS
    ROMV2_DISPLAY_TSL2591_CALIBRATION _displayTSL2591Calibration = ROMV2_DISPLAY_TSL2591_CALIBRATION();     // Objet pour l'affichage du panneau Calibration du TSL2591

    // Membres internes
    DISPLAY_SCREEN_TYPE _currentDisplayScreenType = DISPLAY_HOME;
    DISPLAY_SCREEN_TYPE _lastDisplayScreenType = DISPLAY_HOME;
    DISPLAY_HOME_TYPE _displayHomeType = DISPLAY_HOME_LUX;
    DataSensorEnvironment* _dataEnvironment;
    DataSensorLuminosity* _dataLuminosity;
    DataSensorSkyState* _dataSkyState;
    DataSensorAcceleration* _dataAcceleration;
    DataSensorGPS* _dataGPS;
    bool* _bluetoothConnected;

    // Chronos
    long _chronoDisplay;
};