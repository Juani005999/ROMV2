/// ---------------------------------------------------------------------
///
/// Projet                          : ROMV2 - Remote Open Météo Version 2 - Sky Quality Meter DIY
/// Auteur                          : Juanito del Pepito
/// Version                         : 2.1.0.1
/// Date                            : 10/04/2026
/// Date Révision                   : 27/06/2026
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
#include <ROMV2_I2C.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2591.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_ADXL345_U.h>
#include <TFT_eSPI.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Preferences.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Définition des constantes correspondant aux PIN
#define ESP32_GPIO_SDA								21                  // GPIO I2C SDA
#define ESP32_GPIO_SCL								22                  // GPIO I2C SCL
#define GPS_RX										17					// RX pour le Serial du GPS
#define GPS_TX										16					// TX pour le Serial du GPS
#define JOYSTICK_PIN_SWITCH							25                  // Joystick Switch
#define JOYSTICK_PIN_X								32                  // Joystick X
#define JOYSTICK_PIN_Y								33                  // Joystick Y

// Définition des constantes correspondant aux paramètres des capteurs
#define BME280_I2C_ADDRESS							0x76                // Sensor T/P/H BME280
#define TSL2591_NUMBER_ID							2591                // Identifiant du TSL2591
#define ADXL345_NUMBER_ID							7112                // Identifiant du ADXL345
#define GPS_BAUD									9600				// Vitesse de communication sur le port série du GPS

// Définition des constantes correspondant aux intervalles
#define READ_ENVIRONMENT_INTERVAL					3000                // Intervalle de lecture de l'environnement via BME280
#define READ_LUX_INTERVAL							2000				// Intervalle de lecture de la luminosité via TSL2591
#define READ_IRTEMP_INTERVAL						2000                // Intervalle de lecture de la température IR via MLX90614
#define READ_ACCEL_INTERVAL							120					// Intervalle de lecture de l'accélération via ADXL345
#define READ_GPS_STATE_INTERVAL						5000				// Intervalle de lecture de l'état du GPS
#define BLE_NOTIFY_INTERVAL							1000				// Intervalle de Notification des caractéristiques BLE (Bluetooth Low Energy)
#define BLE_ADVERTISING_RESTART_DELAY				500					// Délai avant relance de l'advertising après déconnexion (laisse la stack BLE se stabiliser)
#define DISPLAY_INTERVAL							120                 // Intervalle d'affichage sur l'écran
#define JOYSTICK_BUTTON_LONG_INTERVAL				1000				// Intervalle du clic long sur le Joystick
#define JOYSTICK_MOVE_LONG_INTERVAL					500					// Intervalle du mouvement long sur le Joystick
#define JOYSTICK_READ_INTERVAL						50					// Intervalle de lecture de l'état du Joystick
#define WIFI_LOOP_INTERVAL							500                 // Intervalle de traitement du loop WiFi (ms)
#define WIFI_CHECKSTATE_INTERVAL					200					// Interval de vérification de l'état du Wifi (ms)
#define MQTT_RECONNECT_INTERVAL						10000				// Interval entre deux tentatives de reconnexion MQTT
#define MQTT_PUBLISH_INTERVAL						10000               // Intervalle de publication MQTT (ms)
#define MQTT_LOOP_INTERVAL							200                 // Intervalle de traitement du loop MQTT (ms)

// Définition des constantes de taille pour les chaînes de caractères
#define NMEA_CMD_LINE_LENGTH						256                 // Taille max pour une chaîne MNEA provenant du GPS
#define BT_CMD_LINE_LENGTH							256                 // Taille max d'une commande Bluetooth

