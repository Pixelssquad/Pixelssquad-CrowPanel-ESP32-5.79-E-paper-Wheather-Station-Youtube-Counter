#include <WiFiClientSecure.h>
#include <YoutubeApi.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "EPD.h"
#include "pic.h"
#include "picyt.h"

// Define the black and white image array as the buffer for the e-paper display
uint8_t ImageBW[27200];  // Define the buffer size according to the resolution of the e-paper display
const char* ssid = "OOZTEST";       // WiFi network name
const char* password = "xxxxxxxxxxx";     // WiFi password

//Youtube api and channel id
#define API_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  // your google apps API Token
#define CHANNEL_ID "UCm3gzmmPBf8AwbEvsANWBBg" // makes up the url of channel
WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

// OpenWeatherMap API key
String openWeatherMapApiKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
// Example API key: String openWeatherMapApiKey = "bd939aa3d23ff33d3c8f5dd1dd435";

// Replace with the actual city and country code
String city = "Cassolnovo";                    // City name
String countryCode = "3179778";            // Country code of the city

// Default timer set to 10 seconds for testing
// For the final application, set an appropriate time interval based on the hourly/minute API call limits
unsigned long lastTime = 0;                // Last update time
unsigned long timerDelay = 10000;          // Timer set to 10 seconds (10000)

// Define variables related to JSON data
String jsonBuffer;
int httpResponseCode;
JSONVar myObject;

// Define variables related to weather information
String weather;
String temperature;
String humidity;
String sea_level;
String wind_speed;
String city_js;
int weather_flag = 0;                     // Weather icon flag


//String subs;



// Function to display weather forecast
void UI_weather_forecast()
{
  char buffer[40];  // Create a character array to store information

  // Clear the image and initialize the e-ink screen
  Paint_NewImage(ImageBW, EPD_W, EPD_H, Rotation, WHITE); // Create a new image
  Paint_Clear(WHITE); // Clear the image content
  EPD_FastMode1Init(); // Initialize the e-ink screen
  EPD_Display_Clear(); // Clear the screen display
  EPD_Update(); // Update the screen
  EPD_Clear_R26A6H(); // Clear the e-ink screen cache

  // Display the image
  EPD_ShowPicture(0, 0, 792, 272, pic, WHITE); // Display the background image
  // Display YT logo
  EPD_ShowPicture(250, 4, 200, 47, ytlogo, BLACK); // Display the background image

  // Display the corresponding weather icon based on the weather icon flag
  EPD_ShowPicture(4, 3, 432, 184, Weather_Num[weather_flag], WHITE);

  // Draw partition lines
  EPD_DrawLine(0, 190, 792, 190, BLACK); // Draw a horizontal line
  EPD_DrawLine(530, 0, 530, 270, BLACK); // Draw a vertical line
  EPD_DrawLine(230, 0, 230, 270, BLACK); // Draw a vertical line

  // Display the update time
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s ", city_js); // Format the update time as a string
  EPD_ShowString(620, 60, buffer, 24, BLACK); // Display the update time

  // Display the temperature
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s C", temperature); // Format the temperature as a string
  EPD_ShowString(340, 240, buffer, 24, BLACK); // Display the temperature

  // Display the humidity
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s %", humidity); // Format the humidity as a string
  EPD_ShowString(620, 150, buffer, 24, BLACK); // Display the humidity

  // Display the wind speed
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s m/s", wind_speed); // Format the wind speed as a string
  EPD_ShowString(90, 240, buffer, 24, BLACK); // Display the wind speed

  // Display the sea level pressure
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s hPa", sea_level); // Format the sea level pressure as a string
  EPD_ShowString(620, 240, buffer, 24, BLACK); // Display the sea level pressure



const char* subs = "Subscribers";  //text to display
const char* chaname = "Diegooz";  //text to display
uint16_t subs2 = api.channelStats.subscriberCount;  //number of subs from yt api
uint16_t subsnchar = (sizeof(api.channelStats.subscriberCount)+1);  //count number of chars in subs number

EPD_ShowString(345, 70, chaname, 24, BLACK);  
EPD_ShowString(325, 110, subs, 24, BLACK);

EPD_ShowNum(330, 134, subs2, subsnchar, 48, BLACK);
  // Display yt subs
  // memset(buffer, 0, sizeof(buffer));
  // snprintf(buffer, sizeof(buffer), "%s ", subs); // Format the sea level pressure as a string
  // EPD_ShowString(1, 1, buffer, 24, BLACK); // Display the sea level pressure


  // Display YT logo
  EPD_ShowPicture(280, 4, 200, 46, ytlogo, BLACK); // Display the background image


  // Update the e-ink screen display content
  EPD_Display(ImageBW); // Display the image
  EPD_PartUpdate(); // Partially update the screen
  EPD_DeepSleep(); // Enter deep sleep mode
}

