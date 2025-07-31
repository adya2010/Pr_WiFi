#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "IOT"
#define WIFI_PASSWORD "1234567890"
#define BOT_TOKEN "8045707524:AAFsEOr2QyOo0fxwjnv1ldbnpMSKm05kD_M"
#define BOT_MTBS 1000  // Time between two bot messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

unsigned long bot_lasttime;
String message;
String text, chat_id;
String chatid = "-4629083771";

// Variables for accident conditions
bool accident = false;

void handleNewMessages(int numNewMessages) {
  Serial.println("Handling new messages...");
  for (int i = 0; i < numNewMessages; i++) {
    chat_id = bot.messages[i].chat_id;
    text = bot.messages[i].text;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // Set pin modes for sensors
  pinMode(22, INPUT_PULLUP);

  // Connecting to WiFi
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected. IP address: " + WiFi.localIP().toString());

  // Retrieve time from NTP server
  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org");  // Get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop() {
  // Read sensor inputs and update accident flags
  accident = digitalRead(22);

  // Send alert messages based on detected accidents
  if (accident == LOW) {
    bot.sendMessage(chatid, "HELLO I'AM HEAR", "");
  }
  // Handle incoming messages from the Telegram bot
  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("Received response from Telegram Bot");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}