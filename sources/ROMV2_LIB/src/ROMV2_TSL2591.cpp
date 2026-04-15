#include <ROMV2_TSL2591.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_TSL2591::ROMV2_TSL2591()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataLuminosity"></param>
void ROMV2_TSL2591::Init(ROMV2_TFT* tft, DataSensorLuminosity* dataLuminosity)
{
    // Valorisation des membres internes
    _tft = tft;
    _dataLuminosity = dataLuminosity;

    // Initialisation TSL2591
    if (!_tsl2591.begin()) {
        debugln(F("Could not find a valid TSL2591 sensor, check wiring!"));
        while (1);
    }

    // Configuration du capteur
    DisplaySensorDetails();
    ConfigureSensor();

    // Initialisation des chronos
    _chronoReadLux = 0;
}

/// <summary>
/// Lecture de la Luminosité
/// </summary>
void ROMV2_TSL2591::ReadLuminosity()
{
    // Lecture de luminosité sur intervalle
    if (millis() > _chronoReadLux + READ_LUX_INTERVAL)
    {
        // On positionne l'icone de lecture d'état
        _tft->SetLuminosityIcon(true);

        // Lecture des données du sensor
        uint32_t lum = _tsl2591.getFullLuminosity();
        _dataLuminosity->ir = lum >> 16;
        _dataLuminosity->full = lum & 0xFFFF;
        _dataLuminosity->visible = _dataLuminosity->full - _dataLuminosity->ir;
        _dataLuminosity->lux = _tsl2591.calculateLux(_dataLuminosity->full, _dataLuminosity->ir);

        // Si les données lues sont dans les limites
        if (!isnan(_dataLuminosity->lux) && !isinf(_dataLuminosity->lux)
            && _dataLuminosity->lux > MIN_LUX_THRESHOLD && _dataLuminosity->full != 0xFFFF && _dataLuminosity->ir != 0xFFFF)
        {
            // On ajoute à la queue et on récupère la moyenne et la taille
            _luxAverageQueue.Push(_dataLuminosity->lux);
            _luxAverageQueue.Average(_dataLuminosity->luxAverage);
            _dataLuminosity->luxAverageCount = _luxAverageQueue.Size();
        }

        // Update de la valeur de SQM et de Bortle en fonction des valeurs mesurées
        UpdateSQM();
        UpdateBortle();

        // Trace
        debugln(F(""));
        debug(F("[LUX] IR: "));
        debugln(_dataLuminosity->ir);
        debug(F("[LUX] Full: "));
        debugln(_dataLuminosity->full);
        debug(F("[LUX] Visible: "));
        debugln(_dataLuminosity->visible);
        debug(F("[LUX] Lux: "));
        debugln(_dataLuminosity->lux);
        debug(F("[LUX] Lux Average: "));
        debugln(_dataLuminosity->luxAverage);
        debug(F("[LUX] Lux Average Count: "));
        debugln(_dataLuminosity->luxAverageCount);
        debug(F("[LUX] SQM: "));
        debugln(_dataLuminosity->sqm);
        debug(F("[LUX] Bortle: "));
        debugln(_dataLuminosity->bortle);
        debug(F("[LUX] Sensor Gain: "));
        debugln(_dataLuminosity->luxSensorGain);
        debug(F("[LUX] Sensor Timing: "));
        debugln(_dataLuminosity->luxSensorTiming);

        // Ajustement des valeurs du sensor si nécessaire
        CheckSensorValues();

        // On positionne l'icone de lecture d'état
        _tft->SetLuminosityIcon(false);

        // Actualisation flag et compteurs
        _chronoReadLux = millis();
    }
}

/// <summary>
/// Affichage des détails du sensor TSL2591
/// </summary>
void ROMV2_TSL2591::DisplaySensorDetails()
{
    sensor_t sensor;
    _tsl2591.getSensor(&sensor);

    debugln(F(""));
    debugln(F("[LUX] Sensor TSL2591 : Details"));
    debug(F("Sensor:       ")); debugln(sensor.name);
    debug(F("Driver Ver:   ")); debugln(sensor.version);
    debug(F("Unique ID:    ")); debugln(sensor.sensor_id);
    debug(F("Max Value:    ")); debug(sensor.max_value); debugln(F(" lux"));
    debug(F("Min Value:    ")); debug(sensor.min_value); debugln(F(" lux"));
    debug(F("Resolution:   ")); debug(sensor.resolution); debugln(F(" lux"));  
}

