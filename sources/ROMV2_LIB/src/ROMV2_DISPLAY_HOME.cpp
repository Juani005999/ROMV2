#include <ROMV2_DISPLAY_HOME.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_HOME::ROMV2_DISPLAY_HOME()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataEnvironment"></param>
/// <param name="dataLuminosity"></param>
/// <param name="dataSkyState"></param>
/// <param name="dataGPS"></param>
void ROMV2_DISPLAY_HOME::Init(TFT_eSPI* tft,
	DataSensorEnvironment* dataEnvironment,
	DataSensorLuminosity* dataLuminosity,
	DataSensorSkyState* dataSkyState,
	DataSensorGPS* dataGPS)
{
	// Initialisation des objets internes
	_tft = tft;
	_dataEnvironment = dataEnvironment;
	_dataLuminosity = dataLuminosity;
	_dataSkyState = dataSkyState;
	_dataGPS = dataGPS;

	// Initialisation des membres
	_forceRedraw = true;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_HOME::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Met à jour les variables de l'objet
/// </summary>
/// <param name="displayHomeType"></param>
void ROMV2_DISPLAY_HOME::SetCurrentDisplay(DISPLAY_HOME_TYPE displayHomeType)
{
	// Actualisation des membres internes
	_displayHomeType = displayHomeType;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_HOME::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayCoordinates();
	DisplayTemperature();
	DisplayLux();
	DisplayDewPoint();
	DisplayBortle();
	DisplaySkyState();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_HOME::ClearDisplay()
{
	// Mise à jour complète de l'affichage
	if (_forceRedraw)
	{
		// Partie haute
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, 50, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, 50, TFT_DARKGREY);

		// Partie gauche
		_tft->fillRect(0, SCREEN_HDR_HEIGHT + 50, 90, SCREEN_HEIGHT - 80, TFT_MAROON);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT + 50, 90, SCREEN_HEIGHT - 80, TFT_DARKGREY);

		// Partie droite
		_tft->fillRect(90, SCREEN_HDR_HEIGHT + 50, 70, SCREEN_HEIGHT - 80, TFT_ORANGE);
		_tft->drawRect(90, SCREEN_HDR_HEIGHT + 50, 70, SCREEN_HEIGHT - 80, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage du Lux
/// </summary>
void ROMV2_DISPLAY_HOME::DisplayLux()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Affichage icone et libellé sur mise à jour complète de l'affichage
	if (_forceRedraw)
	{
		_tft->setSwapBytes(true);
		_tft->pushImage(1, 85, 20, 20, icon_ampoule_maroon);

		_tft->setTextColor(TFT_LIGHTGREY, TFT_MAROON);
		_tft->setCursor(25, 92);
		_tft->setTextSize(1);
		if (_displayHomeType == DISPLAY_HOME_SQM)
		{
			_tft->setCursor(25, 85);
			_tft->print(F("SQM"));
			_tft->setCursor(23, 95);
			_tft->print(F("(Mg/Asec^2)"));
		}
		else
		{
			_tft->print(F("Lux"));
		}
	}

	// Affichage des données
	_tft->setTextColor(TFT_LIGHTGREY, TFT_MAROON);
	_tft->setTextSize(2);
	_tft->setCursor(4, 110);
	char stringBuffer[12];
	if (_displayHomeType == DISPLAY_HOME_SQM)
	{
		if (!isnan(_dataLuminosity->sqm))
		{
			if (_dataLuminosity->sqm >= 10)
			{
				sprintf(stringBuffer, "%.4f", _dataLuminosity->sqm);
			}
			else
			{
				sprintf(stringBuffer, "%.5f", _dataLuminosity->sqm);
			}
		}
		else
		{
			strcpy(stringBuffer, "       \0");
		}
	}
	else
	{
		if (!isnan(_dataLuminosity->luxAverage))
		{
			if (_dataLuminosity->luxAverage >= 10000)
			{
				sprintf(stringBuffer, "%.1f", _dataLuminosity->luxAverage);
			}
			else if (_dataLuminosity->luxAverage >= 1000)
			{
				sprintf(stringBuffer, "%.2f", _dataLuminosity->luxAverage);
			}
			else if (_dataLuminosity->luxAverage >= 100)
			{
				sprintf(stringBuffer, "%.3f", _dataLuminosity->luxAverage);
			}
			else if (_dataLuminosity->luxAverage >= 10)
			{
				sprintf(stringBuffer, "%.4f", _dataLuminosity->luxAverage);
			}
			else if (_dataLuminosity->luxAverage >= 0)
			{
				sprintf(stringBuffer, "%.5f", _dataLuminosity->luxAverage);
			}
			else
			{
				strcpy(stringBuffer, "       \0");
			}
		}
		else
		{
			strcpy(stringBuffer, "       \0");
		}
	}
	while (strlen(stringBuffer) < 7)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);
}

/// <summary>
/// Affichage de la température
/// </summary>
void ROMV2_DISPLAY_HOME::DisplayTemperature()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Affichage icone et libellé sur mise à jour complète de l'affichage
	if (_forceRedraw)
	{
		_tft->setSwapBytes(true);
		_tft->pushImage(91, 85, 20, 20, icon_thermometre_orange);

		_tft->setTextColor(TFT_DARKGREY, TFT_ORANGE);
		_tft->setCursor(111, 92);
		_tft->setTextSize(1);
		_tft->print(F("Temp(c)"));
	}

	// Affichage des données
	_tft->setTextColor(TFT_DARKGREY, TFT_ORANGE);
	_tft->setTextSize(2);
	_tft->setCursor(94, 110);
	char stringBuffer[12];
	if (!isnan(_dataEnvironment->temperature))
	{
		if (_dataEnvironment->temperature >= 100)
		{
			sprintf(stringBuffer, "%.1f", _dataEnvironment->temperature);
		}
		else if (_dataEnvironment->temperature >= 10)
		{
			sprintf(stringBuffer, "%.2f", _dataEnvironment->temperature);
		}
		else if (_dataEnvironment->temperature >= 0)
		{
			sprintf(stringBuffer, "%.3f", _dataEnvironment->temperature);
		}
		else if (_dataEnvironment->temperature <= -10)
		{
			sprintf(stringBuffer, "%.1f", _dataEnvironment->temperature);
		}
		else if (_dataEnvironment->temperature < 0)
		{
			sprintf(stringBuffer, "%.2f", _dataEnvironment->temperature);
		}
		else
		{
			strcpy(stringBuffer, "     \0");
		}
	}
	else
	{
		strcpy(stringBuffer, "     \0");
	}
	_tft->print(stringBuffer);
}

