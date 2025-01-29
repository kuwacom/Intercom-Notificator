#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "secret.h"

// WiFi 設定
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// Discoed WEBHook
const char *discordWebhookUrl = DISCORD_WEBHOOK_URL;

// スイッチのピン
// G26とかG36でもいいかも
// グランドと繋ぐ
#define SWITCH_PIN 32

void setup()
{
    M5.begin();                        // M5StickC の初期化
    pinMode(SWITCH_PIN, INPUT_PULLUP); // スイッチピンをプルアップモードで設定

    M5.Lcd.setRotation(3);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 0);

    // Wi-Fi 接続
    WiFi.begin(ssid, password);
    M5.Lcd.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.println("\nWiFi Connected!!");
    M5.Lcd.println(WiFi.localIP()); // IPアドレスを表示

    delay(2000);
    waitingMessage();
}

void loop()
{
    static bool lastSwitchState = HIGH;         // スイッチの状態を記録
    bool switchState = digitalRead(SWITCH_PIN); // スイッチの現在の状態を取得

    // スイッチが押された瞬間
    if (switchState == LOW && lastSwitchState == HIGH)
    {
        M5.Lcd.fillScreen(GREEN); // 画面リセット
        M5.Lcd.setCursor(0, 0);

        M5.Lcd.setTextSize(2);
        M5.Lcd.setTextColor(RED, BLACK);
        M5.Lcd.println("Wellcome");

        M5.Lcd.setTextSize(1);
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.println("Sending Message...");

        // Discord にメッセージ送信
        sendDiscordMessage("<@664814874328563712>\\n🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔\\n🔔 誰かがインターホンを鳴らしています！ 🔔\\n🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔🔔");
    }
    else if (switchState == HIGH && lastSwitchState == LOW)
    {
        waitingMessage();
    }
    lastSwitchState = switchState; // 前回のスイッチ状態を更新

    delay(100); // チャタリング防止
}

void waitingMessage()
{
    M5.Lcd.fillScreen(BLACK); // 画面をクリア
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.println("Waiting...");
}

void sendDiscordMessage(String message)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        // Webhook URL に接続
        http.begin(discordWebhookUrl);
        http.addHeader("Content-Type", "application/json");

        // Discord に送るメッセージのフォーマット
        String payload = "{\"content\":\"" + message + "\"}";
        // HTTP POST リクエスト
        int httpResponseCode = http.POST(payload);

        M5.Lcd.setTextColor(WHITE, BLACK);
        if (httpResponseCode > 0)
        {
            M5.Lcd.println("Message Sent!");
            M5.Lcd.print("HTTP Code: ");
            M5.Lcd.println(httpResponseCode);
        }
        else
        {
            M5.Lcd.println("Failed to send message");
            M5.Lcd.print("HTTP Code: ");
            M5.Lcd.println(httpResponseCode);
        }
        http.end(); // 接続終了
    }
    else
    {
        M5.Lcd.println("WiFi Not Connected!");
    }
}