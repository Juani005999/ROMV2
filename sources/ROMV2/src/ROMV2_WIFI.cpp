#include <ROMV2_WIFI.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_WIFI::ROMV2_WIFI()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="dataWifi"></param>
void ROMV2_WIFI::Init(APP_TYPE appType, DataWifi* dataWifi, DataMqtt* dataMqtt)
{
    // Valorisation des membres internes
    _dataWifi = dataWifi;
    _dataMqtt = dataMqtt;
    _appType = appType;
    _apSSID = _appType == APP_SQMLITE ? WIFI_AP_SSID_SQMLITE : WIFI_AP_SSID_ROMV2;

    // Renseignement du SSID et de l'IP de l'AP
    strncpy(_dataWifi->apSsid, _apSSID.c_str(), sizeof(_dataWifi->apSsid) - 1);

    // Création du mutex FreeRTOS
    _mutex = xSemaphoreCreateMutex();

    // Chargement des credentials depuis les Preferences
    LoadCredentials();

    // Trace
    debugln(F(""));
    debugln(F("[WIFI] Initialisation"));
    debug(F("[WIFI] SSID charge : "));
    debugln(_dataWifi->ssid);

    // Démarrage en mode STA
    _nextAction = ACTION_WIFI_START_STA;
}

/// <summary>
/// Démarre la tâche FreeRTOS sur le coeur 0
/// </summary>
void ROMV2_WIFI::StartTask()
{
    // Trace
    debugln(F(""));
    debugln(F("[WIFI] Starting ROMV2_WIFI TaskLoop"));

    xTaskCreatePinnedToCore(
        TaskWrapper,    // Fonction statique wrapper
        "WIFI",         // Nom de la tâche
        4096,           // Stack en bytes (augmenter à 8192 si stack overflow)
        this,           // Passage du pointeur this pour accéder aux membres
        1,              // Priorité
        NULL,           // Handle (non utilisé)
        0               // Coeur 0 -> laisse le coeur 1 libre pour l'UI
    );

    // Trace
    debugln(F("[WIFI] ROMV2_WIFI TaskLoop Started"));
}

/// <summary>
/// Wrapper statique requis par FreeRTOS — redirige vers TaskLoop()
/// </summary>
void ROMV2_WIFI::TaskWrapper(void* pvParameters)
{
    // pvParameters contient le pointeur this passé dans StartTask()
    static_cast<ROMV2_WIFI*>(pvParameters)->TaskLoop();
}

/// <summary>
/// Boucle de la tâche FreeRTOS — tourne en continu sur le coeur 0
/// </summary>
void ROMV2_WIFI::TaskLoop()
{
    while (true)
    {
        // Traces
        debugln(F(""));
        debugln(F("[WIFI] Gestion du Wifi dans TaskLoop"));

        // Exécution de l'action en cours
        DoAction();

        // Lecture des données sous mutex
        bool apActive = false;
        bool connected = false;
        char ip[20] = "";
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            apActive = _apActive;
            connected = _connected;
            strncpy(ip, _ip, sizeof(ip) - 1);

            // On libère le mutex
            xSemaphoreGive(_mutex);
        }

        // En mode AP : traitement des requêtes web
        if (apActive && _webServer != nullptr)
        {
            HandleWebClient();
        }
        // En mode STA : vérification de la connexion
        else if (!apActive)
        {
            bool nowConnected = (WiFi.status() == WL_CONNECTED);

            if (nowConnected && !connected)
            {
                // Nouvelle connexion établie
                String currentIp = WiFi.localIP().toString();
                strncpy(ip, currentIp.c_str(), sizeof(ip) - 1);
                connected = true;

                // Trace
                debug(F("[WIFI] Nouvelle connexion. IP : "));
                debugln(ip);
            }
            else if (!nowConnected && connected)
            {
                // Connexion perdue
                connected = false;
                ip[0] = '\0';

                // Trace
                debugln(F("[WIFI] Connexion perdue"));
            }

            // Ecriture des données sous mutex
            if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
            {
                _connected = connected;
                strcpy(_ip, ip);

                // On libère le mutex
                xSemaphoreGive(_mutex);
            }
        }

        // Traces
        debug(F("[WIFI] IP: "));
        debugln(ip);
        debug(F("[WIFI] Connected: "));
        debugln(connected);

        // Libère le CPU
        vTaskDelay(pdMS_TO_TICKS(WIFI_CHECKSTATE_INTERVAL));
    }
}

