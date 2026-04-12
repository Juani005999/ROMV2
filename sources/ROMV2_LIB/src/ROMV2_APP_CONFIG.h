/// ---------------------------------------------------------------------
///
/// Projet                          : ROMV2 - Remote Open Météo Version 2 - Sky Quality Meter
/// Auteur                          : Juanito del Pepito
/// Version                         : 2.0.1.0
/// Date                            : 10/04/2026
/// 
/// Description                     : Sky Quality Meter (SQM) - Fichier de configuration de l'application
/// Gitub                           : https://github.com/Juani005999/ROMV2
/// Licence							: GPL-3.0 license
///
/// Capteurs et périphériques       : Cette application utilise les capteurs et périphériques suivant
///										- TSL2591
///										- MLX90614
///										- BME280
///										- ADXL345
///										- GPS NEO 8M
///										- TFT ST7735
/// 
/// Prérequis                       : Cette application nécessite l'ajout des bibliothèques suivantes
///										- Adafruit_TSL2591
///										- Adafruit_MLX90614
///										- Adafruit_BME280
///										- Adafruit_ADXL345
///										- TFT_eSPI
/// 
/// ---------------------------------------------------------------------
#pragma once

// Include des librairies
#include "JUANITO_APP.h"
#include "JUANITO_MOVING_AVERAGE.h"
#include <ROMV2_APP_ICONS.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2591.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_ADXL345_U.h>
#include <TFT_eSPI.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Définition des constantes correspondant aux PIN
#define ESP32_GPIO_SDA                          21                      // GPIO I2C SDA
#define ESP32_GPIO_SCL                          22                      // GPIO I2C SCL
#define GPS_RX                                  17						// RX pour le Serial du GPS
#define GPS_TX                                  16						// TX pour le Serial du GPS
#define JOYSTICK_PIN_SWITCH                     25                      // Joystick Switch
#define JOYSTICK_PIN_X                          32                      // Joystick X
#define JOYSTICK_PIN_Y                          33                      // Joystick Y

// Définition des constantes correspondant aux paramètres des capteurs
#define BME280_I2C_ADDRESS                      0x76                    // Sensor T/P/H BME280
#define TSL2591_NUMBER_ID                       2591                    // Identifiant du TSL2591
#define ADXL345_NUMBER_ID                       7112                    // Identifiant du ADXL345
#define GPS_BAUD                                9600					// Vitesse de communication sur le port série du GPS

// Définition des constantes correspondant aux intervalles
#define READ_ENVIRONMENT_INTERVAL               3000                    // Intervalle de lecture de l'environnement via BME280
#define READ_LUX_INTERVAL                       5000					// Intervalle de lecture de la luminosité via TSL2591
#define READ_IRTEMP_INTERVAL                    2000                    // Intervalle de lecture de la température IR via MLX90614
#define READ_ACCEL_INTERVAL                     120						// Intervalle de lecture de l'accélération via ADXL345
#define READ_GPS_STATE_INTERVAL					5000					// Intervalle de lecture de l'état du GPS
#define BLE_NOTIFY_INTERVAL						1000					// Intervalle de Notification des caractéristiques BLE (Bluetooth Low Energy)
#define DISPLAY_INTERVAL                        120                     // Intervalle d'affichage sur l'écran
#define JOYSTICK_BUTTON_LONG_INTERVAL           1000					// Intervalle du clic long sur le Joystick
#define JOYSTICK_READ_INTERVAL					50						// Intervalle de lecture de l'état du Joystick

// Définition des constantes de taille pour les chaînes de caractères
#define NMEA_CMD_LINE_LENGTH                    256                     // Taille max pour une chaîne MNEA provenant du GPS
#define BT_CMD_LINE_LENGTH                      256                     // Taille max d'une commande Bluetooth

