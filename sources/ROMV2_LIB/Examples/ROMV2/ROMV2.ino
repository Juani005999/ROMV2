/// ---------------------------------------------------------------------
///
/// Projet                          : ROMV2 - Remote Open Météo Version 2 - Sky Quality Meter
/// Auteur                          : Juanito del Pepito
/// Version                         : 2.0.1.0
/// Date                            : 10/04/2026
/// 
/// Description                     : Sky Quality Meter (SQM)
/// Gitub                           : https://github.com/Juani005999/ROMV2
/// Licence							            : GPL-3.0 license
///
/// Configuration                   : Les paramètres de configuration de l'application se trouvent dans le fichier suivant
///                                   <Arduino librairies> -> ROMV2 -> src -> ROMV2_APP_CONFIG.h
///
/// Capteurs et périphériques       : Cette application utilise les capteurs et périphériques suivant
///										                - TSL2591
///										                - MLX90614
///										                - BME280
///										                - ADXL345
///										                - GPS NEO 8M
///										                - TFT ST7735
/// 
/// Prérequis                       : Cette application nécessite l'ajout des bibliothèques suivantes
///                                   - ROMV2
///										                - Adafruit_TSL2591
///										                - Adafruit_MLX90614
///										                - Adafruit_BME280
///										                - Adafruit_ADXL345
///										                - TFT_eSPI
/// 
/// ---------------------------------------------------------------------
#include <ROMV2_APP.h>

// Instanciation des objets internes
ROMV2_APP app = ROMV2_APP();

void setup()
{
  // Initialisation SerialPort
  Serial.begin(115200);
  while(!Serial);

  // Initialisation de l'application
  app.Init();
}

void loop()
{
 // Exécution du processus Loop de l'application
  app.Loop();
}