/// <summary>
/// Configuration du sensor TSL2591
/// </summary>
/// <param name="gain"></param>
/// <param name="timing"></param>
void ROMV2_TSL2591::ConfigureSensor(tsl2591Gain_t gain, tsl2591IntegrationTime_t timing)
{
    // Mise à jour des membres internes
    _gain = gain;
    _timing = timing;

    // Mise à jour du Sensor
    _tsl2591.setGain(_gain);
    _tsl2591.setTiming(_timing);

    // Trace Gain
    debugln(F(""));
    debugln(F("[LUX] Sensor TSL2591 : Configure"));
    debug(F("[LUX] Gain:         "));
    switch (_gain)
    {
        case TSL2591_GAIN_LOW:
            strcpy(_dataLuminosity->luxSensorGain, "1x (Low)");
            break;
        case TSL2591_GAIN_MED:
            strcpy(_dataLuminosity->luxSensorGain, "25x (Medium)");
            break;
        case TSL2591_GAIN_HIGH:
            strcpy(_dataLuminosity->luxSensorGain, "428x (High)");
            break;
        case TSL2591_GAIN_MAX:
            strcpy(_dataLuminosity->luxSensorGain, "9876x (Max)");
            break;
        default:
            strcpy(_dataLuminosity->luxSensorGain, "            ");
            break;
    }
    while (strlen(_dataLuminosity->luxSensorGain) < 12)
    {
        strcat(_dataLuminosity->luxSensorGain, " ");
    }
    debugln(_dataLuminosity->luxSensorGain);

    // Trace Timing
    sprintf(_dataLuminosity->luxSensorTiming, "%d ms", (int)(_timing + 1) * 100);
    debug(F("[LUX] Timing:       "));
    debugln(_dataLuminosity->luxSensorTiming);
}

/// <summary>
/// Vérifie les valeurs du sensor TSL2591 pour un ajustement si nécessaire
/// </summary>
void ROMV2_TSL2591::CheckSensorValues()
{
    if (!isnan(_dataLuminosity->full) && _dataLuminosity->full > TSL2591_SENSOR_VALUE_HIGH)
    {
        DecreaseSensor();
    }
    else if (!isnan(_dataLuminosity->full) && _dataLuminosity->full < TSL2591_SENSOR_VALUE_LOW)
    {
        IncreaseSensor();
    }
}

/// <summary>
/// Si le niveau du sensor est trop haut, on réduit le gain ou augmente le timing
/// </summary>
void ROMV2_TSL2591::DecreaseSensor()
{
    // 1 X sur 2 on réduit le gain ou on augmente le timing
    if (!_lastDecreaseGain)
    {
        // Réduction du gain
        DecreaseGain();
    }
    else
    {
        // Réduction du timing
        DecreaseTiming();
    }

    // Update du flag
    _lastDecreaseGain = !_lastDecreaseGain;

    // Update du sensor
    ConfigureSensor(_gain, _timing);
}

/// <summary>
/// Si le niveau du sensor est trop bas, on augmente le gain ou réduit le timing
/// </summary>
void ROMV2_TSL2591::IncreaseSensor()
{
    // 1 X sur 2 on réduit le gain ou on augmente le timing
    if (!_lastIncreaseGain)
    {
        // Augmentation du gain
        IncreaseGain();
    }
    else
    {
        // Augmentation du timing
        IncreaseTiming();
    }

    // Update du flag
    _lastIncreaseGain = !_lastIncreaseGain;

    // Update du sensor
    ConfigureSensor(_gain, _timing);
}

/// <summary>
/// Réduction du gain du sensor
/// </summary>
void ROMV2_TSL2591::DecreaseGain()
{
    switch (_gain)
    {
        case TSL2591_GAIN_MAX:
            _gain = TSL2591_GAIN_HIGH;
            break;

        case TSL2591_GAIN_HIGH:
            _gain = TSL2591_GAIN_MED;
            break;

        case TSL2591_GAIN_MED:
        default:
            _gain = TSL2591_GAIN_LOW;
            break;
    }
}

