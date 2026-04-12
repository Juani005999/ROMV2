#include <ROMV2_DISPLAY_LUX.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_LUX::ROMV2_DISPLAY_LUX()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataLuminosity"></param>
void ROMV2_DISPLAY_LUX::Init(TFT_eSPI* tft, DataSensorLuminosity* dataLuminosity)
{
	// Valorisation des membres internes
	_tft = tft;
	_dataLuminosity = dataLuminosity;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_LUX::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_LUX::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayLux();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_LUX::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Panneau
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage du Lux
/// </summary>
void ROMV2_DISPLAY_LUX::DisplayLux()
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
		_tft->print("LUMINOSITE");

		// Libellés
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->drawRightString(F("Ir/Full :"), 80, 45, 1);
		_tft->drawRightString(F("Visible:"), 80, 55, 1);
		_tft->drawRightString(F("Lux:"), 80, 65, 1);
		_tft->drawRightString(F("Lux Moy.:"), 80, 75, 1);
		_tft->drawRightString(F("SQM:"), 80, 85, 1);
		_tft->drawRightString(F("Bortle:"), 80, 95, 1);
		_tft->drawRightString(F("Gain:"), 80, 105, 1);
		_tft->drawRightString(F("Expo.:"), 80, 115, 1);

		// Icone du Bortle
		_tft->setSwapBytes(true);
		_tft->pushImage(4, 83, 20, 20, icon_star_yellow_lightgrey);

		// Icone du sensor
		_tft->setSwapBytes(true);
		_tft->pushImage(5, 105, 20, 20, icon_sensor_lightgrey);
	}

	// Affichage des données
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setTextSize(1);

	// IR / Full
	char stringBuffer[20];
	_tft->setCursor(84, 45);
	if (!isnan(_dataLuminosity->ir) && _dataLuminosity->ir != 0)
	{
		sprintf(stringBuffer, "%.d/", (int)_dataLuminosity->ir);
	}
	else
	{
		strcat(stringBuffer, "_/");
	}
	if (!isnan(_dataLuminosity->full) && _dataLuminosity->full != 0)
	{
		sprintf(stringBuffer, "%s%.d", stringBuffer, (int)_dataLuminosity->full);
	}
	else
	{
		strcat(stringBuffer, "_");
	}
	while (strlen(stringBuffer) < 12)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// Visible
	_tft->setCursor(84, 55);
	if (!isnan(_dataLuminosity->visible))
	{
		sprintf(stringBuffer, "%.d", (int)_dataLuminosity->visible);
		while (strlen(stringBuffer) < 12)
		{
			strcat(stringBuffer, " ");
		}
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	_tft->print(stringBuffer);

	// Lux
	_tft->setCursor(84, 65);
	if (!isnan(_dataLuminosity->lux))
	{
		if (_dataLuminosity->lux >= 10000)
		{
			sprintf(stringBuffer, "%.1f", _dataLuminosity->lux);
		}
		else if (_dataLuminosity->lux >= 1000)
		{
			sprintf(stringBuffer, "%.2f", _dataLuminosity->lux);
		}
		else if (_dataLuminosity->lux >= 100)
		{
			sprintf(stringBuffer, "%.3f", _dataLuminosity->lux);
		}
		else if (_dataLuminosity->lux >= 10)
		{
			sprintf(stringBuffer, "%.4f", _dataLuminosity->lux);
		}
		else if (_dataLuminosity->lux >= 0)
		{
			sprintf(stringBuffer, "%.5f", _dataLuminosity->lux);
		}
		else
		{
			strcpy(stringBuffer, "            \0");
		}
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	while (strlen(stringBuffer) < 10)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// Lux Average
	_tft->setCursor(84, 75);
	if (!isnan(_dataLuminosity->luxAverage))
	{
		if (_dataLuminosity->luxAverage >= 100)
		{
			sprintf(stringBuffer, "%.4f(%.d)", _dataLuminosity->luxAverage, _dataLuminosity->luxAverageCount);
		}
		else
		{
			sprintf(stringBuffer, "%.5f(%.d)", _dataLuminosity->luxAverage, _dataLuminosity->luxAverageCount);
		}
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	while (strlen(stringBuffer) < 12)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// SQM
	_tft->setCursor(84, 85);
	if (!isnan(_dataLuminosity->sqm))
	{
		sprintf(stringBuffer, "%.5f", _dataLuminosity->sqm);
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	while (strlen(stringBuffer) < 12)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// Bortle
	_tft->setCursor(84, 95);
	if (!isnan(_dataLuminosity->bortle))
	{
		sprintf(stringBuffer, "%.1f", _dataLuminosity->bortle);
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	while (strlen(stringBuffer) < 12)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// Gain
	_tft->setCursor(84, 105);
	_tft->print(_dataLuminosity->luxSensorGain);

	// Timing
	_tft->setCursor(84, 115);
	_tft->print(_dataLuminosity->luxSensorTiming);
}