void setup() {
  Serial.begin(115200); // Initialize the serial port

  // Connect to the WiFi network
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); // Print the IP address after successful connection

  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  // Set the screen power pin to output mode and set it to high level to turn on the power
  pinMode(7, OUTPUT);  // Set GPIO 7 to output mode
  digitalWrite(7, HIGH);  // Set GPIO 7 to high level to turn on the power

  // Initialize the e-paper display
  EPD_GPIOInit();  // Initialize the GPIO pins of the e-paper

  client.setInsecure();  //fix error innewer board version in ide
}

void loop() {

   if(api.getChannelStatistics(CHANNEL_ID))
    {
      Serial.println("---------Stats---------");
      Serial.print("Subscriber Count: ");
      Serial.println(api.channelStats.subscriberCount);
      Serial.print("View Count: ");
      Serial.println(api.channelStats.viewCount);
      Serial.print("Comment Count: ");
      Serial.println(api.channelStats.commentCount);
      Serial.print("Video Count: ");
      Serial.println(api.channelStats.videoCount);
      // Probably not needed :)
      //Serial.print("hiddenSubscriberCount: ");
      //Serial.println(api.channelStats.hiddenSubscriberCount);
      Serial.println("------------------------");


  js_analysis();   // Parse weather data
  UI_weather_forecast(); // Update weather display
    }
  delay(1000 * 60 * 60); // Main loop delay for 1 hour
}

void js_analysis()
{
  // Check if successfully connected to the WiFi network
  if (WiFi.status() == WL_CONNECTED) {
    // Build the OpenWeatherMap API request URL
    String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";

    // Loop until a valid HTTP response code of 200 is obtained
    while (httpResponseCode != 200) {
      // Send an HTTP GET request and get the response content
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer); // Print the obtained JSON data
      myObject = JSON.parse(jsonBuffer); // Parse the JSON data

      // Check if JSON parsing was successful
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!"); // Error message when parsing fails
        return; // Exit the function if parsing fails
      }
      delay(2000); // Wait for 2 seconds before retrying
    }

    // Extract weather information from the parsed JSON data
    weather = JSON.stringify(myObject["weather"][0]["main"]); // Weather main information
    temperature = JSON.stringify(myObject["main"]["temp"]); // Temperature
    humidity = JSON.stringify(myObject["main"]["humidity"]); // Humidity
    sea_level = JSON.stringify(myObject["main"]["sea_level"]); // Sea level pressure
    wind_speed = JSON.stringify(myObject["wind"]["speed"]); // Wind speed
    city_js = JSON.stringify(myObject["name"]); // City name

    // Print the extracted weather information
    Serial.print("String weather: ");
    Serial.println(weather);
    Serial.print("String Temperature: ");
    Serial.println(temperature);
    Serial.print("String humidity: ");
    Serial.println(humidity);
    Serial.print("String sea_level: ");
    Serial.println(sea_level);
    Serial.print("String wind_speed: ");
    Serial.println(wind_speed);
    Serial.print("String city_js: ");
    Serial.println(city_js);

    // Set the weather icon flag based on the weather description
    if (weather.indexOf("clouds") != -1 || weather.indexOf("Clouds") != -1) {
      weather_flag = 1; // Cloudy
 //   } else if (weather.indexOf("clear sky") != -1 || weather.indexOf("Clear sky") != -1) {
    } else if (weather.indexOf("clear") != -1 || weather.indexOf("Clear") != -1) {      
      weather_flag = 3; // Clear sky
    } else if (weather.indexOf("rain") != -1 || weather.indexOf("Rain") != -1) {
      weather_flag = 5; // Rainy
    } else if (weather.indexOf("thunderstorm") != -1 || weather.indexOf("Thunderstorm") != -1) {
      weather_flag = 2; // Thunderstorm
    } else if (weather.indexOf("snow") != -1 || weather.indexOf("Snow") != -1) {
      weather_flag = 4; // Snowy
    } else if (weather.indexOf("mist") != -1 || weather.indexOf("Mist") != -1) {
      weather_flag = 0; // Foggy
    }
  }
  else {
    // Print a message if the WiFi connection is lost
    Serial.println("WiFi Disconnected");
  }
}

// Define the HTTP GET request function
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Initialize the HTTP client and specify the requested server URL
  http.begin(client, serverName);

  // Send an HTTP GET request
  httpResponseCode = http.GET();

  // Initialize the returned response content
  String payload = "{}";

  // Check the response code and process the response content
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode); // Print the response code
    payload = http.getString(); // Get the response content
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode); // Print the error code
  }
  // Release the HTTP client resources
  http.end();

  return payload; // Return the response content
}