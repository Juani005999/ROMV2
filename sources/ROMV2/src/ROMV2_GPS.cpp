#include <ROMV2_GPS.h>

// Création du port série pour le GPS
HardwareSerial _gpsSerial(2);

/// <summary>
/// Constructeur
/// </summary>
ROMV2_GPS::ROMV2_GPS()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="dataGPS"></param>
void ROMV2_GPS::Init(DataSensorGPS* dataGPS)
{
    // Valorisation des champs internes
    _dataGPS = dataGPS;

    // Initialisation GPS
    currentline = line1;
    lastline    = line2;
    GpsInit();
    delay(100);
    //GpsSendCommand(PMTK_SET_BAUD_57600);
    GpsSendCommand("$PMTK314,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
    GpsSendCommand(PMTK_SET_NMEA_UPDATE_5S);
}

/// <summary>
/// Initialisation du GPS
/// </summary>
void ROMV2_GPS::GpsInit()
{
    _gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
    delay(100);
}

/// <summary>
/// Lecture sur le port série du GPS
/// </summary>
void ROMV2_GPS::ReadNMEALine()
{
    char c;

    if (_gpsSerial.available())
    {
        // Lecture d'un nouveau caractère sur port série
        c = _gpsSerial.read();

        // Si fin de ligne reçue, on traite la ligne et on met à jour les flags
        if (c == '\n')
        {
            currentline[_gpsIndex] = 0;
            _gpsIndex = 0;
            _gpsNMEAFlag = true;
            if (currentline == line1)
            {
                currentline = line2;
                lastline = line1;
            }
            else
            {
                currentline = line1;
                lastline = line2;
            }
        }
        // Sinon, on ajoute le caractère à la ligne courante
        else
            currentline[_gpsIndex++] = c;

        // Vérif sur taille max
        if (_gpsIndex >= NMEA_CMD_LINE_LENGTH)
        {
            _gpsIndex = NMEA_CMD_LINE_LENGTH - 1;
        }
    }
}

/// <summary>
/// Parse d'une commande NMEA
/// </summary>
/// <param name="nmea"></param>
/// <returns></returns>
boolean ROMV2_GPS::ParseNMEALine(char* nmea)
{
    // Trace de la commande NMEA à parser
    debugln("");
    debug(F("[GPS] Receive NEMA: "));
    debugln(nmea);

    // Vérification du checksum de la réponse
    if (!ValidNMEAChecksum(nmea))
    {
        return false;
    }

    // Réponse de type $GxGGA
    if (strstr(nmea, "GGA") != NULL)
    {
        // Création d'un pointeur permettant le parcours de la string
        char* pCurrentNMEA = nmea;
        
        // Parse de l'heure
        pCurrentNMEA = NMEAParseTime(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse de la Latitude
        pCurrentNMEA = NMEAParseLatitude(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse de la Longitude
        pCurrentNMEA = NMEAParseLongitude(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse du Fix
        pCurrentNMEA = NMEAParseFix(pCurrentNMEA, false);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse du nombre de satellites
        pCurrentNMEA = NMEAParseSatellites(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse d'un champ non utilisé (HOP)
        pCurrentNMEA = NMEAParseUnknown(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse de l'altitude
        pCurrentNMEA = NMEAParseAltitude(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Retour
        return true;
    }

    // Réponse de type $GxRMC
    else if (strstr(nmea, "RMC") != NULL)
    {
        // Création d'un pointeur permettant le parcours de la string
        char* pCurrentNMEA = nmea;

        // Parse de l'heure
        pCurrentNMEA = NMEAParseTime(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse du Fix
        pCurrentNMEA = NMEAParseFix(pCurrentNMEA, true);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse de la Latitude
        pCurrentNMEA = NMEAParseLatitude(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse de la Longitude
        pCurrentNMEA = NMEAParseLongitude(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse d'un champ non utilisé (Speed)
        pCurrentNMEA = NMEAParseUnknown(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse d'un champ non utilisé (Angle)
        pCurrentNMEA = NMEAParseUnknown(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse de la Date
        pCurrentNMEA = NMEAParseDate(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Retour
        return true;
    }

    // Réponse de type $GxGLL
    else if (strstr(nmea, "GLL") != NULL)
    {
        // Création d'un pointeur permettant le parcours de la string
        char* pCurrentNMEA = nmea;

        // Parse de la Latitude
        pCurrentNMEA = NMEAParseLatitude(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Parse de la Longitude
        pCurrentNMEA = NMEAParseLongitude(pCurrentNMEA);
        if (pCurrentNMEA == NULL)
        {
            return false;
        }

        // Retour
        return true;
    }

    // Retour
    return false;
}

/// <summary>
/// Vérification par checksum de la réponse reçue du GPS
/// </summary>
/// <param name="nmea"></param>
/// <returns></returns>
boolean ROMV2_GPS::ValidNMEAChecksum(char* nmea)
{
    // On fait un checksum sur la réponse afin de vérifier sa validité
    if (strlen(nmea) > 4 && nmea[strlen(nmea) - 4] == '*')
    {
        // On lit la taille remontée dans la réponse
        uint16_t sum = GpsParseHex(nmea[strlen(nmea) - 3]) * 16;
        sum += GpsParseHex(nmea[strlen(nmea) - 2]);

        // On vérifie avec la réponse
        for (uint8_t i = 1; i < (strlen(nmea) - 4); i++)
        {
            sum ^= nmea[i];
        }
        if (sum != 0)
        {
            // Le checksum ne correspond pas, on s'arrête là
            debugln(F("[GPS] Bad checksum"));
            return false;
        }
    }
    // Mauvais format de réponse, on s'arrête là
    else
    {
        debugln(F("[GPS] Bad checksum"));
        return false;
    }

    // Le checksum est valide
    return true;
}

/// <summary>
/// Parse de la date
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseDate(char* nmea)
{
    uint8_t dyear, dmonth, dday;

    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Lecture et Parse de la date
    if (*pCurrentNMEA != ',')
    {
        uint32_t fulldate = atof(pCurrentNMEA);
        dday = fulldate / 10000;
        dmonth = (fulldate % 10000) / 100;
        dyear = (fulldate % 100);
        sprintf(_dataGPS->gpsDate, "%02d/%02d/20%02d", dday, dmonth, dyear);
    }
    else
    {
        strcpy(_dataGPS->gpsDate, "00/00/1900");
    }

    return pCurrentNMEA;
}

/// <summary>
/// Parse de l'heure
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseTime(char* nmea)
{
    uint8_t thours, tminutes, tseconds;

    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Parse Heures, Minutes et Secondes de la string
    float timef = atof(pCurrentNMEA);
    uint32_t gtime = timef;
    thours = gtime / 10000;
    tminutes = (gtime % 10000) / 100;
    tseconds = (gtime % 100);

    // Formattage de l'heure
    sprintf(_dataGPS->gpsTime, "%02d:%02d:%02d", thours, tminutes, tseconds);

    return pCurrentNMEA;
}

/// <summary>
/// Parse de la latitude
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseLatitude(char* nmea)
{
    float   latitude = 0;
    int32_t degree;
    float   minutes;
    char    degreebuff[10];

    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Lecture et Parse de la latitude
    if (*pCurrentNMEA != ',')
    {
        // On lit la partie entière
        strncpy(degreebuff, pCurrentNMEA, 2);
        pCurrentNMEA += 2;
        degreebuff[2] = '\0';
        degree = atol(degreebuff);

        // On lit la partie décimale
        strncpy(degreebuff, pCurrentNMEA, 2);
        degreebuff[2] = '.';
        pCurrentNMEA += 3;
        strncpy(degreebuff + 3, pCurrentNMEA, 4);
        degreebuff[7] = '\0';
        minutes = atof(degreebuff) / 60;

        // On ajoute les valeurs lues
        latitude = degree + minutes;
    }

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Formatage de la Latitude en fonction de l'orientation
    if (*pCurrentNMEA != ',')
    {
        if (pCurrentNMEA[0] == 'S')
        {
            sprintf(_dataGPS->gpsLatitude, "%s S", CoordinateToString(latitude));
        }
        else if (pCurrentNMEA[0] == 'N')
        {
            sprintf(_dataGPS->gpsLatitude, "%s N", CoordinateToString(latitude));
        }
        else
        {
            debugln(F("[GPS] Bad latitude"));
            return NULL;
        }
    }
    else
    {
        _dataGPS->gpsLatitude[0] = '\0';
    }

    return pCurrentNMEA;
}

/// <summary>
/// Parse de la longitude
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseLongitude(char* nmea)
{
    float   longitude = 0;
    int32_t degree;
    float   minutes;
    char    degreebuff[10];

    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Lecture et Parse de la longitude
    if (*pCurrentNMEA != ',')
    {
        // On lit la partie entière
        strncpy(degreebuff, pCurrentNMEA, 3);
        pCurrentNMEA += 3;
        degreebuff[3] = '\0';
        degree = atol(degreebuff);

        // On lit la partie décimale
        strncpy(degreebuff, pCurrentNMEA, 2);
        degreebuff[2] = '.';
        pCurrentNMEA += 3;
        strncpy(degreebuff + 3, pCurrentNMEA, 4);
        degreebuff[7] = '\0';
        minutes = atof(degreebuff) / 60;

        // On ajoute les valeurs lues
        longitude = degree + minutes;
    }

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Formatage de la Longitude en fonction de l'orientation
    if (*pCurrentNMEA != ',')
    {
        if (pCurrentNMEA[0] == 'W')
        {
            sprintf(_dataGPS->gpsLongitude, "%s W", CoordinateToString(longitude));
        }
        else if (pCurrentNMEA[0] == 'E')
        {
            sprintf(_dataGPS->gpsLongitude, "%s E", CoordinateToString(longitude));
        }
        else
        {
            debugln(F("[GPS] Bad longitude"));
            return NULL;
        }
    }
    else
    {
        _dataGPS->gpsLongitude[0] = '\0';
    }

    return pCurrentNMEA;
}

/// <summary>
/// Parse de l'Altitude
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseAltitude(char* nmea)
{
    float aaltitude = 0;

    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Lecture et Parse de l'altitude
    if (*pCurrentNMEA != ',')
    {
        aaltitude = atof(pCurrentNMEA);
        sprintf(_dataGPS->gpsAltitude, "%.2f m", aaltitude);
    }
    else
    {
        strcpy(_dataGPS->gpsAltitude, "0");
    }

    return pCurrentNMEA;
}

/// <summary>
/// Parse du Fix
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseFix(char* nmea, bool letterValue)
{
    uint8_t fixquality;

    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Lecture et Parse du Fix
    if (*pCurrentNMEA != ',')
    {
        // Lecture du Fix avec des valeurs Alphanumériques
        if (letterValue)
        {
            if (pCurrentNMEA[0] == 'A')
                _dataGPS->gpsFix = true;
            else if (pCurrentNMEA[0] == 'V')
                _dataGPS->gpsFix = false;
            else
            {
                _dataGPS->gpsFix = false;
                return NULL;
            }
        }
        // Lecture du Fix avec des valeurs Numériques
        else
        {
            fixquality = atoi(pCurrentNMEA);
            if (fixquality > 0)
            {
                _dataGPS->gpsFix = true;
            }
            else
            {
                _dataGPS->gpsFix = false;
            }
        }
    }
    else
    {
        _dataGPS->gpsFix = false;
    }

    return pCurrentNMEA;
}

/// <summary>
/// Parse du nombre de satellites
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseSatellites(char* nmea)
{
    uint8_t ssatellites;

    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    // Lecture et Parse du nombre de satellites
    if (*pCurrentNMEA != ',')
    {
        ssatellites = atoi(pCurrentNMEA);
        sprintf(_dataGPS->gpsSatellites, "%d", ssatellites);
    }
    else
    {
        strcpy(_dataGPS->gpsSatellites, "0");
    }

    return pCurrentNMEA;
}

/// <summary>
/// Parse d'un champ non utilisé
/// </summary>
/// <param name="nmea"></param>
char* ROMV2_GPS::NMEAParseUnknown(char* nmea)
{
    // Création d'un pointeur permettant le parcours de la string
    char* pCurrentNMEA = nmea;

    // Recherche du caractère de séparation
    pCurrentNMEA = strchr(pCurrentNMEA, ',') + 1;
    if (pCurrentNMEA == NULL)
    {
        return NULL;
    }

    return pCurrentNMEA;
}

/// <summary>
/// Parse d'une commande NMEA HEX
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
uint8_t ROMV2_GPS::GpsParseHex(char c)
{
  if (c < '0')
    return 0;
  if (c <= '9')
    return c - '0';
  if (c < 'A')
    return 0;
  if (c <= 'F')
    return (c - 'A') + 10;
  // if (c > 'F')
  return 0;
}

/// <summary>
/// Envoi d'une commande au GPS
/// </summary>
/// <param name="str"></param>
void ROMV2_GPS::GpsSendCommand(const char *str)
{
  _gpsSerial.println(str);
}

/// <summary>
/// Retourne une coordonnées formatée en string
/// </summary>
/// <param name="coordinate"></param>
/// <returns></returns>
char* ROMV2_GPS::CoordinateToString(float coordinate)
{
    if (!isnan(coordinate) && coordinate != 0)
    {
        int deg = static_cast<int>(coordinate);
        double remainingMinutes = fabs(coordinate - deg) * 60.0;
        int min = static_cast<int>(remainingMinutes);
        float sec = (remainingMinutes - min) * 60.0;

        sprintf(stringBufferCoordinate, "%02d %02d%s %.1f%s", deg, min, "'", sec, "\"");
        return stringBufferCoordinate;
    }

    stringBufferCoordinate[0] = '\0';
    return stringBufferCoordinate;
}

/// <summary>
/// Lecture de commande sur port série du GPS
/// </summary>
void ROMV2_GPS::ReadGPS()
{
    ReadNMEALine();
    if (_gpsNMEAFlag == true)
    {
        _gpsNMEAFlag = false;
        if (ParseNMEALine((char *)lastline) == false)
        {
            debugln(F("[GPS] Unable to parse NMEA message"));
        }
        else
        {
            // MAJ du chrono
            _chronoLastGPSState = millis();

            // Trace
            debugln(F("[GPS] Parse NEMA: "));
            //debugln((char *)lastline);

            debug(F("[GPS] "));
            debug(_dataGPS->gpsDate);
            debug(F(" "));
            debugln(_dataGPS->gpsTime);

            debug(F("[GPS] L: "));
            debug(_dataGPS->gpsLongitude);
            debug(F(" / l: "));
            debug(_dataGPS->gpsLatitude);
            debug(F(" / A: "));
            debug(_dataGPS->gpsAltitude);
            debug(F(" / Sat: "));
            debug(_dataGPS->gpsSatellites);
            debug(F(" / Fix: "));
            debugln(_dataGPS->gpsFix ? F("1") : F("0"));
        }
    }
}

/// <summary>
/// Vérification de la dernière lecture de données du GPS pour vérification de l'état
/// </summary>
void ROMV2_GPS::CheckGPSState()
{
    // Vérification sur intervalle de la dernière lecture
    if (millis() > _chronoLastGPSState + READ_GPS_STATE_INTERVAL)
    {
        // Reset des données
        ResetGPSData();

        // MAJ du chrono
        _chronoLastGPSState = millis();
    }
}

/// <summary>
/// Reset des données du GPS
/// </summary>
void ROMV2_GPS::ResetGPSData()
{
    // Reset des données
    strcpy(_dataGPS->gpsDate, "");
    strcpy(_dataGPS->gpsTime, "");
    strcpy(_dataGPS->gpsLatitude, "");
    strcpy(_dataGPS->gpsLongitude, "");
    strcpy(_dataGPS->gpsAltitude, "");
    strcpy(_dataGPS->gpsSatellites, "0");
    _dataGPS->gpsFix = false;
}