/// <summary>
/// Affichage des coordonnées GPS
/// </summary>
void ROMV2_DISPLAY_HOME::DisplayCoordinates()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Affichage icone et libellés sur mise à jour complète de l'affichage
	if (_forceRedraw)
	{
		_tft->setSwapBytes(true);
		_tft->pushImage(5, 35, 20, 20, icon_gps_lightgrey);

		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->setTextSize(1);
		_tft->drawRightString(F("Long:"), 62, 38, 1);
		_tft->drawRightString(F("Lat:"), 62, 49, 1);
	}

	// Affichage des données
	_tft->setTextSize(1);
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setCursor(64, 38);
	if (_dataGPS->gpsFix && strlen(_dataGPS->gpsLongitude) > 0)
		_tft->print(_dataGPS->gpsLongitude);
	else
		_tft->print(F("              "));

	_tft->setCursor(64, 49);
	if (_dataGPS->gpsFix && strlen(_dataGPS->gpsLatitude) > 0)
		_tft->print(_dataGPS->gpsLatitude);
	else
		_tft->print(F("              "));
}

/// <summary>
/// Affichage du point de rosée
/// </summary>
void ROMV2_DISPLAY_HOME::DisplayDewPoint()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Icone de l'état du Point de rosée : Affichage sur MAJ complète de l'écran ou sur changement d'état
	if (_forceRedraw || _dataEnvironment->dewPointState != _dewPointStateLast)
	{
		// Icone du Point de Rosée
		_tft->setSwapBytes(true);
		switch (_dataEnvironment->dewPointState)
		{
		case DEWPOINT_STATE_DRY:
			_tft->pushImage(38, 58, 20, 20, icon_goutte_blue_lightgrey);
			break;

		case DEWPOINT_STATE_HUMID:
			_tft->pushImage(38, 58, 20, 20, icon_goutte_orange_lightgrey);
			break;

		case DEWPOINT_STATE_WET:
			_tft->pushImage(38, 58, 20, 20, icon_goutte_red_lightgrey);
			break;

		case DEWPOINT_STATE_UNKNOWN:
		default:
			_tft->drawRect(38, 58, 20, 20, TFT_LIGHTGREY);
			break;
		}
	}

	// Mise à jour des flags
	_dewPointStateLast = _dataEnvironment->dewPointState;

	// Affichage des données
	_tft->setTextSize(1);
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setCursor(58, 65);
	char stringBuffer[12];
	if (!isnan(_dataEnvironment->dewPoint))
	{
		sprintf(stringBuffer, "%.1f(c)", _dataEnvironment->dewPoint);
	}
	else
	{
		strcpy(stringBuffer, "         \0");
	}
	while (strlen(stringBuffer) < 8)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);
}

/// <summary>
/// Affichage du Bortle
/// </summary>
void ROMV2_DISPLAY_HOME::DisplayBortle()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Affichage icone sur mise à jour complète de l'affichage
	if (_forceRedraw)
	{
		_tft->setSwapBytes(true);
		_tft->pushImage(111, 58, 20, 20, icon_star_yellow_lightgrey);
	}

	// Affichage des données
	_tft->setTextSize(1);
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setCursor(136, 65);
	char stringBuffer[12];
	if (!isnan(_dataLuminosity->bortle))
	{
		sprintf(stringBuffer, "%.1f", _dataLuminosity->bortle);
	}
	else
	{
		strcpy(stringBuffer, "   \0");
	}
	_tft->print(stringBuffer);
}

/// <summary>
/// Affichage de l'état du ciel
/// </summary>
void ROMV2_DISPLAY_HOME::DisplaySkyState()
{
	// Icone de l'état du ciel : Affichage sur MAJ complète de l'écran ou sur changement d'état
	if (_forceRedraw || _dataSkyState->skyState != _skyStateLast)
	{
		_tft->setSwapBytes(true);
		switch (_dataSkyState->skyState)
		{
		case SKY_STATE_CLEAR:
			_tft->pushImage(3, 59, 20, 20, icon_sky_clear_lightgrey);
			break;

		case SKY_STATE_PARTLY_CLOUDY:
			_tft->pushImage(3, 59, 20, 20, icon_sky_partly_cloudy_lightgrey);
			break;

		case SKY_STATE_CLOUDY:
			_tft->pushImage(3, 59, 20, 20, icon_sky_cloudy_lightgrey);
			break;

		case SKY_STATE_UNKNOWN:
		default:
			_tft->drawRect(3, 59, 20, 20, TFT_LIGHTGREY);
			break;
		}
	}

	// Mise à jour des flags
	_skyStateLast = _dataSkyState->skyState;
}
