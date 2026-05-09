#include <ROMV2_DISPLAY_GPS.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_GPS::ROMV2_DISPLAY_GPS()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataGPS"></param>
void ROMV2_DISPLAY_GPS::Init(TFT_eSPI* tft, DataSensorGPS* dataGPS)
{
	// Valorisation des membres internes
	_tft = tft;
	_dataGPS = dataGPS;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_GPS::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_GPS::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayGPS();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_GPS::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Panneau
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage du panneau GPS
/// </summary>
void ROMV2_DISPLAY_GPS::DisplayGPS()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Affichage des libellés et icones
	if (_forceRedraw)
	{
		// Titre
		_tft->setTextColor(TFT_BROWN, TFT_LIGHTGREY);
		_tft->setTextSize(1);
		_tft->setCursor(10, 35);
		_tft->print("GPS");

		// Libellés
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->setTextSize(1);
		_tft->drawRightString(F("Long:"), 62, 48, 1);
		_tft->drawRightString(F("Lat:"), 62, 60, 1);
		_tft->drawRightString(F("Sat.:"), 62, 78, 1);
		_tft->drawRightString(F("Fix:"), 62, 90, 1);
		_tft->drawRightString(F("Alt:"), 62, 109, 1);

		// Icone Altitude
		_tft->setSwapBytes(true);
		_tft->pushImage(5, 48, 20, 20, icon_target_lightgrey);
		_tft->pushImage(5, 76, 20, 20, icon_sensor_lightgrey);
		_tft->pushImage(5, 100, 20, 20, icon_altitude_lightgrey);
	}

	// Affichage des données
	_tft->setTextSize(1);
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setCursor(64, 48);
	if (_dataGPS->gpsFix && strlen(_dataGPS->gpsLongitude) > 0)
		_tft->print(_dataGPS->gpsLongitude);
	else
		_tft->print(F("              "));

	_tft->setCursor(64, 60);
	if (_dataGPS->gpsFix && strlen(_dataGPS->gpsLatitude) > 0)
		_tft->print(_dataGPS->gpsLatitude);
	else
		_tft->print(F("              "));

	_tft->setCursor(64, 78);
	if (strlen(_dataGPS->gpsSatellites) > 0)
		_tft->print(_dataGPS->gpsSatellites);
	else
		_tft->print(F("  "));

	_tft->setCursor(64, 90);
	if (_dataGPS->gpsFix)
	{
		_tft->print("Oui");
	}
	else
	{
		_tft->print("Non");
	}
	_tft->setCursor(64, 109);
	if (_dataGPS->gpsFix && strlen(_dataGPS->gpsAltitude) > 0)
		_tft->print(_dataGPS->gpsAltitude);
	else
		_tft->print(F("              "));
}
