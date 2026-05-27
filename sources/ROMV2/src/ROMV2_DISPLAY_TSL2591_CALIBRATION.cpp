#include <ROMV2_DISPLAY_TSL2591_CALIBRATION.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_TSL2591_CALIBRATION::ROMV2_DISPLAY_TSL2591_CALIBRATION()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataLuminosity"></param>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::Init(TFT_eSPI* tft, DataSensorLuminosity* dataLuminosity)
{
	// Valorisation des membres internes
	_tft = tft;
	_dataLuminosity = dataLuminosity;
}

/// <summary>
/// Met à jour la valeur courante de la calibration du TSL2591
/// </summary>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::SetNewCalibrationValue(int newCalibrationValue)
{
	_newCalibrationValue = newCalibrationValue;
}

/// <summary>
/// Augmente la calibration du TSL2591 de 0.01 si < TSL2591_CALIBRATION_MAX_VALUE
/// </summary>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::IncreaseCalibrationValue()
{
	if (_newCalibrationValue < TSL2591_CALIBRATION_MAX_VALUE)
	{
		_newCalibrationValue = _newCalibrationValue + 1;
	}
}

/// <summary>
/// Diminue la calibration du TSL2591 de 0.01 si > TSL2591_CALIBRATION_MIN_VALUE
/// </summary>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::DecreaseCalibrationValue()
{
	if (_newCalibrationValue > TSL2591_CALIBRATION_MIN_VALUE)
	{
		_newCalibrationValue = _newCalibrationValue - 1;
	}
}

/// <summary>
/// Renvoi la nouvelle valeur de la calibration du TSL2591
/// </summary>
int ROMV2_DISPLAY_TSL2591_CALIBRATION::GetNewCalibrationValue()
{
	if (isnan(_newCalibrationValue))
	{
		return 0;
	}
	return _newCalibrationValue;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayCalibration();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Panneau
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage de la calibration du TSL2591
/// </summary>
void ROMV2_DISPLAY_TSL2591_CALIBRATION::DisplayCalibration()
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
		_tft->print(F("CALIBRATION DU TSL2591"));

		// Libellés
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->drawRightString(F("Nouvelle:"), 80, 60, 1);
		_tft->drawRightString(F("Actuelle:"), 80, 80, 1);

		// Zone Edit
		_tft->fillRect(84, 53, 50, 20, TFT_WHITE);
		_tft->drawRect(84, 53, 50, 20, TFT_BLACK);
	}
	_tft->setTextColor(TFT_BROWN, TFT_LIGHTGREY);
	if (!isnan(_newCalibrationValue) && !isnan(_dataLuminosity->tsl2591Calibration) && _newCalibrationValue != _dataLuminosity->tsl2591Calibration)
	{
		_tft->drawCentreString(F("Clic pour sauvegarder"), 80, 100, 1);
	}
	else
	{
		_tft->drawCentreString(F("                       "), 80, 100, 1);
	}

	// Affichage des données
	_tft->setTextColor(TFT_DARKGREY, TFT_WHITE);
	if (!isnan(_newCalibrationValue) && !isnan(_dataLuminosity->tsl2591Calibration))
	{
		if (_newCalibrationValue == _dataLuminosity->tsl2591Calibration)
		{
			_tft->setTextColor(TFT_DARKGREEN, TFT_WHITE);
		}
		else
		{
			_tft->setTextColor(TFT_RED, TFT_WHITE);
		}
	}
	_tft->setTextSize(1);

	// Initialisation buffer
	char stringBuffer[20];
	stringBuffer[0] = 0;

	// Nouvelle valeur de Calibration
	if (!isnan(_newCalibrationValue))
	{
		sprintf(stringBuffer, "%.2f", _newCalibrationValue / 100.0f);
	}
	int len = strlen(stringBuffer);
	int addChar = 6 - len;
	if (addChar > 0 && addChar + len < 20)
	{
		memmove(stringBuffer + addChar, stringBuffer, len + 1);
		memset(stringBuffer, ' ', addChar);
	}
	_tft->drawRightString(stringBuffer, 125, 60, 1);

	// Valeur actuel de Calibration
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setCursor(90, 80);
	if (!isnan(_dataLuminosity->tsl2591Calibration))
	{
		sprintf(stringBuffer, "%.2f", _dataLuminosity->tsl2591Calibration / 100.0f);
	}
	while (strlen(stringBuffer) < 6)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);
}