// Définition des constantes pour le GPS
#define PMTK_SET_NMEA_UPDATE_10S					"$PMTK220,10000*2F"
#define PMTK_SET_NMEA_UPDATE_5S						"$PMTK220,5000*1B"
#define PMTK_SET_NMEA_UPDATE_1HZ					"$PMTK220,1000*1F"
#define PMTK_API_SET_FIX_CTL_1HZ					"$PMTK300,1000,0,0,0,0*1C"
#define PMTK_SET_BAUD_9600							"$PMTK251,9600*17"
#define PMTK_SET_BAUD_57600							"$PMTK251,57600*17"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA					"$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// Définition des constantes pour la calibration de la lecture du TSL2591
#define TSL2591_SENSOR_VALUE_LOW					750					// Niveau bas du sensor => augmentation de la sensibilité du sensor si valeur en dessous
#define TSL2591_SENSOR_VALUE_HIGH					20000				// Niveau haut du sensor => diminution de la sensibilité du sensor si valeur au dessus
#define MIN_LUX_THRESHOLD							0.00005f			// Seuil minimum de lux valide (en dessous = bruit capteur)
#define TSL2591_MOVING_AVERAGE_COUNT				50					// Nombre de valeurs pour le calcul de la moyenne mobile du Lux
#define TSL2591_CORRECTION_TEMP_COEFF				-0.002f				// Coefficient thermique /°C
#define TSL2591_CORRECTION_TEMP_REF_C				25.0f				// Température de référence (°C)
#define TSL2591_CALIBRATION_MIN_VALUE				-200				// Valeur minimale pour la calibration du TSL2591
#define TSL2591_CALIBRATION_MAX_VALUE				200					// Valeur maximale pour la calibration du TSL2591

// Définition des constantes pour la calibration de l'état du ciel
#define SKY_STATE_POINT_LOW							5					// Calibration basse pour l'état du ciel
#define SKY_STATE_POINT_HIGH						20					// Calibration haute pour l'état du ciel

// Définition des constantes pour la communication Bluetooth Low Energy (BLE)
#define BLE_ROMV2_DEVICE_NAME							"REMOTE OPEN METEO"
#define BLE_ROMV2_SERVICE_UUID							"b97ef9b6-d251-4d15-a748-53997037d0b8"
#define BLE_ROMV2_CHARACTERISTIC_UUID_TEMPERATURE		"cba1d466-344c-4be3-ab3f-189f80dd7518"
#define BLE_ROMV2_CHARACTERISTIC_UUID_HUMIDITY			"ca73b3ba-39f6-4ab3-91ae-186dc9577d99"
#define BLE_ROMV2_CHARACTERISTIC_UUID_PRESSURE			"fb6feb39-6b4a-4b9d-8529-ef6c311b4e0d"
#define BLE_ROMV2_CHARACTERISTIC_UUID_DEWPOINT			"e121cbc7-ef2c-4924-8a87-b0d83e1a9c05"
#define BLE_ROMV2_CHARACTERISTIC_UUID_CLOUDCOVER		"cd213e3e-94d9-4cf3-be60-03702c0b9b9b"
#define BLE_ROMV2_CHARACTERISTIC_UUID_SKYBRIGHTNESS		"67be4745-e3c4-4513-ab75-7bb9744214da"
#define BLE_ROMV2_CHARACTERISTIC_UUID_SKYQUALITY		"ebd480d1-15fd-4be7-b2fb-22fd97c71ecb"
#define BLE_ROMV2_CHARACTERISTIC_UUID_SKYTEMPERATURE	"9f0fc588-e1b5-4c3c-8a6a-2ce78c84166d"
#define BLE_SQMLITE_DEVICE_NAME							"SQM Lite"
#define BLE_SQMLITE_SERVICE_UUID						"d45872d6-b482-488e-b620-a0bb4a9c4d86"
#define BLE_SQMLITE_CHARACTERISTIC_UUID_TEMPERATURE		"fd1839f2-f064-4f47-9e18-d212cbe538a9"
#define BLE_SQMLITE_CHARACTERISTIC_UUID_HUMIDITY		"634fb69b-62b4-4fec-bce9-bc70e6b8166c"
#define BLE_SQMLITE_CHARACTERISTIC_UUID_PRESSURE		"b8e5b7e1-4693-40e7-adbf-7784299e3b51"
#define BLE_SQMLITE_CHARACTERISTIC_UUID_DEWPOINT		"41ce96a2-b73b-46be-90be-34e5dcd59c56"
#define BLE_SQMLITE_CHARACTERISTIC_UUID_SKYBRIGHTNESS	"6fcf4961-63d5-4a3c-9b4a-60163aefa479"
#define BLE_SQMLITE_CHARACTERISTIC_UUID_SKYQUALITY		"09c6e80f-55ed-4d70-bff2-62fc6ba07b3e"
#define BLE_CHARACTERISTIC_NUMHANDLES_COUNT				32				// Nombre de Handles nécessaires pour les caractéristiques du service BLE

