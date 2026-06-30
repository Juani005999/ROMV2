#include <SQMLITE_TFT.h>

/// <summary>
/// Constructeur
/// </summary>
SQMLITE_TFT::SQMLITE_TFT()
{
}

/// <summary>
/// Initialisation des écrans propres au modèle.
/// </summary>
void SQMLITE_TFT::InitModelDisplays()
{
    // Initialisation des objets d'affichage
    _displayHeader.Init(_appType, &_tft, _dataEnvironment, _dataGPS, _dataWifi, _dataMqtt, _bluetoothConnected, _networkMode);
    _displayHome.Init(_appType, &_tft, _dataEnvironment, _dataLuminosity, _dataSkyState, _dataGPS);
    _displayLux.Init(&_tft, _dataLuminosity);
    _displayEnvironment.Init(&_tft, _dataEnvironment);
    _displayNetwork.Init(&_tft, _networkMode);
    _displayWifi.Init(&_tft, _dataWifi);
    _displayTSL2591Calibration.Init(&_tft, _dataLuminosity);

    // Positionnement ecran TFT
    _tft.setRotation(3);
}

/// <summary>
/// Affichage de l'écran d'initialisation de l'application
/// </summary>
void SQMLITE_TFT::DisplaySplashApp()
{
    // Couleur de fond
    _tft.fillScreen(TFT_MAROON);
    delay(500);

    // Titre principal
    _tft.setTextColor(TFT_GOLD, TFT_MAROON);
    _tft.drawString(F("S"), 40, 5, 2);
    delay(50);
    _tft.drawString(F("Q"), 50, 5, 2);
    delay(50);
    _tft.drawString(F("M"), 60, 5, 2);
    delay(50);
    _tft.drawString(F(" "), 70, 5, 2);
    delay(50);
    _tft.drawString(F("L"), 80, 5, 2);
    delay(50);
    _tft.drawString(F("i"), 90, 5, 2);
    delay(50);
    _tft.drawString(F("t"), 100, 5, 2);
    delay(50);
    _tft.drawString(F("e"), 110, 5, 2);
    delay(200);

    // Titre secondaire
    _tft.setCursor(25, 25);
    _tft.setTextSize(1);
    _tft.setTextColor(TFT_GOLD, TFT_MAROON);
    _tft.print(F("Sky Quality Meter"));
    //_tft.drawString(F("Sky Quality Meter"), 25, 25, 2);
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
/// Force la mise à jour complète de l'affichage
/// </summary>
void SQMLITE_TFT::ForceRedraw()
{
    _displayHeader.ForceRedraw();
    _displayHome.ForceRedraw();
    _displayLux.ForceRedraw();
    _displayEnvironment.ForceRedraw();
    _displayNetwork.ForceRedraw();
    _displayWifi.ForceRedraw();
    _displayTSL2591Calibration.ForceRedraw();
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void SQMLITE_TFT::UpdateDisplay()
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

            case DISPLAY_NETWORK:
                _displayNetwork.UpdateDisplay();
                break;

            case DISPLAY_WIFI:
                _displayWifi.UpdateDisplay();
                break;

            case DISPLAY_TSL2591_CALIBRATION:
                _displayTSL2591Calibration.UpdateDisplay();
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
