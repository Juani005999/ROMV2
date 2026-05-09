// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_BME280 : Objet applicatif permettant le pilotage du capteur BME280
/// </summary>
class ROMV2_BME280
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_BME280();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="dataEnvironment"></param>
    void Init(DataSensorEnvironment* dataEnvironment);

    /// <summary>
    /// Lecture des conditions d'environnement sur le BME280
    /// </summary>
    void ReadEnvironment();

  private:
    // Fonctions
      void UpdateDewPoint();
      void UpdateDiewPointState();

    // Instanciation des objets internes
    Adafruit_BME280 _bme280;                                                // BME 280 : Capteur T°, Humidité, Pression Atmosphérique

    // Membres internes
    DataSensorEnvironment* _dataEnvironment;

    // Chronos
    long _chronoReadEnvironment;                                            // Chrono pour lecture des conditions d'environnements
};