/// ---------------------------------------------------------------------
///
/// Projet                          : SQM Lite - Sky Quality Meter DIY
/// Auteur                          : Juanito del Pepito
/// Version                         : 2.1.0.1
/// Date                            : 10/04/2026
/// Date Révision                   : 27/06/2026
/// 
/// Description                     : Sky Quality Meter (SQM)
/// Gitub                           : https://github.com/Juani005999/SQMLite
/// Licence							: GPL-3.0 license
///
/// Configuration                   : Les paramètres de configuration de l'application se trouvent dans le fichier suivant
///                                   <Arduino librairies> -> ROMV2 -> src -> ROMV2_APP_CONFIG.h
///
/// Capteurs et périphériques       : Cette application utilise les capteurs et périphériques suivant
///										- TSL2591
///										- BME280
///										- TFT ST7735
/// 
/// Prérequis                       : Cette application nécessite l'ajout des bibliothèques suivantes
///										- Adafruit_TSL2591
///										- Adafruit_BME280
///										- TFT_eSPI
/// 
/// ---------------------------------------------------------------------
#include <SQMLITE_APP.h>

// Instanciation des objets internes
SQMLITE_APP app = SQMLITE_APP();

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
