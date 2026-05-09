// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_ADXL345 : Objet applicatif permettant le pilotage du capteur ADXL345
/// </summary>
class ROMV2_ADXL345
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_ADXL345();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="dataAcceleration"></param>
    void Init(DataSensorAcceleration* dataAcceleration);

    /// <summary>
    /// Lecture de l'accélération
    /// </summary>
    void ReadAcceleration();

  private:
    // Fonctions
    void DisplaySensorDetails();
    void DisplayDataRate();
    void DisplayRange();

    // Instanciation des objets internes
    Adafruit_ADXL345_Unified _adxl345 = Adafruit_ADXL345_Unified(ADXL345_NUMBER_ID);    // ADXL345 : Capteur accéléromètre

    // Membres internes
    DataSensorAcceleration* _dataAcceleration;

    // Chronos
    long _chronoReadAccel;                                                              // Chrono pour lecture de l'accélération'
};