#include <ROMV2_TFT.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_TFT::ROMV2_TFT()
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
void ROMV2_TFT::Init(DataSensorEnvironment* dataEnvironment,
    DataSensorLuminosity* dataLuminosity,
    DataSensorSkyState* dataSkyState,
    DataSensorAcceleration* dataAcceleration,
    DataSensorGPS* dataGPS,
    bool* bluetoothConnected)
{
    // Valorisation des champs internes
    _dataEnvironment = dataEnvironment;
    _dataLuminosity = dataLuminosity;
    _dataSkyState = dataSkyState;
    _dataAcceleration = dataAcceleration;
    _dataGPS = dataGPS;
    _bluetoothConnected = bluetoothConnected;

    // Initialisation ecran TFT
    _tft.init();
    _tft.fillScreen(TFT_BLACK);
    _tft.setRotation(1);

    // Initialisation des objets d'affichage
    _displayHeader.Init(&_tft, _dataEnvironment, _dataGPS, _bluetoothConnected);
    _displayHome.Init(&_tft, _dataEnvironment, _dataLuminosity, _dataSkyState, _dataGPS);
    _displayLux.Init(&_tft, _dataLuminosity);
    _displayEnvironment.Init(&_tft, _dataEnvironment);
    _displayIRTemp.Init(&_tft, _dataSkyState);
    _displayAcceleration.Init(&_tft, _dataAcceleration);
    _displayGPS.Init(&_tft, _dataGPS);

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
void ROMV2_TFT::SetCurrentDisplay(DISPLAY_SCREEN_TYPE currentDisplayScreenType, DISPLAY_HOME_TYPE displayHomeType)
{
    _currentDisplayScreenType = currentDisplayScreenType;
    _displayHomeType = displayHomeType;
}

/// <summary>
/// Affichage de l'écran d'initialisation de l'application
/// </summary>
void ROMV2_TFT::DisplaySplashApp()
{
    // Couleur de fond
    _tft.fillScreen(TFT_MAROON);
    delay(500);

    // Titre principal
    _tft.setTextColor(TFT_GOLD, TFT_MAROON);
    _tft.drawString(F("R"), 55, 5, 2);
    delay(50);
    _tft.drawString(F("O"), 65, 5, 2);
    delay(50);
    _tft.drawString(F("M"), 75, 5, 2);
    delay(50);
    _tft.drawString(F(" "), 85, 5, 2);
    delay(50);
    _tft.drawString(F("V"), 95, 5, 2);
    delay(50);
    _tft.drawString(F("2"), 105, 5, 2);
    delay(200);
 
    // Titre secondaire
    _tft.setCursor(25, 25);
    _tft.setTextSize(1);
    _tft.setTextColor(TFT_GOLD, TFT_MAROON);
    _tft.print(F("Sky Quality Meter"));
    delay(100);

    // Signature
    _tft.setTextColor(TFT_GOLD, TFT_MAROON);
    _tft.drawString(F("JUANITO del PEPITO"), 15, 110, 2);
    delay(100);

    // Cercle central
    int lastAngle = 0;
    int currentAngle = 1;
    while (currentAngle < 360)
    {
        _tft.drawSmoothArc(80, 70, 30, 28, lastAngle, currentAngle, TFT_GOLD, TFT_SILVER);
        lastAngle = currentAngle;
        currentAngle++;
        delay(2);
    }

    int loopInsideCircle = 0;
    bool showInsideCircle = true;
    while (loopInsideCircle < 10)
    {
        if (showInsideCircle)
        {
            _tft.fillCircle(80, 70, 28, TFT_LIGHTGREY);
            _tft.setSwapBytes(true);
            _tft.pushImage(62, 52, 36, 36, icon_star_big_lightgrey);
        }
        else
        {
            _tft.fillCircle(80, 70, 28, TFT_MAROON);
        }
        delay(500);
        showInsideCircle = !showInsideCircle;
        loopInsideCircle++;
    }
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_TFT::UpdateDisplay()
{
    if (millis() > _chronoDisplay + DISPLAY_INTERVAL)
    {
        // Mise à jour du mode d'affichage du Header
        _displayHeader.SetCurrentDisplay(_currentDisplayScreenType);

        // Affichage en mode veille
        if (_currentDisplayScreenType == DISPLAY_NONE)
        {
            // On effectue la mise à jour de l'écran seulement si on est pas déjà en mode veille
            if (_lastDisplayScreenType != DISPLAY_NONE)
            {
                // On affiche un écran noir en mode veille
                _tft.fillScreen(TFT_BLACK);
            }
        }
        else
        {
            // Actualisation du Header
            _displayHeader.UpdateDisplay();

            // Actualisation du panneau principal
            switch (_currentDisplayScreenType)
            {
                case DISPLAY_LUX:
                    _displayLux.UpdateDisplay();
                    break;

                case DISPLAY_ENVIRONMENT:
                    _displayEnvironment.UpdateDisplay();
                    break;

                case DISPLAY_IRTEMP:
                    _displayIRTemp.UpdateDisplay();
                    break;

                case DISPLAY_ACCELERATION:
                    _displayAcceleration.UpdateDisplay();
                    break;

                case DISPLAY_GPS:
                    _displayGPS.UpdateDisplay();
                    break;

                case DISPLAY_HOME:
                default:
                    _displayHome.SetCurrentDisplay(_displayHomeType);
                    _displayHome.UpdateDisplay();
                    break;
            }
        }

        // Actualisation flag et compteurs
        _chronoDisplay = millis();
        _lastDisplayScreenType = _currentDisplayScreenType;
    }
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
void ROMV2_TFT::ForceRedraw()
{
    _displayHeader.ForceRedraw();
    _displayHome.ForceRedraw();
    _displayLux.ForceRedraw();
    _displayEnvironment.ForceRedraw();
    _displayIRTemp.ForceRedraw();
    _displayAcceleration.ForceRedraw();
    _displayGPS.ForceRedraw();
}

/// <summary>
/// Affiche l'icone d'état de lecture de la luminosité
/// </summary>
void ROMV2_TFT::SetLuminosityIcon(bool on)
{
    _displayHeader.SetLuminosityIcon(on);
}