/// <summary>
/// Augemntation du gain du sensor
/// </summary>
void ROMV2_TSL2591::IncreaseGain()
{
    switch (_gain)
    {
        case TSL2591_GAIN_LOW:
            _gain = TSL2591_GAIN_MED;
            break;

        case TSL2591_GAIN_MED:
            _gain = TSL2591_GAIN_HIGH;
            break;

        case TSL2591_GAIN_HIGH:
        default:
            _gain = TSL2591_GAIN_MAX;
            break;
        }
}

/// <summary>
/// Réduction du timing du sensor
/// </summary>
void ROMV2_TSL2591::DecreaseTiming()
{
    switch (_timing)
    {
        case TSL2591_INTEGRATIONTIME_600MS:
            _timing = TSL2591_INTEGRATIONTIME_500MS;
            break;

        case TSL2591_INTEGRATIONTIME_500MS:
            _timing = TSL2591_INTEGRATIONTIME_400MS;
            break;

        case TSL2591_INTEGRATIONTIME_400MS:
            _timing = TSL2591_INTEGRATIONTIME_300MS;
            break;

        case TSL2591_INTEGRATIONTIME_300MS:
            _timing = TSL2591_INTEGRATIONTIME_200MS;
            break;

        case TSL2591_INTEGRATIONTIME_200MS:
        default:
            _timing = TSL2591_INTEGRATIONTIME_100MS;
            break;
    }
}

/// <summary>
/// Augmentation du timing du sensor
/// </summary>
void ROMV2_TSL2591::IncreaseTiming()
{
    switch (_timing)
    {
    case TSL2591_INTEGRATIONTIME_100MS:
        _timing = TSL2591_INTEGRATIONTIME_200MS;
        break;

    case TSL2591_INTEGRATIONTIME_200MS:
        _timing = TSL2591_INTEGRATIONTIME_300MS;
        break;

    case TSL2591_INTEGRATIONTIME_300MS:
        _timing = TSL2591_INTEGRATIONTIME_400MS;
        break;

    case TSL2591_INTEGRATIONTIME_400MS:
        _timing = TSL2591_INTEGRATIONTIME_500MS;
        break;

    case TSL2591_INTEGRATIONTIME_500MS:
    default:
        _timing = TSL2591_INTEGRATIONTIME_600MS;
        break;
    }
}

/// <summary>
/// Update de la valeur de SQM en fonction des valeurs mesurées
/// </summary>
void ROMV2_TSL2591::UpdateSQM()
{
    if (_dataLuminosity->luxAverageCount > 0)
    {
        // Conversion en magnitude de surface à partir du lux
        LuxToMagConversionResult r = LuxToMagnitude(_dataLuminosity->luxAverage);
        if (r.valid)
        {
            _dataLuminosity->sqm = r.magnitude_arcsec2;
        }
        else
        {
            _dataLuminosity->sqm = NAN;
        }
    }
    else
    {
        _dataLuminosity->sqm = NAN;
    }
}

/// <summary>
/// Update de la valeur de Bortle en fonction du SQM
/// </summary>
void ROMV2_TSL2591::UpdateBortle()
{
    if (!isnan(_dataLuminosity->sqm) && !isinf(_dataLuminosity->sqm))
    {
        // Positionnement du Bortle selon une échelle de valeur de SQM
        if (_dataLuminosity->sqm > 21.995)
        {
            _dataLuminosity->bortle = 1;
        }
        else if (_dataLuminosity->sqm > 21.99)
        {
            _dataLuminosity->bortle = 1.5;
        }
        else if (_dataLuminosity->sqm > 21.94)
        {
            _dataLuminosity->bortle = 2;
        }
        else if (_dataLuminosity->sqm > 21.89)
        {
            _dataLuminosity->bortle = 2.5;
        }
        else if (_dataLuminosity->sqm > 21.79)
        {
            _dataLuminosity->bortle = 3;
        }
        else if (_dataLuminosity->sqm > 21.69)
        {
            _dataLuminosity->bortle = 3.5;
        }
        else if (_dataLuminosity->sqm > 21.25)
        {
            _dataLuminosity->bortle = 4;
        }
        else if (_dataLuminosity->sqm > 20.49)
        {
            _dataLuminosity->bortle = 4.5;
        }
        else if (_dataLuminosity->sqm > 20)
        {
            _dataLuminosity->bortle = 5;
        }
        else if (_dataLuminosity->sqm > 19.5)
        {
            _dataLuminosity->bortle = 5.5;
        }
        else if (_dataLuminosity->sqm > 19.22)
        {
            _dataLuminosity->bortle = 6;
        }
        else if (_dataLuminosity->sqm > 18.94)
        {
            _dataLuminosity->bortle = 6.5;
        }
        else if (_dataLuminosity->sqm > 18.66)
        {
            _dataLuminosity->bortle = 7;
        }
        else if (_dataLuminosity->sqm > 18.38)
        {
            _dataLuminosity->bortle = 7.5;
        }
        else if (_dataLuminosity->sqm > 18.16)
        {
            _dataLuminosity->bortle = 8;
        }
        else if (_dataLuminosity->sqm > 17.94)
        {
            _dataLuminosity->bortle = 8.5;
        }
        else
        {
            _dataLuminosity->bortle = 9;
        }
    }
    else
    {
        _dataLuminosity->bortle = NAN;
    }
}

