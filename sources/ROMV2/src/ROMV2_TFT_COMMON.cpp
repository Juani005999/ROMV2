#include <ROMV2_TFT_COMMON.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_TFT_COMMON::ROMV2_TFT_COMMON()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="dataEnvironment"></param>
/// <param name="dataLuminosity"></param>
/// <param name="dataSkyState"></param>
/// <param name="dataAcceleration"></param>
/// <param name="_dataGPS"></param>
/// <param name="bluetoothConnected"></param>
/// <param name="networkMode"></param>
void ROMV2_TFT_COMMON::Init(APP_TYPE appType,
    DataSensorEnvironment* dataEnvironment,
    DataSensorLuminosity* dataLuminosity,
    DataSensorSkyState* dataSkyState,
    DataSensorAcceleration* dataAcceleration,
    DataSensorGPS* dataGPS,
    DataWifi* dataWifi,
    DataMqtt* dataMqtt,
    bool* bluetoothConnected,
    NETWORK_MODE* networkMode)
{
    // Valorisation des champs internes
    _appType = appType;
    _dataEnvironment = dataEnvironment;
    _dataLuminosity = dataLuminosity;
    _dataSkyState = dataSkyState;
    _dataAcceleration = dataAcceleration;
    _dataGPS = dataGPS;
    _dataWifi = dataWifi;
    _dataMqtt = dataMqtt;
    _bluetoothConnected = bluetoothConnected;
    _networkMode = networkMode;

    // Initialisation ecran TFT
    _tft.init();
    _tft.setTextFont(TFT_FONT_DEFAULT);
    _tft.fillScreen(TFT_BLACK);
    _tft.setRotation(1);

    // Initialisation des objets d'affichage
    InitModelDisplays();

    // Splash App
    DisplaySplashApp();

    // Réinitialisation de l'affichage
    _tft.fillScreen(TFT_BLACK);
    _tft.setCursor(0, 0);
    _tft.setTextColor(TFT_SILVER, TFT_BLACK);
    _tft.setTextSize(1);

    // Initialisation des chronos
    _chronoDisplay = 0;
}

/// <summary>
/// Met à jour l'affichage en cours
/// </summary>
/// <param name="currentDisplayScreenType"></param>
/// <param name="displayHomeType"></param>
void ROMV2_TFT_COMMON::SetCurrentDisplay(DISPLAY_SCREEN_TYPE currentDisplayScreenType, DISPLAY_HOME_TYPE displayHomeType)
{
    _currentDisplayScreenType = currentDisplayScreenType;
    _displayHomeType = displayHomeType;
}

/// <summary>
/// Affiche l'icone d'état de lecture de la luminosité
/// </summary>
void ROMV2_TFT_COMMON::SetLuminosityIcon(bool on)
{
    _displayHeader.SetLuminosityIcon(on);
}

/// <summary>
/// Met à jour la valeur courante de la calibration du TSL2591
/// </summary>
void ROMV2_TFT_COMMON::SetNewCalibrationValue(int newCalibrationValue)
{
    _displayTSL2591Calibration.SetNewCalibrationValue(newCalibrationValue);
}

/// <summary>
/// Augmente la calibration du TSL2591 de 0.01 si < TSL2591_CALIBRATION_MAX_VALUE
/// </summary>
void ROMV2_TFT_COMMON::IncreaseCalibrationValue()
{
    _displayTSL2591Calibration.IncreaseCalibrationValue();
}

/// <summary>
/// Diminue la calibration du TSL2591 de 0.01 si > TSL2591_CALIBRATION_MIN_VALUE
/// </summary>
void ROMV2_TFT_COMMON::DecreaseCalibrationValue()
{
    _displayTSL2591Calibration.DecreaseCalibrationValue();
}

/// <summary>
/// Renvoi la nouvelle valeur de la calibration du TSL2591
/// </summary>
int ROMV2_TFT_COMMON::GetNewCalibrationValue()
{
    return _displayTSL2591Calibration.GetNewCalibrationValue();
}

/// <summary>
/// Modification du réseau en fonction de la direction (Up / Down)
/// </summary>
void ROMV2_TFT_COMMON::SetNewNetworkMode(bool directionUp)
{
    _displayNetwork.SetNewNetworkMode(directionUp);
}

/// <summary>
/// Sauvegarde les credentials réseau en Preferences
/// </summary>
void ROMV2_TFT_COMMON::SaveNetworkCredentials()
{
    _displayNetwork.SaveNetworkCredentials();
}