// Définition des constantes pour le GPS
#define PMTK_SET_NMEA_UPDATE_10S                "$PMTK220,10000*2F"
#define PMTK_SET_NMEA_UPDATE_5S                 "$PMTK220,5000*1B"
#define PMTK_SET_NMEA_UPDATE_1HZ                "$PMTK220,1000*1F"
#define PMTK_API_SET_FIX_CTL_1HZ                "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_SET_BAUD_9600                      "$PMTK251,9600*17"
#define PMTK_SET_BAUD_57600						"$PMTK251,57600*17"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA             "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// Définition des constantes pour la calibration de la lecture du TSL2591
#define TSL2591_SENSOR_VALUE_LOW				750						// Niveau bas du sensor => augmentation de la sensibilité du sensor si valeur en dessous
#define TSL2591_SENSOR_VALUE_HIGH				20000					// Niveau haut du sensor => diminution de la sensibilité du sensor si valeur au dessus
#define TSL2591_CALIBRATION						0						// Valeur servant à la calibration du TSL2591 : le calcul donne 1.121, la pratique donne 0
#define MIN_LUX_THRESHOLD						0.0001f					// Seuil minimum de lux valide (en dessous = bruit capteur)
#define TSL2591_MOVING_AVERAGE_COUNT			50						// Nombre de valeurs pour le calcul de la moyenne mobile du Lux

// Définition des constantes pour la calibration de l'état du ciel
#define SKY_STATE_POINT_LOW						5						// Calibration basse pour l'état du ciel
#define SKY_STATE_POINT_HIGH					20						// Calibration haute pour l'état du ciel

// Définition des constantes pour la communication Bluetooth Low Energy (BLE)
#define BLE_DEVICE_NAME							"REMOTE OPEN METEO"
#define BLE_SERVICE_UUID						"b97ef9b6-d251-4d15-a748-53997037d0b8"
#define BLE_CHARACTERISTIC_UUID_TEMPERATURE		"cba1d466-344c-4be3-ab3f-189f80dd7518"
#define BLE_CHARACTERISTIC_UUID_HUMIDITY		"ca73b3ba-39f6-4ab3-91ae-186dc9577d99"
#define BLE_CHARACTERISTIC_UUID_PRESSURE		"fb6feb39-6b4a-4b9d-8529-ef6c311b4e0d"
#define BLE_CHARACTERISTIC_UUID_DEWPOINT		"e121cbc7-ef2c-4924-8a87-b0d83e1a9c05"
#define BLE_CHARACTERISTIC_UUID_CLOUDCOVER		"cd213e3e-94d9-4cf3-be60-03702c0b9b9b"
#define BLE_CHARACTERISTIC_UUID_SKYBRIGHTNESS	"67be4745-e3c4-4513-ab75-7bb9744214da"
#define BLE_CHARACTERISTIC_UUID_SKYQUALITY		"ebd480d1-15fd-4be7-b2fb-22fd97c71ecb"
#define BLE_CHARACTERISTIC_UUID_SKYTEMPERATURE	"9f0fc588-e1b5-4c3c-8a6a-2ce78c84166d"
#define BLE_CHARACTERISTIC_NUMHANDLES_COUNT		32						// Nombre de Handles nécessaires pour les caractéristiques du service BLE

// Définition des constantes pour l'affichage
#define SCREEN_WIDTH                            160						// Largeur de l'affichage TFT
#define SCREEN_HEIGHT                           128						// Hauteur de l'affichage TFT
#define SCREEN_HDR_HEIGHT                       30						// Hauteur du Header pour l'affichage
#define SCREEN_TIMEOUT							300000					// Timeout (ms) de mise en veille de l'affichage

// Définition des constantes pour le Joystick
#define JOYSTICK_POSITION_MIN_VALUE_LOW         700
#define JOYSTICK_POSITION_MIN_VALUE_HIGH        3850

// Définition des différents mode d'affichage
enum DISPLAY_SCREEN_TYPE
{
	DISPLAY_NONE,
	DISPLAY_HOME,
	DISPLAY_LUX,
	DISPLAY_ENVIRONMENT,
	DISPLAY_IRTEMP,
	DISPLAY_ACCELERATION,
	DISPLAY_GPS
};

