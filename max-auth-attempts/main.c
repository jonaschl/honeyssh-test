#include <string.h>
#include <jansson.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libssh2.h>
#include <unistd.h>

#define CONFIG_PATH "/etc/honeyssh-test/config.json"


int get_integer_from_json(json_t *json, char *key, int *integer){
  json_t *value;
  value = json_object_get(json, key);
  if(value == NULL){
    return -1;
  }
  *integer = json_integer_value(value);
  return 0;
}

int get_string_from_json(json_t *json, char *key, const char **string){

  const json_t *value;
  value = json_object_get(json, key);
  if(value == NULL){
    return -1;
  }
  *string = json_string_value(value);
  if(*string == NULL){
    return -1;
  }
return 0;
}

int main(){


  json_t *conf;
  json_error_t *error;
  //lese Konfiguration ein
  conf = json_load_file(CONFIG_PATH, 0, error);

  if (conf == NULL){
    printf("Konnte Konfigurations Datei  %s nicht öffnen\n", CONFIG_PATH );
    //gebe Fehler Informationen aus
    ///char *tmp;
    //tmp = json_dumps(error, 0);
    //printf("%s\n", tmp);
    //free(tmp);
    return 1;
  }

  int port;
  get_integer_from_json(conf, "port", &port);
  const char *host;
  get_string_from_json(conf, "host", &host);
  int max_auth;
  get_integer_from_json(conf, "max-auth-attempts", &max_auth);
  const char *username;
  get_string_from_json(conf, "username", &username);


  int r;
  //initialisiere die libssh2 Bibliothek
  r = libssh2_init(0);
  if(r){
    printf("Konnte libssh2 Bibliothek nicht initialisieren\n");
    return 1;
  }

  // Erstelle den nötigen Netzwerk socket

 struct sockaddr_in addre;
 int socke;

 socke = socket(AF_INET, SOCK_STREAM, 0);

 if(socke == -1){
   printf("Konnte den benötigten Socket nicht erstellen\n");
   return 1;
 }
 // Setze die nötigen Optionen für die Verbindung
 addre.sin_family = AF_INET;
 addre.sin_port = htons(port);
 addre.sin_addr.s_addr = inet_addr(host);

 //verbinde
 if (connect(socke, (struct sockaddr*)(&addre), sizeof(struct sockaddr_in)) != 0) {
	  printf("Konnte nicht verbinden\n");
    return 1;
  }
  // Erstelle die SSH Session
  LIBSSH2_SESSION *session;
  session = libssh2_session_init();

  if(!session){
    printf("Konnte SSH Session nicht erstellen\n");
  }
  //setze die Session non blocking (Wir warten nicht bis Daten ankommen)
  //libssh2_session_set_blocking(session, 0);

  //Verbinde mit dem Server
  r = libssh2_session_handshake(session, socke);

  if (r){
    printf("SSH Verbindung fehlgeschlagen\n");
    return 1;
  }

  // Versuche die Authentifizierung
  int i = 1;
  int n = 0;
  char pass[10] = "";

  do {
        // generiere eine Zufallszahl als Passwort (im Bereich von 1 bis 100)
    n = rand() % 100 + 1;
    sprintf(pass,"%d",n);
    r = libssh2_userauth_password(session, username, pass);
    //printf("%d\n", r);
    if (r == -18){
      printf("Authentifizierung nicht erfolgreich\n");
    }
    if (!r)
    {
      printf("Authentifizierung erfolgreich\n");
      break;
    }
    i++;

  } while((i <= max_auth) && (r == LIBSSH2_ERROR_AUTHENTICATION_FAILED));

  // Ein Versuch ist der der die Schleife abbricht und wir starten bei eins
  i = i-2;
  printf("%d Versuche waren möglich\n", i);



  // Beende die Verbindung
  if (libssh2_session_disconnect(session, "Normales Ende")){
    printf("Ende der Session nicht erfolgreich\n");
  }
  else
  {
    printf("Session erfolgreich beendet\n");
  }

  // Gebe den Speicherplatz von session wieder frei
	libssh2_session_free(session);

  // schließe den Socket
	close(socke);

  // räume alle nciht mehr nötigen Struckturen der Bibliothek auf
	libssh2_exit();

  return 0;

}
