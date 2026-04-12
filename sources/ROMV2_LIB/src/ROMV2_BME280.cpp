#include <ROMV2_BME280.h>

/// ----------------------------
/// Constructeur
///
ROMV2_BME280::ROMV2_BME280()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="dataEnvironment"></param>
void ROMV2_BME280::Init(DataSensorEnvironment* dataEnvironment)
{
    // Valorisation des champs internes
    _dataEnvironment = dataEnvironment;
    
    // Initialisation BME280
    if (!_bme280.begin(BME280_I2C_ADDRESS, &Wire)) {
        debugln(F("Could not find a valid BME280 sensor, check wiring!"));
        while (1);
    }

    // Initialisation des chronos
    _chronoReadEnvironment = 0;
}

/// <summary>
/// Lecture de la Température / Humidité / Pression
/// </summary>
void ROMV2_BME280::ReadEnvironment()
{
    // Lecture de l'environnement sur intervalle
    if (millis() > _chronoReadEnvironment + READ_ENVIRONMENT_INTERVAL)
    {
        _dataEnvironment->temperature = _bme280.readTemperature();
        _dataEnvironment->pression = _bme280.readPressure() / 100;
        _dataEnvironment->humidite = _bme280.readHumidity();
        UpdateDewPoint();
        UpdateDiewPointState();

        // Trace
        debugln(F(""));
        debug(F("[ENV] Température interne : "));
        debug(_dataEnvironment->temperature);
        debugln(F(" °C"));
        debug(F("[ENV] Humidité : "));
        debug(_dataEnvironment->humidite);
        debugln(F(" %"));
        debug(F("[ENV] Pression atm. : "));
        debug(_dataEnvironment->pression);
        debugln(F(" Ha"));
        debug(F("[ENV] Point de rosée : "));
        debug(_dataEnvironment->dewPoint);
        debugln(F(""));

        // Actualisation flag et compteurs
        _chronoReadEnvironment = millis();
    }
}

/// <summary>
/// Mise à jour du point de rosée à partir de T° et de H
/// </summary>
void ROMV2_BME280::UpdateDewPoint()
{
    double a = 17.271;
    double b = 237.7;
    if (!isnan(_dataEnvironment->temperature) && !isnan(_dataEnvironment->humidite))
    {
        double temp = (a * _dataEnvironment->temperature) / (b + _dataEnvironment->temperature) + log(_dataEnvironment->humidite * 0.01);
        _dataEnvironment->dewPoint = (b * temp) / (a - temp);
    }
    else
        _dataEnvironment->dewPoint = NAN;
}

/// <summary>
/// MAJ de l'état du DewPoint
/// </summary>
void ROMV2_BME280::UpdateDiewPointState()
{
    // MAJ de l'état du DewPoint
    if (_dataEnvironment->temperature <= _dataEnvironment->dewPoint + 1)
    {
        _dataEnvironment->dewPointState = DEWPOINT_STATE_WET;
    }
    else if (_dataEnvironment->temperature <= _dataEnvironment->dewPoint + 5)
    {
        _dataEnvironment->dewPointState = DEWPOINT_STATE_HUMID;
    }
    else
    {
        _dataEnvironment->dewPointState = DEWPOINT_STATE_DRY;
    }
}
