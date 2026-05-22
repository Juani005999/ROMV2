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
/// <param name="dataEnvironment"></param>
void ROMV2_TSL2591::Init(ROMV2_TFT* tft, DataSensorLuminosity* dataLuminosity, DataSensorEnvironment* dataEnvironment)
{
    // Valorisation des membres internes
    _tft = tft;
    _dataLuminosity = dataLuminosity;
    _dataEnvironment = dataEnvironment;

    // Création du mutex FreeRTOS
    _mutex = xSemaphoreCreateMutex();

    // Initialisation TSL2591
    if (!_tsl2591.begin()) {
        debugln(F("Could not find a valid TSL2591 sensor, check wiring!"));
        while (1);
    }

    // Configuration du capteur
    DisplaySensorDetails();
    ConfigureSensor();

    // Initialisation des chronos
    _chronoDisplayLux = 0;
}

/// <summary>
/// Démarre la tâche FreeRTOS sur le cœur 1
/// </summary>
void ROMV2_TSL2591::StartTask()
{
    xTaskCreatePinnedToCore(
        TaskWrapper,    // Fonction statique wrapper
        "TSL2591",      // Nom de la tâche
        4096,           // Stack en bytes (augmenter à 8192 si stack overflow)
        this,           // Passage du pointeur this pour accéder aux membres
        1,              // Priorité
        NULL,           // Handle (non utilisé)
        1               // Cœur 1 → laisse le cœur 0 libre pour l'UI
    );

    // Traces
    debugln(F(""));
    debugln(F("[LUX] ROMV2_TSL2591 TaskLoop Started"));
}

/// <summary>
/// Wrapper statique requis par FreeRTOS — redirige vers TaskLoop()
/// </summary>
void ROMV2_TSL2591::TaskWrapper(void* pvParameters)
{
    // pvParameters contient le pointeur this passé dans StartTask()
    static_cast<ROMV2_TSL2591*>(pvParameters)->TaskLoop();
}

/// <summary>
/// Boucle de la tâche FreeRTOS — tourne en continu sur le cœur 1
/// </summary>
void ROMV2_TSL2591::TaskLoop()
{
    while (true)
    {
        // Lecture des données brutes du capteur
        _luminosityIconOn = true;
        uint32_t lum = _tsl2591.getFullLuminosity();
        uint16_t ir = lum >> 16;
        uint16_t full = lum & 0xFFFF;
        uint16_t visible = full - ir;
        float    lux = _tsl2591.calculateLux(full, ir);

        // Ecriture des données sous mutex
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            _lastIrRead = ir;
            _lastFullRead = full;
            _lastVisibleRead = visible;
            _lastLuxRead = lux;
            _newDataReady = true;

            // On libère le mutex
            xSemaphoreGive(_mutex);
        }

        // On repositionne l'icone de lecture d'état
        _luminosityIconOn = false;

        // Traces
        debugln(F(""));
        debugln(F("Lecture du TSL2591 dans TaskLoop"));
        debug(F("[LUX] IR: "));
        debugln(ir);
        debug(F("[LUX] Full: "));
        debugln(full);
        debug(F("[LUX] Visible: "));
        debugln(visible);
        debug(F("[LUX] Lux: "));
        debugln(lux);

        // Libère le CPU pendant le temps d'intégration
        // => le cœur 0 est entièrement libre pendant cette attente
        vTaskDelay(pdMS_TO_TICKS(READ_LUX_INTERVAL));
    }
}

