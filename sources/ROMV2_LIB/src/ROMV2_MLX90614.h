// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_MLX90614 : Objet applicatif permettant le pilotage du capteur MLX90614
/// </summary>
class ROMV2_MLX90614
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_MLX90614();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="dataSkyState"></param>
    void Init(DataSensorSkyState* dataSkyState);

    /// <summary>
    /// Lecture de la Température IR
    /// </summary>
    void ReadIRTemp();

  private:
    // Fonctions
    void DisplaySensorDetails();
    void UpdateSkyState();

    // Instanciation des objets internes
    Adafruit_MLX90614 _mlx90614 = Adafruit_MLX90614();                    // MLX90614 : Capteur de température IR

    // Membres internes
    DataSensorSkyState* _dataSkyState;

    // Chronos
    long _chronoReadIRTemp;                                               // Chrono pour lecture de la T° IR
};