#include <ROMV2_DISPLAY_ACCELERATION.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_ACCELERATION::ROMV2_DISPLAY_ACCELERATION()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataAcceleration"></param>
void ROMV2_DISPLAY_ACCELERATION::Init(TFT_eSPI* tft, DataSensorAcceleration* dataAcceleration)
{
	// Valorisation des membres internes
	_tft = tft;
	_dataAcceleration = dataAcceleration;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_ACCELERATION::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_ACCELERATION::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayAcceleration();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_ACCELERATION::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Panneau
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage du panneau Accélération (niveau à bulle)
/// </summary>
void ROMV2_DISPLAY_ACCELERATION::DisplayAcceleration()
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
		_tft->print("NIVEAUX");
	}

	// Squelette du niveau à bulle
	_tft->fillCircle(110, 80, 39, TFT_GOLD);
	_tft->drawCircle(110, 80, 40, TFT_BLACK);
	_tft->drawCircle(110, 80, 10, TFT_BLACK);

	// Affichage des bulles
	DrawBubble();

	// Affichage des données
	DisplayValues();
}

/// <summary>
/// Affichage des valeurs des données
/// </summary>
void ROMV2_DISPLAY_ACCELERATION::DisplayValues()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Affichage des libellés et icones
	if (_forceRedraw)
	{
		// Libellés
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->drawString(F("Valeurs:"), 7, 55, 1);
		_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
		_tft->drawString(F("(m/s^2)"), 7, 65, 1);
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->drawRightString(F("X:"), 20, 80, 1);
		_tft->drawRightString(F("Y:"), 20, 90, 1);
		_tft->drawRightString(F("Z:"), 20, 100, 1);
	}

	// Affichage des données
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setTextSize(1);
	char stringBuffer[20];

	// X
	_tft->setCursor(24, 80);
	if (!isnan(_dataAcceleration->x))
	{
		sprintf(stringBuffer, "%.2f", _dataAcceleration->x);
	}
	else
	{
		strcpy(stringBuffer, "      \0");
	}
	while (strlen(stringBuffer) < 7)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// Y
	_tft->setCursor(24, 90);
	if (!isnan(_dataAcceleration->y))
	{
		sprintf(stringBuffer, "%.2f", _dataAcceleration->y);
	}
	else
	{
		strcpy(stringBuffer, "      \0");
	}
	while (strlen(stringBuffer) < 7)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// Z
	_tft->setCursor(24, 100);
	if (!isnan(_dataAcceleration->z))
	{
		sprintf(stringBuffer, "%.2f", _dataAcceleration->z);
	}
	else
	{
		strcpy(stringBuffer, "      \0");
	}
	while (strlen(stringBuffer) < 7)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);
}

/// <summary>
/// Dessin de la bulle dans le niveau de type circulaire
/// </summary>
void ROMV2_DISPLAY_ACCELERATION::DrawBubble()
{
	float xScaled, yScaled;
	float xCorrected, yCorrected;
	float xModified, yModified;
	float aTanPoint;
	uint32_t bubbleColor = TFT_GREEN;

	// On scale la valeur de X
	if (_dataAcceleration->x < -5)
		xScaled = -5;
	else if (_dataAcceleration->x > 5)
		xScaled = 5;
	else
		xScaled = _dataAcceleration->x;

	// On scale la valeur de Y
	if (_dataAcceleration->y < -5)
		yScaled = -5;
	else if (_dataAcceleration->y > 5)
		yScaled = 5;
	else
		yScaled = _dataAcceleration->y;

	if (xScaled != 0)
	{
		// On calcule l'ArcTan du rapport
		aTanPoint = atan(yScaled / xScaled);

		// On en déduit les nouvelle coordonées
		xModified = xScaled * cos(aTanPoint);
		yModified = yScaled * sin(aTanPoint);

		// On Map les valeurs
		xCorrected = map(xModified, -5, 5, -35, 35);
		yCorrected = map(yModified, -5, 5, -35, 35);
		yCorrected = abs(yCorrected);
		if (_dataAcceleration->y < 0)
			yCorrected = yCorrected * -1;

		// On déduit la couleur de la bulle
		if (abs(xCorrected) > 12 || abs(yCorrected) > 12)
		{
			// Bulle rouge
			bubbleColor = TFT_RED;
		}
		else if (abs(xCorrected) > 6 || abs(yCorrected) > 6)
		{
			// Bulle orange
			bubbleColor = TFT_ORANGE;
		}
		else
		{
			// Bulle verte
			bubbleColor = TFT_GREEN;
		}

		// On dessine la bulle
		_tft->fillCircle(110 + xCorrected, 80 + yCorrected, 7, bubbleColor);
		_tft->drawCircle(110 + xCorrected, 80 + yCorrected, 8, TFT_SILVER);
	}
}