/// <summary>
/// Effectue l'action en cours
/// </summary>
void ROMV2_WIFI::DoAction()
{
    // Lecture des données sous mutex
    ACTION_WIFI nextAction = ACTION_WIFI_NONE;
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        nextAction = _nextAction;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }

    // On effectue l'action souhaitée
    switch (nextAction)
    {
        case ACTION_WIFI_START_STA:
            DoStartSTA();
            break;
    
        case ACTION_WIFI_START_AP:
            DoStartAP();
            break;

        case ACTION_WIFI_CANCEL_AP:
            DoCancelAP();
            break;

        case ACTION_WIFI_RESET_CREDENTIALS:
            DoResetCredentials();
            break;

        case ACTION_WIFI_NONE:
        default:
            break;
    }

    // Ecriture des données sous mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _nextAction = ACTION_WIFI_NONE;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Traitements cadencés
/// </summary>
void ROMV2_WIFI::Loop()
{
    if (millis() - _chronoWifiLoop >= WIFI_LOOP_INTERVAL)
    {
        // Recopie des valeurs sous Mutex
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            _apActiveLocal = _apActive;
            _connectedLocal = _connected;
            strncpy(_ipLocal, _ip, sizeof(_ipLocal) - 1);
            _wifiStateLocal = _wifiState;
            strncpy(_ssidLocal, _ssid, sizeof(_ssidLocal) - 1);
            strncpy(_passwordLocal, _password, sizeof(_passwordLocal) - 1);
            strncpy(_brokerIpLocal, _brokerIp, sizeof(_brokerIpLocal) - 1);
            strncpy(_apIpLocal, _apIp, sizeof(_apIpLocal) - 1);
            _brokerPortLocal = _brokerPort;
            _configReceivedLocal = _configReceived;

            // On libère le mutex
            xSemaphoreGive(_mutex);
        }

        // Recopie des valeures Locales dans _dataWifi
        _dataWifi->connected = _connectedLocal;
        _dataWifi->wifiState = _wifiStateLocal;
        _dataWifi->configReceived = _configReceivedLocal;
        strncpy(_dataWifi->ip, _ipLocal, sizeof(_dataWifi->ip) - 1);
        strncpy(_dataWifi->ssid, _ssidLocal, sizeof(_dataWifi->ssid) - 1);
        strncpy(_dataWifi->password, _passwordLocal, sizeof(_dataWifi->password) - 1);
        strncpy(_dataWifi->apIp, _apIpLocal, sizeof(_dataWifi->apIp) - 1);
        strncpy(_dataMqtt->brokerIp, _brokerIpLocal, sizeof(_dataMqtt->brokerIp) - 1);
        _dataMqtt->brokerPort = _brokerPortLocal;

        // Actualisation Chrono
        _chronoWifiLoop = millis();
    }
}

/// <summary>
/// Démarre le mode STA avec les credentials chargés
/// </summary>
void ROMV2_WIFI::StartSTA()
{
    // Ecriture des données sous mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _nextAction = ACTION_WIFI_START_STA;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Démarre le mode STA avec les credentials chargés
/// </summary>
void ROMV2_WIFI::DoStartSTA()
{
    // Trace
    debugln(F(""));
    debugln(F("[WIFI] Demarrage mode STA"));

    char ssid[64] = "";
    char password[64] = "";
    // Lecture/Ecriture des valeures sous Mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        // Ecriture des variables d'état
        _wifiState = WIFI_STATE_CONNECTING;
        _connected = false;
        _ip[0] = '\0';

        // Lecture des paramètres de connexion
        strncpy(ssid, _ssid, sizeof(ssid) - 1);
        strncpy(password, _password, sizeof(password) - 1);

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }

    // Si pas de SSID configuré, on reste en STA déconnecté sans tenter de connexion
    if (strlen(ssid) == 0)
    {
        // Ecriture des valeures sous Mutex
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            _wifiState = WIFI_STATE_STA;

            // On libère le mutex
            xSemaphoreGive(_mutex);
        }

        // Trace et retour
        debugln(F("[WIFI] Aucun SSID configure"));
        return;
    }

    // Positionnement du mode Wifi STA et démarrage du Wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Attente de connexion avec timeout
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < WIFI_STA_CONNECT_TIMEOUT)
    {
        delay(200);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        // Désactive le modem sleep : stabilité MQTT
        WiFi.setSleep(false);
    }
    else
    {
        // Trace
        debugln(F("[WIFI] Echec de connexion"));
    }

    // Ecriture des valeures sous Mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _wifiState = WIFI_STATE_STA;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Démarre le mode AP
