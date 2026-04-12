#include <ROMV2_MLX90614.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_MLX90614::ROMV2_MLX90614()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="dataSkyState"></param>
void ROMV2_MLX90614::Init(DataSensorSkyState* dataSkyState)
{
    // Valorisation des champs internes
    _dataSkyState = dataSkyState;

    // Initialisation MLX90614
    if (!_mlx90614.begin()) {
        debugln(F("Could not find a valid MLX90614 sensor, check wiring!"));
        while (1);
    };
    DisplaySensorDetails();

    // Initialisation des chronos
    _chronoReadIRTemp = 0;
}

/// <summary>
/// Affichage des détails du sensor MLX90614
/// </summary>
void ROMV2_MLX90614::DisplaySensorDetails()
{
    debugln(F(""));
    debugln(F("Sensor: MLX90614"));
    debug(F("Emissivity = "));
    debugln(_mlx90614.readEmissivity());
}

/// <summary>
/// Lecture de la Température IR
/// </summary>
void ROMV2_MLX90614::ReadIRTemp()
{
    // Lecture de luminosité sur intervalle
    if (millis() > _chronoReadIRTemp + READ_IRTEMP_INTERVAL)
    {
        _dataSkyState->tempAmbient = _mlx90614.readAmbientTempC();
        _dataSkyState->tempObject = _mlx90614.readObjectTempC();
        UpdateSkyState();

        debugln(F(""));
        debug("[IRTEMP] Ambient = ");
        debug(_dataSkyState->tempAmbient);
        debug("*C\tObject = ");
        debug(_dataSkyState->tempObject);
        debugln("*C");
        debug("[IRTEMP] Ambient = ");
        debug(_mlx90614.readAmbientTempF());
        debug("*F\tObject = ");
        debug(_mlx90614.readObjectTempF());
        debugln("*F");
        debug("[IRTEMP] SkyState = ");
        debugln(_dataSkyState->skyState);
        debug("[IRTEMP] Cloud Cover = ");
        debugln(_dataSkyState->cloudCover);

        // Actualisation flag et compteurs
        _chronoReadIRTemp = millis();
    }
}

/// <summary>
/// Actualisation de l'état du ciel
/// </summary>
void ROMV2_MLX90614::UpdateSkyState()
{
    if (!isnan(_dataSkyState->tempAmbient) && !isnan(_dataSkyState->tempObject))
    {
        // On récupère la différence entre les 2 valeurs mesurées
        float tempDiff = _dataSkyState->tempAmbient - _dataSkyState->tempObject;

        // On positionne l'état du ciel en fonction de la différence des valeurs mesurées
        if (tempDiff > SKY_STATE_POINT_HIGH)
        {
            _dataSkyState->skyState = SKY_STATE_CLEAR;
            _dataSkyState->cloudCover = 0;
        }
        else if ((tempDiff > SKY_STATE_POINT_LOW) && (tempDiff < SKY_STATE_POINT_HIGH))
        {
            _dataSkyState->skyState = SKY_STATE_PARTLY_CLOUDY;
            _dataSkyState->cloudCover = 50;
        }
        else if (tempDiff < SKY_STATE_POINT_LOW)
        {
            _dataSkyState->skyState = SKY_STATE_CLOUDY;
            _dataSkyState->cloudCover = 100;
        }
        else
        {
            _dataSkyState->skyState = SKY_STATE_UNKNOWN;
            _dataSkyState->cloudCover = 100;
        }
    }
    else
    {
        _dataSkyState->skyState = SKY_STATE_UNKNOWN;
        _dataSkyState->cloudCover = 100;
    }
}
