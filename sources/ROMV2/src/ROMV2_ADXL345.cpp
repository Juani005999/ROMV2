#include <ROMV2_ADXL345.h>

/// ----------------------------
/// Constructeur
///
ROMV2_ADXL345::ROMV2_ADXL345()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="dataAcceleration"></param>
void ROMV2_ADXL345::Init(DataSensorAcceleration* dataAcceleration)
{
    // Valorisation des champs internes
    _dataAcceleration = dataAcceleration;

    // Initialisation ADXL345
    if (!_adxl345.begin()) {
        debugln(F("Could not find a valid ADXL345 sensor, check wiring!"));
        while (1);
    }
    DisplaySensorDetails();
    DisplayDataRate();
    DisplayRange();

    // Initialisation des chronos
    _chronoReadAccel = 0;
}

/// <summary>
/// Affichage des détails du sensor MLX90614
/// </summary>
void ROMV2_ADXL345::DisplaySensorDetails()
{
    sensor_t sensor;
    _adxl345.getSensor(&sensor);

    debugln(F(""));
    debug(F("Sensor:       ")); debugln(sensor.name);
    debug(F("Driver Ver:   ")); debugln(sensor.version);
    debug(F("Unique ID:    ")); debugln(sensor.sensor_id);
    debug(F("Max Value:    ")); debug(sensor.max_value); debugln(F(" m/s^2"));
    debug(F("Min Value:    ")); debug(sensor.min_value); debugln(F(" m/s^2"));
    debug(F("Resolution:   ")); debug(sensor.resolution); debugln(F(" m/s^2"));  
}

/// <summary>
/// Affichage des détails du sensor MLX90614
/// </summary>
void ROMV2_ADXL345::DisplayDataRate()
{
    debugln(F(""));
    debugln(F("ADXL345 DataRate"));

    switch(_adxl345.getDataRate())
    {
        case ADXL345_DATARATE_3200_HZ:
            debug(F("3200 ")); 
            break;
        case ADXL345_DATARATE_1600_HZ:
            debug(F("1600 ")); 
            break;
        case ADXL345_DATARATE_800_HZ:
            debug(F("800 ")); 
            break;
        case ADXL345_DATARATE_400_HZ:
            debug(F("400 ")); 
            break;
        case ADXL345_DATARATE_200_HZ:
            debug(F("200 ")); 
            break;
        case ADXL345_DATARATE_100_HZ:
            debug(F("100 ")); 
            break;
        case ADXL345_DATARATE_50_HZ:
            debug(F("50 ")); 
            break;
        case ADXL345_DATARATE_25_HZ:
            debug(F("25 ")); 
            break;
        case ADXL345_DATARATE_12_5_HZ:
            debug(F("12.5 ")); 
            break;
        case ADXL345_DATARATE_6_25HZ:
            debug(F("6.25 ")); 
            break;
        case ADXL345_DATARATE_3_13_HZ:
            debug(F("3.13 ")); 
            break;
        case ADXL345_DATARATE_1_56_HZ:
            debug(F("1.56 ")); 
            break;
        case ADXL345_DATARATE_0_78_HZ:
            debug(F("0.78 ")); 
            break;
        case ADXL345_DATARATE_0_39_HZ:
            debug(F("0.39 ")); 
            break;
        case ADXL345_DATARATE_0_20_HZ:
            debug(F("0.20 ")); 
            break;
        case ADXL345_DATARATE_0_10_HZ:
            debug(F("0.10 ")); 
            break;
        default:
            debug (F("???? ")); 
            break;
    }  
    debugln(F(" Hz"));  
}

/// <summary>
/// Affichage des détails du sensor ADXL345
/// </summary>
void ROMV2_ADXL345::DisplayRange()
{
    debugln(F(""));
    debugln(F("ADXL345 Range"));

    switch(_adxl345.getRange())
    {
        case ADXL345_RANGE_16_G:
            debug(F("16 ")); 
            break;
        case ADXL345_RANGE_8_G:
            debug(F("8 ")); 
            break;
        case ADXL345_RANGE_4_G:
            debug(F("4 ")); 
            break;
        case ADXL345_RANGE_2_G:
            debug(F("2 ")); 
            break;
        default:
            debug(F("?? ")); 
            break;
    }  
    debugln(F(" g"));
}

/// <summary>
/// Lecture de l'accélération
/// </summary>
void ROMV2_ADXL345::ReadAcceleration()
{
    // Lecture de luminosité sur intervalle
    if (millis() > _chronoReadAccel + READ_ACCEL_INTERVAL)
    {
        // Récupère un event sur l'objet ADXL345 
        sensors_event_t event; 
        _adxl345.getEvent(&event);
  
        // On lit les accélérations (!! ATTENTION !! Dans le boitier, les X et Y sont inversés)
        _dataAcceleration->x = event.acceleration.y * -1.0;
        _dataAcceleration->y = event.acceleration.x * -1.0;
        _dataAcceleration->z = event.acceleration.z;

        // Trace
        debugln(F(""));
        debug(F("[ACCEL] X: ")); debug(_dataAcceleration->x); debugln(F(" m/s^2 "));
        debug(F("[ACCEL] Y: ")); debug(_dataAcceleration->y); debugln(F(" m/s^2 "));
        debug(F("[ACCEL] Z: ")); debug(_dataAcceleration->z); debugln(F(" m/s^2 "));

        // Actualisation flag et compteurs
        _chronoReadAccel = millis();
    }
}