// Définition des constantes pour l'affichage
#define SCREEN_WIDTH								160					// Largeur de l'affichage TFT
#define SCREEN_HEIGHT								128					// Hauteur de l'affichage TFT
#define SCREEN_HDR_HEIGHT							30					// Hauteur du Header pour l'affichage
#define SCREEN_TIMEOUT								300000				// Timeout (ms) de mise en veille de l'affichage
#define TFT_FONT_DEFAULT							1					// Indice de la police par défaut
#define TFT_FONT_TITLE								2					// Indice de la police du titre de l'application

// Définition des constantes pour le Joystick
#define ROMV2_JOYSTICK_POSITION_MIN_VALUE_LOW		700
#define ROMV2_JOYSTICK_POSITION_MIN_VALUE_HIGH		3850
#define SQMLITE_JOYSTICK_POSITION_MIN_VALUE_LOW		700
#define SQMLITE_JOYSTICK_POSITION_MIN_VALUE_HIGH	3300

// Définition des constantes utiles au WiFi
#define WIFI_STA_CONNECT_TIMEOUT					10000				// Timeout de connexion au Wifi en mode STA (ms)
#define WIFI_AP_SSID_ROMV2							"ROMV2"				// SSID du point d'accès AP en mode APP_ROMV2
#define WIFI_AP_SSID_SQMLITE						"SQMLite"			// SSID du point d'accès AP en mode APP_SQMLITE
#define WIFI_AP_PWD									""					// Password du point d'accès AP : pas de mot de passe en mode AP
#define WIFI_READ_CLIENT_TIMEOUT					2000				// Timeout de lecture d'une requête Http

// Définition des constantes utiles au MQTT
#define MQTT_CLIENT_ID_ROMV2						"ROMV2"				// Identifiant MQTT du ROMV2
#define MQTT_CLIENT_ID_SQMLITE						"SQMLite"			// Identifiant MQTT du SQMLite
#define MQTT_DEFAULT_PORT							1883				// Port par défaut du Broker MQTT

// Définition des constantes utiles aux préférences stockées en EEPROM
#define PREF_TSL2591_NAMESPACE						"config"			// Namespace de la configuration du TSL2591
#define PREF_TSL2591_KEY_NAME						"tsl_calib"			// Nom de la clé de la configuration du TSL2591
#define PREF_WIFI_NAMESPACE							"wifi"				// Namespace de la configuration du Wifi
#define PREF_WIFI_KEY_SSID							"ssid"				// Nom de la clé SSID de la configuration du Wifi
#define PREF_WIFI_KEY_PWD							"password"			// Nom de la clé Password de la configuration du Wifi
#define PREF_MQTT_NAMESPACE							"mqtt"				// Namespace de la configuration du MQTT
#define PREF_MQTT_KEY_BROKER_IP						"broker_ip"			// Nom de la clé Broker Ip de la configuration du MQTT
#define PREF_MQTT_KEY_BROKER_PORT					"broker_port"		// Nom de la clé Broker Port de la configuration du MQTT
#define PREF_NETWORK_NAMESPACE						"network"			// Namespace de la configuration du réseau
#define PREF_NETWORK_KEY_NETWORK					"mode"				// Nom de la clé Mode de la configuration du réseau