// Définition des différents mode d'affichage de l'écran Home
enum DISPLAY_HOME_TYPE
{
	DISPLAY_HOME_LUX,
	DISPLAY_HOME_SQM
};

// Définition des différents type d'action depuis le joystick
enum ACTION_TYPE
{
	ACTION_NONE,
	ACTION_CLICK,
	ACTION_CLICK_LONG,
	ACTION_RIGHT,
	ACTION_LEFT,
	ACTION_UP,
	ACTION_DOWN
};

// Définition des différents état du point de rosée
enum DEWPOINT_STATE
{
	DEWPOINT_STATE_DRY,
	DEWPOINT_STATE_HUMID,
	DEWPOINT_STATE_WET,
	DEWPOINT_STATE_UNKNOWN
};

// Définition des différents état du ciel
enum SKY_STATE
{
	SKY_STATE_CLEAR,
	SKY_STATE_PARTLY_CLOUDY,
	SKY_STATE_CLOUDY,
	SKY_STATE_UNKNOWN
};

// Structure des données nécessaires pour la gestion du capteur d'environnement BME280
struct DataSensorEnvironment {
	float temperature = NAN;											// Température (°C)
	float humidite = NAN;												// Taux d'humidité	(%)
	float pression = NAN;												// Pression atmosphérique (Pa)
	float dewPoint = NAN;												// Point de rosée (°C)
	DEWPOINT_STATE dewPointState = DEWPOINT_STATE_UNKNOWN;				// Etat du point de rosée pour l'icone
};

// Structure de résultat pour la conversion de Lux vers Mag/Arcsec²
struct LuxToMagConversionResult {
	float lux;															// Lux mesuré par le TSL2591
	double magnitude_arcsec2;											// Résultat final (mag/arcsec²)
	bool valid;
};

// Structure des données nécessaires pour la gestion du capteur de luminosité TSL2591
struct DataSensorLuminosity {
	float ir = NAN;														// Mesure de la luminosité dans l'IR
	float full = NAN;													// Mesure de la luminosité totale
	float visible = NAN;												// Mesure de la luminosité dans le visible
	float lux = NAN;													// Mesure de la quantité de Lux
	float luxAverage = NAN;												// Moyenne mobile du Lux sur TSL2591_MOVING_AVERAGE_COUNT valeurs
	int luxAverageCount = 0;											// Nombre de valeurs dans la queue pour le calcul de la moyenne mobile du Lux
	double sqm = NAN;													// Valeure calculée du SQM (Mag/Arcsec²)
	float bortle = NAN;													// Valeure du Bortle
	char  luxSensorGain[20];											// Gain du sensor
	char  luxSensorTiming[20];											// Temps d'exposition du sensor
};

// Structure des données nécessaires pour la gestion du capteur d'état du ciel MLX90614
struct DataSensorSkyState {
	float tempAmbient = NAN;											// Température ambiante
	float tempObject = NAN;												// Température de l'objet
	SKY_STATE skyState = SKY_STATE_UNKNOWN;								// Etat du ciel
	float cloudCover = NAN;												// Couverture nuageuse (%)
};

// Structure des données nécessaires pour la gestion du capteur d'accélération ADXL345
struct DataSensorAcceleration {
	float x = NAN;														// Accélération en X
	float y = NAN;														// Accélération en Y
	float z = NAN;														// Accélération en Z
};

// Structure des données nécessaires pour la gestion du capteur GPS NEO 8M
struct DataSensorGPS {
	char  gpsDate[20];													// Date
	char  gpsTime[20];													// Heure
	char  gpsLatitude[20];												// Latitude
	char  gpsLongitude[20];												// Longitude
	char  gpsAltitude[20];												// Altitude
	char  gpsSatellites[20];											// Nombre de satellites
	bool  gpsFix = false;												// GPS fixé
};