/// </summary>
void ROMV2_WIFI::StartAP()
{
    // Ecriture des données sous mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _nextAction = ACTION_WIFI_START_AP;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Démarre le mode AP
/// </summary>
void ROMV2_WIFI::DoStartAP()
{
    // Trace
    debugln(F(""));
    debugln(F("[WIFI] Demarrage mode AP"));

    // Arrêt du mode STA
    WiFi.disconnect(true);
    delay(200);

    // Démarrage de l'AP (sans mot de passe)
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_apSSID);
    delay(200);

    // Valorisation des membres associés à l'état du Wifi - Ecriture des valeures sous Mutex
    String apIp = WiFi.softAPIP().toString();
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _apActive = true;
        _wifiState = WIFI_STATE_AP;
        _connected = false;
        _ip[0] = '\0';
        strncpy(_apIp, apIp.c_str(), sizeof(_apIp) - 1);

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }

    // Démarrage du serveur web sur port 80
    if (_webServer == nullptr)
    {
        _webServer = new WiFiServer(80);
    }
    _webServer->begin();

    // Trace
    debug(F("[WIFI] AP actif. SSID : "));
    debugln(_apSSID);
    debug(F("[WIFI] IP AP : "));
    debugln(apIp);
}

/// <summary>
/// Arrête le mode AP
/// </summary>
void ROMV2_WIFI::StopAP()
{
    // Arrêt du serveur Web
    if (_webServer != nullptr)
    {
        _webServer->stop();
        delete _webServer;
        _webServer = nullptr;
    }

    // Lecture des données sous mutex
    bool apActive = false;
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        apActive = _apActive;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }

    // Arrêt du service Wifi AP
    if (apActive)
    {
        // Déconnexion du Wifi
        WiFi.softAPdisconnect(true);

        // Valorisation des membres associés à l'état du Wifi - Ecriture des données sous mutex
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            _apActive = false;
            _apIp[0] = '\0';

            // On libère le mutex
            xSemaphoreGive(_mutex);
        }
    }

    // Trace
    debugln(F(""));
    debugln(F("[WIFI] AP arrete"));
}