// Définition du type d'application
enum APP_TYPE
{
	APP_ROMV2,															// Application ROMV2
	APP_SQMLITE															// Application SQMLite
};

// Définition des différents mode d'affichage
enum DISPLAY_SCREEN_TYPE
{
	DISPLAY_NONE,														// Ecran en mode veille
	DISPLAY_HOME,														// Ecran d'accueil
	DISPLAY_LUX,														// Ecran Luminosité
	DISPLAY_ENVIRONMENT,												// Ecran Environnement
	DISPLAY_IRTEMP,														// Ecran état du ciel
	DISPLAY_ACCELERATION,												// Ecran Niveau à bulle
	DISPLAY_GPS,														// Ecran GPS
	DISPLAY_NETWORK,													// Ecran de sélection du réseau
	DISPLAY_WIFI,														// Ecran de configuration du Wifi
	DISPLAY_TSL2591_CALIBRATION											// Ecran de calibration du TSL2591
};

// Définition des différents mode d'affichage de l'écran Home
enum DISPLAY_HOME_TYPE
{
	DISPLAY_HOME_LUX,													// Ecran d'accueil en mode LUX
	DISPLAY_HOME_SQM													// Ecran d'accueil en mode SQM
};

// Définition des différents type d'action depuis le joystick
enum ACTION_TYPE
{
	ACTION_NONE,														// Pas d'action en cours
	ACTION_CLICK,														// Clic court
	ACTION_CLICK_LONG,													// Clic long
	ACTION_RIGHT,														// Joystick Droite
	ACTION_LEFT,														// Joystick Gauche
	ACTION_UP,															// Joystick Haut
	ACTION_DOWN															// Joystick Bas
};

// Définition des différents état du point de rosée
enum DEWPOINT_STATE
{
	DEWPOINT_STATE_DRY,													// Sec
	DEWPOINT_STATE_HUMID,												// Humide
	DEWPOINT_STATE_WET,													// Mouillé
	DEWPOINT_STATE_UNKNOWN												// Inconnu
};

// Définition des différents état du ciel
enum SKY_STATE
{
	SKY_STATE_CLEAR,													// Cliel clair
	SKY_STATE_PARTLY_CLOUDY,											// Peu nuageux
	SKY_STATE_CLOUDY,													// Nuageux
	SKY_STATE_UNKNOWN													// Inconnu
};

enum NETWORK_MODE
{
	NETWORK_NONE,														// Pas de réseau activé
	NETWORK_BLE,														// Réseau Bluetooth BLE activé
	NETWORK_WIFI														// Réseau Wifi activé
};

// Définition des différents états WiFi
enum WIFI_STATE
{
	WIFI_STATE_STA,														// Mode Station : connexion au réseau configuré
	WIFI_STATE_AP,														// Mode Access Point : attente de configuration via page web
	WIFI_STATE_CONNECTING												// Connexion en cours (transitoire)
};

// États internes de l'écran WiFi
enum DISPLAY_WIFI_STATE
{
	DISPLAY_WIFI_STATE_STA,												// Mode STA actif : Affichage SSID/Password + bouton Réinitialiser
	DISPLAY_WIFI_STATE_STA_CONNECTING,									// Mode STA actif : Affichage Connexion en cours
	DISPLAY_WIFI_STATE_AP_WAITING										// Mode AP actif : attente config web + bouton Annuler
};

// États internes de l'écran WiFi
enum ACTION_WIFI
{
	ACTION_WIFI_NONE,													// Pas d'action en cours
	ACTION_WIFI_START_STA,												// Démarrage du mode Wifi STA
	ACTION_WIFI_START_AP,												// Démarrage du mode AP
	ACTION_WIFI_CANCEL_AP,												// Annulation du mode AP
	ACTION_WIFI_RESET_CREDENTIALS
};