/// <summary>
/// Renvoi la valeur de gain en float
/// </summary>
/// <returns></returns>
float ROMV2_TSL2591::GetGainValue()
{
    switch (_gain)
    {
    case TSL2591_GAIN_LOW:
        return 1;

    case TSL2591_GAIN_MED:
        return 25;

    case TSL2591_GAIN_HIGH:
        return 428;

    case TSL2591_GAIN_MAX:
        return 9876;

    default:
        return NAN;
    }
}

/// <summary>
/// Renvoi la valeur de Timing en float
/// </summary>
/// <returns></returns>
float ROMV2_TSL2591::GetTimingValue()
{
    switch (_timing)
    {
    case TSL2591_INTEGRATIONTIME_100MS:
        return 0.1;

    case TSL2591_INTEGRATIONTIME_200MS:
        return 0.2;

    case TSL2591_INTEGRATIONTIME_300MS:
        return 0.3;

    case TSL2591_INTEGRATIONTIME_400MS:
        return 0.4;

    case TSL2591_INTEGRATIONTIME_500MS:
        return 0.5;

    case TSL2591_INTEGRATIONTIME_600MS:
        return 0.6;

    default:
        return NAN;
    }
}

/// <summary>
/// Calcul du SR (Solid Angle)
/// </summary>
/// <param name="totalAngle_deg"></param>
/// <returns></returns>
float ROMV2_TSL2591::ComputeSolidAngle_sr(float totalAngle_deg)
{
    float halfAngle_rad = (totalAngle_deg / 2.0f) * (PI / 180.0f);
    return 2.0f * PI * (1.0f - cos(halfAngle_rad));
}

/// <summary>
/// Conversion de Lux vers Mag/Arcsec²
/// </summary>
/// <param name="lux">Valeur mesurée en Lux</param>
/// <param name="ch0">Valeur de luminosité dans l'IR</param>
/// <param name="raw_ch1">Valeur de luminosité totale</param>
/// <returns></returns>
LuxToMagConversionResult ROMV2_TSL2591::LuxToMagnitude(float lux)
{
    LuxToMagConversionResult result;
    result.lux = lux;

    // Sécurité : saturation capteur ou valeur nulle
    if (isnan(lux) || isinf(lux) || lux <= MIN_LUX_THRESHOLD) {
        result.valid = false;
        result.magnitude_arcsec2 = 0.0f;
        return result;
    }

    // Calcul de la magnitude à partir du lux
    result.valid = true;
    result.magnitude_arcsec2 = -2.5 * log10(lux / 108000.0) + TSL2591_CALIBRATION;
    if (result.magnitude_arcsec2 < 0)
    {
        result.magnitude_arcsec2 = 0;
    }
    return result;
}

/// <summary>
/// Vide la queue permettant de calculer la moyenne mobile sur le SQM
/// </summary>
void ROMV2_TSL2591::ClearLuxAverage()
{
    _luxAverageQueue.Clear();
    _dataLuminosity->luxAverage = NAN;
    _dataLuminosity->luxAverageCount = 0;
    _dataLuminosity->sqm = NAN;
    _dataLuminosity->bortle = NAN;
}
