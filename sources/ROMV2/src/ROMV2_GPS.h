// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_GPS : Objet applicatif permettant le pilotage du capteur GPS
/// </summary>
class ROMV2_GPS
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_GPS();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="dataGPS"></param>
    void Init(DataSensorGPS* dataGPS);

    /// <summary>
    /// Lecture de commande sur port série du GPS
    /// </summary>
    void ReadGPS();

    /// <summary>
    /// Vérification de la dernière lecture de données du GPS pour vérification de l'état
    /// </summary>
    void CheckGPSState();

  private:
    // Fonctions
    void GpsInit();
    void ReadNMEALine();
    boolean ParseNMEALine(char *nmea);
    boolean ValidNMEAChecksum(char* nmea);
    char* NMEAParseDate(char* nmea);
    char* NMEAParseTime(char* nmea);
    char* NMEAParseLatitude(char* nmea);
    char* NMEAParseLongitude(char* nmea);
    char* NMEAParseAltitude(char* nmea);
    char* NMEAParseFix(char* nmea, bool letterValue = false);
    char* NMEAParseSatellites(char* nmea);
    char* NMEAParseUnknown(char* nmea);
    uint8_t GpsParseHex(char c);
    void GpsSendCommand(const char *str);
    char* CoordinateToString(float coordinate);
    void ResetGPSData();

    // Instanciation des objets internes

    // Membres internes
    DataSensorGPS* _dataGPS;
    int _gpsIndex = 0;
    boolean _gpsNMEAFlag = false;
    char stringBufferCoordinate[64];
    volatile char line1[NMEA_CMD_LINE_LENGTH];
    volatile char line2[NMEA_CMD_LINE_LENGTH];
    volatile char *currentline;
    volatile char *lastline;

    // Chronos
    long _chronoLastGPSState;                                               // Chrono pour lecture de l'état du GPS
};