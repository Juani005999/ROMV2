#include "JUANITO_APP.h"

/// <summary>
/// Constructeur
/// </summary>
JUANITO_APP::JUANITO_APP()
{
}

/// <summary>
/// Réinitialisation des compteurs et chronos de performances de l'application
/// </summary>
void JUANITO_APP::InitAppPerformances()
{
#ifdef DEBUG
    // Actualisation du chrono et flag
    _lastLoopTime = millis();
    _nbTotalLoops = 0;

    // Trace
    debugln("[PERF] Trace performances started");
#endif
}

/// <summary>
/// Trace en console les performances de l'application
/// </summary>
void JUANITO_APP::TraceAppPerformances()
{
#ifdef DEBUG
    // TimeSpan actuel
    long currentDif = millis() - _lastLoopTime;

    // Temps min
    if (_minLoopTime == 0
        || (currentDif != 0 && currentDif < _minLoopTime))
    {
      _minLoopTime = currentDif;
    }

    // Temps MAX
    if (_maxLoopTime == 0 || currentDif > _maxLoopTime)
    {
      _maxLoopTime = currentDif;
    }

    // Actualisation moyenne
    _avgLoopTime = _avgLoopTime + ((currentDif - _avgLoopTime) / (_nbTotalLoops + 1));

    // On trace la moyenne tous les XX Loop
    if (_nbTotalLoops % PERF_LOOP_INTERVAL == 0)
    {
      debugln(F(""));
        
      // SRAM Restant
      debug(F("[PERF] SRAM restant: "));
      debug(GetFreeRam());
      debugln(F(" octets"));

      // LoopTime Min
      debug(F("[PERF] LoopTime : min = "));
      sprintf(stringBuffer, "%d", (int)_minLoopTime);
      debug(stringBuffer);
      debugln(F(" ms"));

      // LoopTime Max
      debug(F("[PERF] LoopTime : MAX = "));
      sprintf(stringBuffer, "%d", (int)_maxLoopTime);
      debug(stringBuffer);
      debugln(F(" ms"));

      // LoopTime AVG
      debug(F("[PERF] LoopTime : AVG = "));
      sprintf(stringBuffer, "%.8f", _avgLoopTime);
      debug(stringBuffer);
      debug(F(" ms / Nb. LOOPS = "));
      sprintf(stringBuffer, "%d", (int)_nbTotalLoops);
      debugln(stringBuffer);
    }

    // Actualisation du chrono et flag
    _lastLoopTime = millis();
    _nbTotalLoops++;
#endif
}

/// <summary>
/// Renvoi la quantité de mémoire disponbile restante
/// </summary>
/// <returns></returns>
int JUANITO_APP::GetFreeRam() {
#ifdef __AVR__
    extern int __heap_start, * __brkval;
    int v;
    return (int)&v - (__brkval == 0
        ? (int)&__heap_start : (int)__brkval);
#else
    return ESP.getFreeHeap();
#endif
}