/// <summary>
/// Annule le mode AP et retourne en STA
/// </summary>
void ROMV2_WIFI::CancelAP()
{
    // Ecriture des données sous mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _nextAction = ACTION_WIFI_CANCEL_AP;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Annule le mode AP et retourne en STA
/// </summary>
void ROMV2_WIFI::DoCancelAP()
{
    StopAP();
    DoStartSTA();
}

/// <summary>
/// Traitement des clients web en mode AP
/// </summary>
void ROMV2_WIFI::HandleWebClient()
{
    // Vérification de la connexion du serveur Web et requête en cours disponible
    WiFiClient client = _webServer->available();
    if (!client)
    {
        return;
    }

    // Trace
    debugln(F(""));
    debugln(F("[WIFI] Client web connecte"));

    // Lecture de la requête HTTP
    String request = "";
    String currentLine = "";
    int contentLength = 0;
    unsigned long clientTimeout = millis();

    while (client.connected() && millis() - clientTimeout < WIFI_READ_CLIENT_TIMEOUT)
    {
        if (client.available())
        {
            char c = client.read();
            request += c;

            if (c == '\n')
            {
                // Lecture du Content-Length de la requête
                if (currentLine.startsWith("Content-Length:"))
                {
                    contentLength = currentLine.substring(15).toInt();
                }
                // Fin des Headers
                if (currentLine.length() == 0)
                {
                    break;
                }
                currentLine = "";
            }
            else if (c != '\r')
            {
                currentLine += c;
            }
        }
    }

    // Trace
    debug(F("[WIFI] Requete recue : "));
    debugln(request.substring(0, 80));

    // Traitement POST (soumission du formulaire)
    if (request.startsWith("POST"))
    {
        // Lecture du body (après les headers)
        String body = "";
        unsigned long bodyTimeout = millis();
        while (client.connected()
            && (int)body.length() < contentLength
            && millis() - bodyTimeout < WIFI_READ_CLIENT_TIMEOUT)
        {
            if (client.available())
            {
                body += (char)client.read();
                // Reset du Timeout à chaque octet reçu
                bodyTimeout = millis();
            }
        }

        // Trace
        debugln(F("[WIFI] Body POST recu"));

        // Extraction SSID, password, broker IP et port
        String newSsid = "";
        String newPassword = "";
        String newBrokerIp = "";
        int newBrokerPort = 1883;

        int ssidStart = body.indexOf("ssid=");
        int passStart = body.indexOf("&password=");
        int brokerIpStart = body.indexOf("&broker_ip=");
        int brokerPortStart = body.indexOf("&broker_port=");

        if (ssidStart >= 0)
        {
            int ssidEnd = body.indexOf("&", ssidStart);
            if (ssidEnd < 0) ssidEnd = body.length();
            newSsid = UrlDecode(body.substring(ssidStart + 5, ssidEnd));
        }

        if (passStart >= 0)
        {
            int passEnd = body.indexOf("&", passStart + 1);
            if (passEnd < 0) passEnd = body.length();
            newPassword = UrlDecode(body.substring(passStart + 10, passEnd));
        }

        if (brokerIpStart >= 0)
        {
            int brokerIpEnd = body.indexOf("&", brokerIpStart + 1);
            if (brokerIpEnd < 0) brokerIpEnd = body.length();
            newBrokerIp = UrlDecode(body.substring(brokerIpStart + 11, brokerIpEnd));
        }

        if (brokerPortStart >= 0)
        {
            int brokerPortEnd = body.indexOf("&", brokerPortStart + 1);
            if (brokerPortEnd < 0) brokerPortEnd = body.length();
            newBrokerPort = UrlDecode(body.substring(brokerPortStart + 13, brokerPortEnd)).toInt();
            if (newBrokerPort <= 0) newBrokerPort = 1883;
        }

        // Trace
        debug(F("[WIFI] Nouveau SSID : "));
        debugln(newSsid);
        debug(F("[WIFI] Nouveau Password : "));
        debugln(newPassword);
        debug(F("[WIFI] Nouveau Broker IP : "));
        debugln(newBrokerIp);
        debug(F("[WIFI] Nouveau Broker Port : "));
        debugln(newBrokerPort);

        if (newSsid.length() > 0 && newPassword.length() > 0)
        {
            // Sauvegarde des credentials
            SaveCredentials(newSsid, newPassword, newBrokerIp, newBrokerPort);

            // Réponse page succès
            ServeSuccess(client);

            // Arrêt AP et connexion STA avec les nouveaux credentials
            delay(1500);
            StopAP();
            // Ecriture des données sous mutex
            if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
            {
                _configReceived = true;

                // On libère le mutex
                xSemaphoreGive(_mutex);
            }
            StartSTA();
        }
        else
        {
            // SSID vide : re-servir la page de config
            ServeConfigPage(client);
        }
    }
    else
    {
        // Requête GET : page de configuration
        ServeConfigPage(client);
    }

    client.stop();
}

/// <summary>
/// Sert la page de configuration HTML
/// </summary>
void ROMV2_WIFI::ServeConfigPage(WiFiClient& client)
{
    String html = BuildConfigPage();
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html; charset=utf-8"));
    client.println(F("Connection: close"));
    client.print(F("Content-Length: "));
    client.println(html.length());
    client.println();
    client.print(html);
}

/// <summary>
/// Sert la page de succès HTML
/// </summary>
void ROMV2_WIFI::ServeSuccess(WiFiClient& client)
{
    String html = BuildSuccessPage();
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html; charset=utf-8"));
    client.println(F("Connection: close"));
    client.print(F("Content-Length: "));
    client.println(html.length());
    client.println();
    client.print(html);
}

/// <summary>
/// Construit la page HTML de configuration WiFi avec CSS inline minimal
/// </summary>
String ROMV2_WIFI::BuildConfigPage()
{
    String html = F("<!DOCTYPE html><html lang=\"fr\"><head>"
        "<meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
        "<title>ROMV2 / SQMLite - Configuration WiFi</title>"
        "<style>"
        // Reset / base
        "*, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }"
        "body { font-family: system-ui, sans-serif; font-size: 16px; line-height: 1.5;"
        "       background: #f0f2f5; color: #212529; min-height: 100vh;"
        "       display: flex; align-items: center; justify-content: center; padding: 1rem; }"
        // Card
        ".card { background: #ffffff; border: 1px solid #dee2e6; border-radius: .5rem;"
        "        width: 100%; max-width: 420px; overflow: hidden;"
        "        box-shadow: 0 2px 12px rgba(0,0,0,.08); }"
        ".card-header { background: #c0392b; padding: .85rem 1.25rem;"
        "               text-align: center; }"
        ".card-header h1 { font-size: 1.1rem; font-weight: 700; color: #ffffff; letter-spacing: .05em; }"
        ".card-body { padding: 1.5rem 1.25rem; }"
        ".card-footer { padding: .75rem 1.25rem; text-align: center;"
        "               font-size: .8rem; color: #6c757d;"
        "               border-top: 1px solid #dee2e6; }"
        // Subtitle
        ".subtitle { font-size: .85rem; color: #6c757d; text-align: center; margin-bottom: 1.25rem; }"
        // Form group
        ".form-group { margin-bottom: 1rem; }"
        ".form-group:last-of-type { margin-bottom: 1.5rem; }"
        "label { display: block; font-size: .85rem; color: #495057; margin-bottom: .35rem; }"
        "input { display: block; width: 100%; padding: .5rem .75rem;"
        "        background: #ffffff; color: #212529;"
        "        border: 1px solid #ced4da; border-radius: .375rem;"
        "        font-size: 1rem; outline: none; transition: border-color .15s; }"
        "input:focus { border-color: #c0392b; }"
        // Button
        ".btn { display: block; width: 100%; padding: .65rem 1rem;"
        "       background: #c0392b; color: #fff; font-size: 1rem; font-weight: 600;"
        "       border: none; border-radius: .375rem; cursor: pointer; transition: background .15s; }"
        ".btn:hover { background: #a93226; }"
        "</style>"
        "</head><body>"
        "<div class=\"card\">"
        "  <div class=\"card-header\"><h1>&#9733; ROMV2 / SQMLite &mdash; Sky Quality Meter</h1></div>"
        "  <div class=\"card-body\">"
        "    <p class=\"subtitle\">Configuration WiFi</p>"
        "    <form method=\"POST\" action=\"/\">"
        "      <div class=\"form-group\">"
        "        <label for=\"ssid\">Nom du r&eacute;seau (SSID)</label>"
        "        <input type=\"text\" id=\"ssid\" name=\"ssid\""
        "               placeholder=\"Mon r&eacute;seau WiFi\" required maxlength=\"63\">"
        "      </div>"
        "      <div class=\"form-group\">"
        "        <label for=\"password\">Mot de passe WiFi</label>"
        "        <input type=\"password\" id=\"password\" name=\"password\""
        "               placeholder=\"Mot de passe\" maxlength=\"63\">"
        "      </div>"
        "      <div class=\"form-group\">"
        "        <label for=\"broker_ip\">IP du broker MQTT</label>"
        "        <input type=\"text\" id=\"broker_ip\" name=\"broker_ip\""
        "               placeholder=\"192.168.1.x\" maxlength=\"39\">"
        "      </div>"
        "      <div class=\"form-group\">"
        "        <label for=\"broker_port\">Port du broker MQTT</label>"
        "        <input type=\"number\" id=\"broker_port\" name=\"broker_port\""
        "               placeholder=\"1883\" min=\"1\" max=\"65535\" value=\"1883\">"
        "      </div>"
        "      <button type=\"submit\" class=\"btn\">Enregistrer et connecter</button>"
        "    </form>"
        "  </div>"
        "  <div class=\"card-footer\">L'appareil relancera la connexion apr&egrave;s validation.</div>"
        "</div>"
        "</body></html>");
    return html;
}

/// <summary>
/// Construit la page HTML de confirmation avec CSS inline minimal
/// </summary>
String ROMV2_WIFI::BuildSuccessPage()
{
    String html = F("<!DOCTYPE html><html lang=\"fr\"><head>"
        "<meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
        "<title>ROMV2 / SQMLite - Configuration enregistr&eacute;e</title>"
        "<style>"
        "*, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }"
        "body { font-family: system-ui, sans-serif; font-size: 16px; line-height: 1.5;"
        "       background: #f0f2f5; color: #212529; min-height: 100vh;"
        "       display: flex; align-items: center; justify-content: center; padding: 1rem; }"
        ".card { background: #ffffff; border: 1px solid #dee2e6; border-radius: .5rem;"
        "        width: 100%; max-width: 420px; overflow: hidden; text-align: center;"
        "        box-shadow: 0 2px 12px rgba(0,0,0,.08); }"
        ".card-header { background: #c0392b; padding: .85rem 1.25rem; }"
        ".card-header h1 { font-size: 1.1rem; font-weight: 700; color: #ffffff; letter-spacing: .05em; }"
        ".card-body { padding: 2rem 1.25rem; }"
        ".icon-ok { font-size: 3rem; color: #28a745; line-height: 1; margin-bottom: .75rem; }"
        ".success-title { font-size: 1.15rem; font-weight: 700; color: #28a745; margin-bottom: .75rem; }"
        ".success-text { font-size: .9rem; color: #6c757d; }"
        "</style>"
        "</head><body>"
        "<div class=\"card\">"
        "  <div class=\"card-header\"><h1>&#9733; ROMV2 / SQMLite &mdash; Sky Quality Meter</h1></div>"
        "  <div class=\"card-body\">"
        "    <div class=\"icon-ok\">&#10003;</div>"
        "    <p class=\"success-title\">Configuration enregistr&eacute;e&nbsp;!</p>"
        "    <p class=\"success-text\">"
        "      L'appareil tente maintenant de se connecter au r&eacute;seau WiFi.<br>"
        "      Vous pouvez fermer cette page."
        "    </p>"
        "  </div>"
        "</div>"
        "</body></html>");
    return html;
}

/// <summary>
/// Sauvegarde les credentials en Preferences
/// </summary>
void ROMV2_WIFI::SaveCredentials(const String& ssid, const String& password, const String& brokerIp, int brokerPort)
{
    Preferences prefs;

    // Wifi
    prefs.begin(PREF_WIFI_NAMESPACE, false);
    prefs.putString(PREF_WIFI_KEY_SSID, ssid);
    prefs.putString(PREF_WIFI_KEY_PWD, password);
    prefs.end();

    // MQTT
    prefs.begin(PREF_MQTT_NAMESPACE, false);
    prefs.putString(PREF_MQTT_KEY_BROKER_IP, brokerIp);
    prefs.putInt(PREF_MQTT_KEY_BROKER_PORT, brokerPort);
    prefs.end();

    // Sauvegarde des données - Ecriture des valeures sous Mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        strncpy(_ssid, ssid.c_str(), sizeof(_ssid) - 1);
        strncpy(_password, password.c_str(), sizeof(_password) - 1);
        strncpy(_brokerIp, brokerIp.c_str(), sizeof(_brokerIp) - 1);
        _brokerPort = brokerPort;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }

    // Trace
    debugln(F(""));
    debugln(F("[WIFI] SaveCredentials"));
    debug(F("[WIFI] SSID : "));
    debugln(ssid);
    debug(F("[WIFI] Password : "));
    debugln(password);
    debug(F("[WIFI] MQTT Broker Ip : "));
    debugln(brokerIp);
    debug(F("[WIFI] MQTT Broker Port : "));
    debugln(brokerPort);
}