// Structure des données nécessaires pour la gestion du capteur d'environnement BME280
struct DataSensorEnvironment {
	float			temperature = NAN;									// Température (°C)
	float			humidite = NAN;										// Taux d'humidité	(%)
	float			pression = NAN;										// Pression atmosphérique (Pa)
	float			dewPoint = NAN;										// Point de rosée (°C)
	DEWPOINT_STATE	dewPointState = DEWPOINT_STATE_UNKNOWN;				// Etat du point de rosée pour l'icone
};

// Structure de résultat pour la conversion de Lux vers Mag/Arcsec²
struct LuxToMagConversionResult {
	float	lux;														// Lux mesuré par le TSL2591
	double	magnitude_arcsec2;											// Résultat final (mag/arcsec²)
	bool	valid;
};

// Structure des données nécessaires pour la gestion du capteur de luminosité TSL2591
struct DataSensorLuminosity {
	float	ir = NAN;													// Mesure de la luminosité dans l'IR
	float	full = NAN;													// Mesure de la luminosité totale
	float	visible = NAN;												// Mesure de la luminosité dans le visible
	float	lux = NAN;													// Mesure de la quantité de Lux
	float	luxAverage = NAN;											// Moyenne mobile du Lux sur TSL2591_MOVING_AVERAGE_COUNT valeurs
	float	luxThermalCorrected = NAN;									// Lux après correction thermique
	int		luxAverageCount = 0;										// Nombre de valeurs dans la queue pour le calcul de la moyenne mobile du Lux
	double	sqm = NAN;													// Valeure calculée du SQM (Mag/Arcsec²)
	float	bortle = NAN;												// Valeure du Bortle
	char	luxSensorGain[20];											// Gain du sensor
	char	luxSensorTiming[20];										// Temps d'exposition du sensor
	int		tsl2591Calibration = 0;										// Calibration du TSL2591. Cette valeur est divisée par 100 dans le calcul. On part sur des int afin d'éviter des problèmes de précision des nombres flotants
};

// Structure des données nécessaires pour la gestion du capteur d'état du ciel MLX90614
struct DataSensorSkyState {
	float		tempAmbient = NAN;										// Température ambiante
	float		tempObject = NAN;										// Température de l'objet
	SKY_STATE	skyState = SKY_STATE_UNKNOWN;							// Etat du ciel
	float		cloudCover = NAN;										// Couverture nuageuse (%)
};

// Structure des données nécessaires pour la gestion du capteur d'accélération ADXL345
struct DataSensorAcceleration {
	float	x = NAN;													// Accélération en X
	float	y = NAN;													// Accélération en Y
	float	z = NAN;													// Accélération en Z
};

// Structure des données nécessaires pour la gestion du capteur GPS NEO 8M
struct DataSensorGPS {
	char	gpsDate[20];												// Date
	char	gpsTime[20];												// Heure
	char	gpsLatitude[20];											// Latitude
	char	gpsLongitude[20];											// Longitude
	char	gpsAltitude[20];											// Altitude
	char	gpsSatellites[20];											// Nombre de satellites
	bool	gpsFix = false;												// GPS fixé
};

// Structure des données WiFi exposées à l'affichage et à l'application
struct DataWifi {
	char        ssid[64] = "";											// SSID du réseau configuré
	char        password[64] = "";										// Password du réseau configuré (masqué à l'affichage)
	char        ip[20] = "";											// IP obtenue en mode STA
	char        apSsid[32] = "";										// SSID du point d'accès en mode AP
	char        apIp[20] = "";											// IP du point d'accès en mode AP
	WIFI_STATE  wifiState = WIFI_STATE_STA;								// Etat de la connexion WiFi en cours
	bool        connected = false;										// Connecté au réseau en mode STA
	bool		configReceived = false;									// Nouvelle config reçue via page web
};

// Structure des données MQTT exposées à l'application
struct DataMqtt {
	char    brokerIp[40] = "";											// IP du broker Mosquitto
	int     brokerPort = MQTT_DEFAULT_PORT;								// Port du broker (défaut MQTT)
	bool    connected = false;											// Connecté au broker
};
