/// ---------------------------------------------------------------------
///
/// Auteur          : Juanito del Pepito
/// Version         : 1.1.0.1
/// Date            : 20/12/2025
/// Date Création   : 15/04/2023
/// Description     : Classe mère des applications utilisant la JUANITO_LIB
///					: Décommenter #define DEBUG pour passer en mode debug
/// Objet           : JUANITO_APP : Classe mère des Objets applicatif
///
/// ---------------------------------------------------------------------
#pragma once
// Librairies
#include "Arduino.h"

// Mode développement. Mettre en commentaire à la fin des développements
//#define DEBUG

// Define nécessaires au mode Debug
#ifdef DEBUG
  #define debug(x)              Serial.print(x)
  #define debugln(x)            Serial.println(x)
  #define debugwrite(x, y)      Serial.write(x, y)
  #define setDebugOutput(x)     Serial.setDebugOutput(x)
#else
  #define debug(x)
  #define debugln(x)
  #define debugwrite(x, y)
  #define setDebugOutput(x)
#endif

// Define nécessaires à l'application
#define         PERF_LOOP_INTERVAL           100000

/// <summary>
/// Objet JUANITO_APP : Classe mère des Objets applicatif
/// </summary>
class JUANITO_APP
{
    public:
        /// <summary>
        /// Constructeur
        /// </summary>
        JUANITO_APP();

        /// <summary>
        /// Trace des performances de l'application
        /// </summary>
        void TraceAppPerformances();

    protected:
        /// <summary>
        /// Réinitialisation des compteurs et chronos de performances de l'application
        /// </summary>
        void InitAppPerformances();

    private:
        int GetFreeRam();
        char stringBuffer[256];

        // Déclaration des variables nécessaire aux mesures des temps de Loop
        long  _lastLoopTime = 0;                                  // DEBUG : Flag de lecture de l'interval mini
        long  _minLoopTime = 0;                                   // DEBUG : Interval mini de la fonction loop
        long  _maxLoopTime = 0;                                   // DEBUG : Interval mini de la fonction loop
        float _avgLoopTime = 0;                                   // DEBUG : Moyenne du temps d'un Interval
        long  _nbTotalLoops = 0;                                  // DEBUG : Nombre total d'intervalles
};