/// <summary>
/// Charge les credentials depuis les Preferences
/// </summary>
void ROMV2_WIFI::LoadCredentials()
{
    Preferences prefs;

    // Wifi
    prefs.begin(PREF_WIFI_NAMESPACE, true);
    String newSsid = prefs.getString(PREF_WIFI_KEY_SSID, "");
    String newPassword = prefs.getString(PREF_WIFI_KEY_PWD, "");
    prefs.end();

    // MQTT
    prefs.begin(PREF_MQTT_NAMESPACE, true);
    String newBrokerIp = prefs.getString(PREF_MQTT_KEY_BROKER_IP, "");
    int newBrokerPort = prefs.getInt(PREF_MQTT_KEY_BROKER_PORT, MQTT_DEFAULT_PORT);
    prefs.end();

    // Ecriture des valeures sous Mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        strncpy(_ssid, newSsid.c_str(), sizeof(_ssid) - 1);
        strncpy(_password, newPassword.c_str(), sizeof(_password) - 1);
        strncpy(_brokerIp, newBrokerIp.c_str(), sizeof(_brokerIp) - 1);
        _brokerPort = newBrokerPort;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }

    // Trace
    debugln(F(""));
    debugln(F("[WIFI] LoadCredentials"));
    debug(F("[WIFI] SSID : "));
    debugln(newSsid);
    debug(F("[WIFI] Password : "));
    debugln(newPassword);
    debug(F("[WIFI] MQTT Broker Ip : "));
    debugln(newBrokerIp);
    debug(F("[WIFI] MQTT Broker Port : "));
    debugln(newBrokerPort);
}