/// <summary>
/// Lecture de la Luminosité
/// </summary>
void ROMV2_TSL2591::ReadLuminosity()
{
    // Lecture de luminosité sur intervalle
    if (millis() - _chronoDisplayLux >= DISPLAY_INTERVAL)
    {
        // Gestion icône — exécuté sur cœur 0, donc TFT safe
        _tft->SetLuminosityIcon(_luminosityIconOn);

        // On récupère dans le Thread principal les valeurs lues dans le Thread 1
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            if (_newDataReady)
            {
                // Mise à jour des données sous mutex
                _localIr = _lastIrRead;
                _localFull = _lastFullRead;
                _localVisible = _lastVisibleRead;
                _localLux = _lastLuxRead;
                _newDataReady = false;
                _updatedData = true;
            }

            // On libère le mutex
            xSemaphoreGive(_mutex);
        }

        if (_updatedData)
        {
            // Mise à jour des données globale
            _dataLuminosity->ir = _localIr;
            _dataLuminosity->full = _localFull;
            _dataLuminosity->visible = _localVisible;
            _dataLuminosity->lux = _localLux;

            // Correction thermique du lux mesuré
            _dataLuminosity->luxThermalCorrected = CorrectThermal(_dataLuminosity->lux);

            // Ajout dans la moyenne mobile si valeurs valides
            if (!isnan(_dataLuminosity->luxThermalCorrected) && !isinf(_dataLuminosity->luxThermalCorrected)
                && _dataLuminosity->luxThermalCorrected > MIN_LUX_THRESHOLD
                && _dataLuminosity->full != 0xFFFF && _dataLuminosity->ir != 0xFFFF)
            {
                // On ajoute à la queue et on récupère la moyenne et la taille
                _luxAverageQueue.Push(_dataLuminosity->luxThermalCorrected);
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
            debug(F("[LUX] Lux Thermal Corrected: "));
            debugln(_dataLuminosity->luxThermalCorrected);
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
        }

        // Actualisation flag et compteurs
        _updatedData = false;
        _chronoDisplayLux = millis();
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

    // Gain
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

    // Timing
    sprintf(_dataLuminosity->luxSensorTiming, "%d ms", (int)(_timing + 1) * 100);

    // Trace Gain
    debugln(F(""));
    debugln(F("[LUX] Sensor TSL2591 : Configure"));
    debug(F("[LUX] Gain:         "));
    debugln(_dataLuminosity->luxSensorGain);

    // Trace Timing
    debug(F("[LUX] Timing:       "));
    debugln(_dataLuminosity->luxSensorTiming);
}

/// <summary>
/// Vérifie les valeurs du sensor TSL2591 pour un ajustement si nécessaire
/// </summary>
void ROMV2_TSL2591::CheckSensorValues()
{
    if (!isnan(_dataLuminosity->full)
        && _dataLuminosity->full > TSL2591_SENSOR_VALUE_HIGH
        && (_gain != TSL2591_GAIN_LOW || _timing != TSL2591_INTEGRATIONTIME_100MS))
    {
        DecreaseSensor();
    }
    else if (!isnan(_dataLuminosity->full)
        && _dataLuminosity->full < TSL2591_SENSOR_VALUE_LOW
        && (_gain != TSL2591_GAIN_MAX || _timing != TSL2591_INTEGRATIONTIME_600MS))
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
    if (_dataLuminosity->luxAverageCount > 0 && !isnan(_dataLuminosity->luxAverage))
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

    // Lecture sous mutex des données partagés entre les coeurs
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _newDataReady = false;
        _updatedData = false;
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Applique la correction de dérive thermique sur la valeur lux
/// Correction linéaire : lux_corrigé = lux_brut × (1 + TEMP_COEFF × (T - TEMP_REF_C))
/// </summary>
/// <param name="lux">Lux brut mesuré</param>
/// <returns>Lux corrigé</returns>
float ROMV2_TSL2591::CorrectThermal(float lux)
{
    // Vérification que la température est disponible
    if (isnan(_dataEnvironment->temperature))
    {
        return lux;
    }

    // Calcul du facteur de correction
    float correction = 1.0f + TSL2591_CORRECTION_TEMP_COEFF * (_dataEnvironment->temperature - TSL2591_CORRECTION_TEMP_REF_C);

    // Clamp : ne pas laisser la correction devenir négative ou absurde
    correction = constrain(correction, 0.5f, 2.0f);

    // Trace
    debugln(F(""));
    debug(F("[LUX] Temp BME280: "));
    debugln(_dataEnvironment->temperature);
    debug(F("[LUX] Temp correction factor: "));
    debugln(correction);

    return lux * correction;
}