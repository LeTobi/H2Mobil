# H2-Mobil
Das Projekt besteht aus 3 Komponenten, welche zusammen ein Fernsteuerbares Gerät mit Webcam steuert. Basiert auf tobilib

## Prinzip
Zuschauer: optional Passwortgeschützt, können Video erhalten
Captain: optional Passwortgeschützt, steuert das Fahrzeug
Admins: Passwortgeschützt, verwalten Passwörter und Verbindungen

## Komponenten
### mobil
Programm läuft auf dem mobilen Gerät. Basiert auf einem Websocket-Client
### client
html, javascript für die Steuerung des Geräts
### relay-server
Websocket-Server, zuständig für handhabung von Verbindungen, Admins, Zuschauern und Videoweiterleitung.