/// <summary>
/// Reset les credentials en Preferences
/// </summary>
void ROMV2_WIFI::ResetCredentials()
{
    // Ecriture des données sous mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _nextAction = ACTION_WIFI_RESET_CREDENTIALS;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Reset les credentials en Preferences
/// </summary>
void ROMV2_WIFI::DoResetCredentials()
{
    // Arrêt du mode STA
    WiFi.disconnect(true);
    delay(200);

    // Reset des préférences
    Preferences prefs;

    // Wifi
    prefs.begin(PREF_WIFI_NAMESPACE, false);
    prefs.putString(PREF_WIFI_KEY_SSID, "");
    prefs.putString(PREF_WIFI_KEY_PWD, "");
    prefs.end();

    // MQTT
    prefs.begin(PREF_MQTT_NAMESPACE, false);
    prefs.putString(PREF_MQTT_KEY_BROKER_IP, "");
    prefs.putInt(PREF_MQTT_KEY_BROKER_PORT, MQTT_DEFAULT_PORT);
    prefs.end();

    // Sauvegarde des données - Ecriture des valeures sous Mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        strcpy(_ssid, "");
        strcpy(_password, "");
        strcpy(_brokerIp, "");
        _brokerPort = MQTT_DEFAULT_PORT;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }

    // Trace
    debugln(F(""));
    debugln(F("[WIFI] Reset Credentials effectue."));

    // On se remet en mode STA
    DoStartSTA();
}

/// <summary>
/// Reset de la variable locale _configReceived
/// </summary>
void ROMV2_WIFI::FlushConfigReceived()
{
    // Sauvegarde des données - Ecriture des valeures sous Mutex
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        _configReceived = false;

        // On libère le mutex
        xSemaphoreGive(_mutex);
    }
}

/// <summary>
/// Décode une chaîne encodée URL (%XX et + ? espace)
/// </summary>
String ROMV2_WIFI::UrlDecode(const String& encoded)
{
    String decoded = "";
    char temp[3] = { 0, 0, 0 };
    for (int i = 0; i < (int)encoded.length(); i++)
    {
        if (encoded[i] == '%' && i + 2 < (int)encoded.length())
        {
            temp[0] = encoded[i + 1];
            temp[1] = encoded[i + 2];
            decoded += (char)strtol(temp, nullptr, 16);
            i += 2;
        }
        else if (encoded[i] == '+')
        {
            decoded += ' ';
        }
        else
        {
            decoded += encoded[i];
        }
    }
    return decoded;
}
