// Include des librairies
#include <ROMV2_APP_CONFIG.h>
#include <ROMV2_TFT.h>

/// <summary>
/// Objet ROMV2_TSL2591 : Objet applicatif permettant le pilotage du capteur TSL2591
/// </summary>
class ROMV2_TSL2591
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_TSL2591();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataLuminosity"></param>
    /// <param name="dataEnvironment"></param>
    void Init(ROMV2_TFT* tft, DataSensorLuminosity* dataLuminosity, DataSensorEnvironment* dataEnvironment);

    /// <summary>
    /// Lecture de la luminosité
    /// </summary>
    void ReadLuminosity();

    /// <summary>
    /// Vide la queue permettant de calculer la moyenne mobile sur le Lux
    /// </summary>
    void ClearLuxAverage();

    /// <summary>
    /// Démarre la tâche FreeRTOS de lecture TSL2591 sur le cœur 1
    /// </summary>
    void StartTask();

    /// <summary>
    /// Sauvegarde de la calibration du TSL2591
    /// </summary>
    /// <param name="value"></param>
    void SaveTSL2591Calibration(int value);

    /// <summary>
    /// Chargement de la calibration du TSL2591
    /// </summary>
    /// <returns></returns>
    void LoadTSL2591Calibration();

  private:
    // Fonctions
    void DisplaySensorDetails();
    void ConfigureSensor(tsl2591Gain_t gain = TSL2591_GAIN_MED, tsl2591IntegrationTime_t timing = TSL2591_INTEGRATIONTIME_300MS);
    void CheckSensorValues();
    void DecreaseSensor();
    void IncreaseSensor();
    void DecreaseGain();
    void IncreaseGain();
    void DecreaseTiming();
    void IncreaseTiming();
    void UpdateSQM();
    void UpdateBortle();
    float GetGainValue();
    float GetTimingValue();
    float ComputeSolidAngle_sr(float totalAngle_deg);
    LuxToMagConversionResult LuxToMagnitude(float lux, int tsl2591Calibration);
    static void TaskWrapper(void* pvParameters);
    void TaskLoop();
    float CorrectThermal(float lux);

    // Instanciation des objets internes
    Adafruit_TSL2591 _tsl2591 = Adafruit_TSL2591(TSL2591_NUMBER_ID);      // TSL2591 : Capteur de luminosité
    ROMV2_TFT* _tft;
    Preferences _preferences;

    // Membres internes
    DataSensorLuminosity* _dataLuminosity;
    DataSensorEnvironment* _dataEnvironment;
    JUANITO_MOVING_AVERAGE<float, TSL2591_MOVING_AVERAGE_COUNT> _luxAverageQueue;
    tsl2591Gain_t _gain = TSL2591_GAIN_MED;
    tsl2591IntegrationTime_t _timing = TSL2591_INTEGRATIONTIME_600MS;
    bool _lastDecreaseGain = false;
    bool _lastIncreaseGain = false;
    SemaphoreHandle_t _mutex;
    bool _newDataReady = false;
    bool _updatedData = false;
    float _lastIrRead = NAN;
    float _lastFullRead = NAN;
    float _lastVisibleRead = NAN;
    float _lastLuxRead = NAN;
    float _localIr = NAN;
    float _localFull = NAN;
    float _localVisible = NAN;
    float _localLux = NAN;
    volatile bool _luminosityIconOn = false;

    // Chronos
    unsigned long _chronoDisplayLux;                                                  // Chrono pour lecture de la luminosité
};