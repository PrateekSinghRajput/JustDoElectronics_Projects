#include <PZEM004Tv30.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP.h>

// Define pins for PZEM
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17

// TFT display pins
#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4
#define TFT_MOSI 23
#define TFT_SCLK 18

// Relay pin for power control
#define RELAY_PIN 14
// Push button pin for page change and manual relay override
#define BUTTON_PIN 32

#define EEPROM_SIZE 128
// Increased EEPROM size to accommodate more settings
#define BALANCE_ADDR 0
#define ENERGY_ADDR 4
#define OVER_VOLTAGE_THRESHOLD_ADDR 8
#define OVER_CURRENT_THRESHOLD_ADDR 12
#define THEFT_CURRENT_THRESHOLD_ADDR 16
#define MINIMUM_BALANCE_ADDR 20
#define COST_PER_KWH_ADDR 24  // Corrected definition - retyped this line

// Default values for thresholds (used if EEPROM is empty or corrupted)
#define DEFAULT_OVER_VOLTAGE_THRESHOLD 260.0
#define DEFAULT_OVER_CURRENT_THRESHOLD 10.0
#define DEFAULT_THEFT_CURRENT_THRESHOLD 0.02
#define DEFAULT_MINIMUM_BALANCE 0.0
#define DEFAULT_COST_PER_KWH 0.2
#define DEFAULT_BALANCE 0.0
#define DEFAULT_ENERGY 0.0

// Authentication credentials for web interface
const char* admin_username = "admin";
const char* admin_password = "admin123";

// Server port
WebServer server(80);

// Colors (Added missing colors if needed)
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define DARKGRAY 0x4208
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define ORANGE 0xFD20

// Text buffers to track changes and avoid unnecessary redraws
char lastVoltageText[20] = "";  // Increased buffer size to 20
char lastCurrentText[20] = "";
char lastPowerText[20] = "";
char lastEnergyText[20] = "";
char lastBalanceText[20] = "";
char lastStatusText[20] = "";
char lastFaultText[20] = "";
char lastTheftText[20] = "";

// Common text settings - Function to reduce repetition
void setTextProperties(Adafruit_ST7735& display, uint16_t color, uint8_t size = 1) {  // Modified to accept tft display object
  display.setTextColor(color);
  display.setTextSize(size);
}

// Global variables
unsigned long measurementInterval = 2000;  // Interval between PZEM readings (milliseconds)
unsigned long lastReadTime = 0;
unsigned long screenRefreshTime = 0;
unsigned long balanceUpdateTime = 0;
unsigned long lastEnergyValue = 0;
const unsigned long SCREEN_REFRESH_INTERVAL = 1000;   // Interval for screen refresh (milliseconds)
const unsigned long BALANCE_UPDATE_INTERVAL = 60000;  // Update balance every minute
bool layoutDrawn = false;
unsigned long notificationEndTime = 0;  //used for determine the end of the notification
String currentNotification = "";        // string to  current displaying notification
uint16_t notificationColor = WHITE;
bool pzemConnected = true;                 // Flag to track PZEM connection status
bool relayState = true;                    // Flag to track relay state
float balance = 0.0;                       // Current balance in rupees
float lastEnergy = 0.0;                    // Last energy reading
bool faultDetected = false;                // Flag for fault detection
bool theftDetected = false;                // Flag for theft detection
int displayPage = 0;                       // Current display page (0=main, 1=alerts)
unsigned long pageChangeTime = 0;          // Time to switch between pages
const unsigned long PAGE_INTERVAL = 5000;  // Switch pages every 5 seconds
bool manualOverride = false;               // Flag for manual relay control override

// Thresholds - now variables, loaded from EEPROM and configurable
float overVoltageThreshold = DEFAULT_OVER_VOLTAGE_THRESHOLD;
float overCurrentThreshold = DEFAULT_OVER_CURRENT_THRESHOLD;
float theftCurrentThreshold = DEFAULT_THEFT_CURRENT_THRESHOLD;
float minimumBalance = DEFAULT_MINIMUM_BALANCE;
float costPerKWh = DEFAULT_COST_PER_KWH;

// Initialize TFT and PZEM
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
PZEM004Tv30 pzem(Serial2, PZEM_RX_PIN, PZEM_TX_PIN);

// Structure for PZEM readings
struct PzemData {
  float voltage;
  float current;
  float power;
  float energy;
  bool isValid;
};

// Store readings
PzemData readings = { 0, 0, 0, 0, false };

// Store IP address as a string to avoid repeated calls
String ipAddressStr = "";

// WiFi credentials - Replace with your actual credentials
const char* ssid = "Prateek";
const char* password = "12345@#12345";

// Button debounce variables
const unsigned long debounceDelay = 50;  // Debounce delay in milliseconds
unsigned long lastButtonPress = 0;
bool buttonState = false;      // Current button state
bool lastButtonState = false;  // Previous button state

// Notification timer interval (5 minutes)
const unsigned long NOTIFICATION_INTERVAL = 300000;  // 5 minutes in milliseconds
unsigned long lastNotificationTime = 0;

// WiFi reconnection variables
unsigned long lastWifiCheckTime = 0;
const unsigned long WIFI_RECHECK_INTERVAL = 30000;  // Check WiFi every 30 seconds

// Manual relay override button variables
unsigned long buttonPressStartTime = 0;
bool manualRelayControlActive = false;

// Function prototypes
void handleRoot();
void handleLogin();
void handleRecharge();
void handleData();
void handleReset();
void handleSetRelay();
void handleNotFound();
void readPzemData();
void drawLayout();
void drawLiveDataPage();
void drawAlertsPage();
void updateDisplayValues();
void showNotification(const String& message, uint16_t color, unsigned long duration = 5000);  //default 5 sec notification
void clearNotification();
void displayStartupSequence();
void configureWiFi();  // Modified to remove WiFiManager
void saveBalance();
void loadBalance();
void updateBalance();
void checkFaultsAndTheft();
void controlRelay();
void printDebugInfo(const char* action, bool success);
void handleSetThresholds();       // New handler for setting thresholds
void saveThresholds();            // Function to save thresholds to EEPROM
void loadThresholds();            // Function to load thresholds from EEPROM
String generateThresholdsHTML();  // Function to generate HTML for threshold settings section
void handleFactoryReset();        // New handler for factory reset
void resetEEPROMToDefaults();     // Function to reset EEPROM to defaults
void checkWiFiConnection();       // Function to check and reconnect WiFi

// Helper function to clear a text area on TFT
void clearTextArea(int x, int y, int w, int h) {
  tft.fillRect(x, y, w, h, BLACK);
}

// HTML page stored in PROGMEM (Improved Web Dashboard UI/UX) - Modified to include Factory Reset Button
const char index_html[] PROGMEM = R"rawliteral(
`
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Prepaid Energy Monitor Pro</title>
    <style>
        /* Modern Professional Color Palette */
        :root {
            --primary: #0a2463;
            --secondary: #3e92cc;
            --accent: #2dc653;
            --warning: #ffb400;
            --danger: #d64045;
            --dark: #1e1f26;
            --light: #f8f9fa;
            --gray-100: #f0f2f5;
            --gray-200: #e9ecef;
            --gray-300: #dee2e6;
            --gray-400: #ced4da;
            --gray-500: #adb5bd;
            --gray-600: #6c757d;
            --gray-700: #495057;
            --gray-800: #343a40;
            --text: #212529;
            --box-shadow: 0 2px 8px rgba(0, 0, 0, 0.12);
            --card-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            --transition: all 0.2s ease;
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Roboto', 'Segoe UI', sans-serif;
            background-color: var(--gray-100);
            color: var(--text);
            line-height: 1.6;
        }

        .dashboard-container {
            display: grid;
            grid-template-columns: 240px 1fr;
            min-height: 100vh;
        }

        /* Sidebar */
        .sidebar {
            background-color: var(--primary);
            color: white;
            padding-top: 20px;
            position: fixed;
            height: 100vh;
            width: 240px;
            z-index: 100;
            box-shadow: var(--box-shadow);
        }

        .brand {
            display: flex;
            align-items: center;
            padding: 0 20px 20px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }

        .brand-icon {
            font-size: 24px;
            margin-right: 10px;
        }

        .brand-name {
            font-size: 18px;
            font-weight: 600;
        }

        .nav-menu {
            list-style: none;
            padding: 20px 0;
        }

        .nav-item {
            padding: 0 15px;
            margin-bottom: 5px;
        }

        .nav-link {
            display: flex;
            align-items: center;
            padding: 12px 15px;
            color: rgba(255, 255, 255, 0.8);
            text-decoration: none;
            border-radius: 5px;
            transition: var(--transition);
        }

        .nav-link:hover {
            background-color: rgba(255, 255, 255, 0.1);
            color: white;
        }

        .nav-link.active {
            background-color: var(--secondary);
            color: white;
        }

        .nav-icon {
            margin-right: 10px;
            width: 20px;
            text-align: center;
        }

        /* Main Content */
        .main-content {
            grid-column: 2;
            padding: 20px;
        }

        .page-header {
            padding: 15px 0;
            margin-bottom: 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .page-title {
            font-size: 24px;
            font-weight: 500;
            color: var(--dark);
        }

        .page-subtitle {
            font-size: 14px;
            color: var(--gray-600);
            margin-top: 5px;
        }

        /* Cards & Grids */
        .card-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
            gap: 20px;
            margin-bottom: 25px;
        }

        .card {
            background-color: white;
            border-radius: 10px;
            box-shadow: var(--card-shadow);
            padding: 20px;
            transition: var(--transition);
        }

        .card:hover {
            box-shadow: 0 10px 15px rgba(0, 0, 0, 0.07);
            transform: translateY(-2px);
        }

        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 15px;
        }

        .card-title {
            font-size: 16px;
            font-weight: 500;
            color: var(--gray-700);
        }

        .card-icon {
            font-size: 18px;
            width: 36px;
            height: 36px;
            display: flex;
            align-items: center;
            justify-content: center;
            border-radius: 50%;
            background-color: var(--secondary);
            color: white;
        }

        .dashboard-value {
            font-size: 32px;
            font-weight: 600;
            color: var(--primary);
            margin: 10px 0;
        }

        .unit {
            font-size: 16px;
            font-weight: normal;
            color: var(--gray-600);
        }

        .trend {
            display: flex;
            align-items: center;
            font-size: 14px;
            color: var(--gray-600);
        }

        .trend-up {
            color: var(--accent);
        }

        .trend-down {
            color: var(--danger);
        }

        /* Status Indicators */
        .status-card {
            display: flex;
            flex-direction: column;
            height: 100%;
        }

        .status-indicators {
            flex-grow: 1;
        }

        .status-item {
            display: flex;
            align-items: center;
            padding: 12px 0;
            border-bottom: 1px solid var(--gray-200);
        }

        .status-item:last-child {
            border-bottom: none;
        }

        .status-indicator {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 12px;
        }

        .status-active {
            background-color: var(--accent);
        }

        .status-inactive {
            background-color: var(--gray-500);
        }

        .status-warning {
            background-color: var(--warning);
        }

        .status-danger {
            background-color: var(--danger);
        }

        .status-label {
            font-weight: 500;
            flex-grow: 1;
        }

        .status-value {
            font-weight: 600;
        }

        /* Action Areas */
        .action-card {
            height: 100%;
        }

        .action-content {
            padding-top: 10px;
        }

        .input-group {
            display: flex;
            margin-bottom: 15px;
        }

        .input-group .form-input {
            flex-grow: 1;
            border-top-right-radius: 0;
            border-bottom-right-radius: 0;
        }

        .input-group .input-append {
            display: flex;
            align-items: center;
            padding: 0 12px;
            background-color: var(--gray-200);
            border: 1px solid var(--gray-300);
            border-left: none;
            border-top-right-radius: 5px;
            border-bottom-right-radius: 5px;
            color: var(--gray-700);
        }

        /* Forms */
        .form-group {
            margin-bottom: 20px;
        }

        .form-label {
            display: block;
            margin-bottom: 8px;
            font-weight: 500;
            color: var(--gray-700);
        }

        .form-input {
            width: 100%;
            padding: 10px 12px;
            border: 1px solid var(--gray-300);
            border-radius: 5px;
            font-size: 14px;
            transition: var(--transition);
        }

        .form-input:focus {
            outline: none;
            border-color: var(--secondary);
            box-shadow: 0 0 0 3px rgba(62, 146, 204, 0.1);
        }

        /* Buttons */
        .btn {
            display: inline-block;
            font-weight: 500;
            text-align: center;
            vertical-align: middle;
            cursor: pointer;
            border: 1px solid transparent;
            padding: 10px 16px;
            font-size: 14px;
            line-height: 1.5;
            border-radius: 5px;
            transition: var(--transition);
        }

        .btn-primary {
            color: white;
            background-color: var(--secondary);
            border-color: var(--secondary);
        }

        .btn-primary:hover {
            background-color: #3182b8;
            border-color: #3182b8;
        }

        .btn-success {
            color: white;
            background-color: var(--accent);
            border-color: var(--accent);
        }

        .btn-success:hover {
            background-color: #25a745;
            border-color: #25a745;
        }

        .btn-danger {
            color: white;
            background-color: var(--danger);
            border-color: var(--danger);
        }

        .btn-danger:hover {
            background-color: #c13c40;
            border-color: #c13c40;
        }

        .btn-block {
            display: block;
            width: 100%;
        }

        .btn:disabled {
            opacity: 0.65;
            pointer-events: none;
        }

        /* Alerts */
        .alert {
            padding: 12px 15px;
            margin-bottom: 15px;
            border-radius: 5px;
            border: 1px solid transparent;
            font-size: 14px;
        }

        .alert-success {
            color: #155724;
            background-color: #d4edda;
            border-color: #c3e6cb;
        }

        .alert-danger {
            color: #721c24;
            background-color: #f8d7da;
            border-color: #f5c6cb;
        }

        .alert-warning {
            color: #856404;
            background-color: #fff3cd;
            border-color: #ffeeba;
        }

        /* Settings */
        .settings-section {
            margin-bottom: 25px;
        }

        .settings-header {
            display: flex;
            align-items: center;
            margin-bottom: 15px;
            padding-bottom: 10px;
            border-bottom: 1px solid var(--gray-300);
        }

        .settings-icon {
            margin-right: 10px;
            color: var(--secondary);
        }

        .settings-title {
            font-size: 18px;
            font-weight: 500;
        }

        .settings-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
            gap: 20px;
        }

        /* Hide sections */
        .page-section {
            display: none;
        }

        .page-section.active {
            display: block;
        }

        /* Footer */
        .dashboard-footer {
            margin-top: 30px;
            padding-top: 15px;
            border-top: 1px solid var(--gray-300);
            color: var(--gray-600);
            font-size: 14px;
            text-align: center;
        }

        /* Responsive */
        @media (max-width: 992px) {
            .dashboard-container {
                grid-template-columns: 1fr;
            }
            .sidebar {
                display: none;
            }
            .main-content {
                grid-column: 1;
            }
        }
    </style>
</head>
<body>
    <div class="dashboard-container">
        <!-- Sidebar Navigation -->
        <div class="sidebar">
            <div class="brand">
                <div class="brand-icon">⚡</div>
                <div class="brand-name">EnergyPro</div>
            </div>
            <ul class="nav-menu">
                <li class="nav-item">
                    <a href="#dashboard" class="nav-link active" onclick="showPage('dashboard')">
                        <span class="nav-icon">📊</span>
                        <span>Dashboard</span>
                    </a>
                </li>
                <li class="nav-item">
                    <a href="#account" class="nav-link" onclick="showPage('account')">
                        <span class="nav-icon">💰</span>
                        <span>Account</span>
                    </a>
                </li>
                <li class="nav-item">
                    <a href="#control" class="nav-link" onclick="showPage('control')">
                        <span class="nav-icon">🎮</span>
                        <span>Control</span>
                    </a>
                </li>
                <li class="nav-item">
                    <a href="#settings" class="nav-link" onclick="showPage('settings')">
                        <span class="nav-icon">⚙️</span>
                        <span>Settings</span>
                    </a>
                </li>
                <li class="nav-item">
                    <a href="#reports" class="nav-link" onclick="showPage('reports')">
                        <span class="nav-icon">📈</span>
                        <span>Reports</span>
                    </a>
                </li>
                <li class="nav-item">
                    <a href="#help" class="nav-link" onclick="showPage('help')">
                        <span class="nav-icon">❓</span>
                        <span>Help</span>
                    </a>
                </li>
            </ul>
        </div>

        <!-- Main Content Area -->
        <div class="main-content">
            <!-- Dashboard Section -->
            <div id="dashboard-section" class="page-section active">
                <div class="page-header">
                    <div>
                        <h1 class="page-title">Energy Dashboard</h1>
                        <p class="page-subtitle">Real-time monitoring of your prepaid energy system</p>
                    </div>
                    <div id="update-timestamp">Last updated: --:--:--</div>
                </div>

                <!-- Metric Cards -->
                <div class="card-grid">
                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">BALANCE</h3>
                            <div class="card-icon">₹</div>
                        </div>
                        <div class="dashboard-value" id="balance">--<span class="unit">₹</span></div>
                        <div class="trend">Available credit</div>
                    </div>

                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">VOLTAGE</h3>
                            <div class="card-icon">V</div>
                        </div>
                        <div class="dashboard-value" id="voltage">--<span class="unit">V</span></div>
                        <div class="trend">Line voltage</div>
                    </div>

                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">CURRENT</h3>
                            <div class="card-icon">A</div>
                        </div>
                        <div class="dashboard-value" id="current">--<span class="unit">A</span></div>
                        <div class="trend">Line current</div>
                    </div>

                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">POWER</h3>
                            <div class="card-icon">W</div>
                        </div>
                        <div class="dashboard-value" id="power">--<span class="unit">W</span></div>
                        <div class="trend">Active power</div>
                    </div>
                </div>

                <!-- Status and Control Cards -->
                <div class="card-grid">
                    <div class="card status-card">
                        <div class="card-header">
                            <h3 class="card-title">SYSTEM STATUS</h3>
                        </div>
                        <div class="status-indicators">
                            <div class="status-item">
                                <div class="status-indicator" id="power-indicator"></div>
                                <div class="status-label">Power</div>
                                <div class="status-value" id="power-status">--</div>
                            </div>
                            <div class="status-item">
                                <div class="status-indicator" id="fault-indicator"></div>
                                <div class="status-label">Fault Status</div>
                                <div class="status-value" id="fault-status">--</div>
                            </div>
                            <div class="status-item">
                                <div class="status-indicator" id="theft-indicator"></div>
                                <div class="status-label">Theft Detection</div>
                                <div class="status-value" id="theft-status">--</div>
                            </div>
                            <div class="status-item">
                                <div class="status-indicator status-active"></div>
                                <div class="status-label">Total Energy</div>
                                <div class="status-value" id="energy-value">-- kWh</div>
                            </div>
                        </div>
                    </div>

                    <div class="card action-card">
                        <div class="card-header">
                            <h3 class="card-title">QUICK ACTIONS</h3>
                        </div>
                        <div class="action-content">
                            <div id="power-control-alert" style="display:none;" class="alert"></div>
                            <p style="margin-bottom:15px;">Control power supply to your premises</p>
                            <div style="display:flex;gap:10px;margin-bottom:20px;">
                                <button id="power-on-btn" class="btn btn-success" onclick="setRelay(1)" style="flex:1;" disabled>Power ON</button>
                                <button id="power-off-btn" class="btn btn-danger" onclick="setRelay(0)" style="flex:1;" disabled>Power OFF</button>
                            </div>

                            <p style="margin-bottom:15px;">Quick recharge your account</p>
                            <div class="input-group">
                                <input type="number" id="quick-recharge-amount" class="form-input" value="100" min="10" step="10">
                                <div class="input-append">₹</div>
                            </div>
                            <button class="btn btn-primary btn-block" onclick="quickRecharge()">Recharge Now</button>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Account Section -->
            <div id="account-section" class="page-section">
                <div class="page-header">
                    <div>
                        <h1 class="page-title">Account Management</h1>
                        <p class="page-subtitle">Manage your prepaid account balance and transactions</p>
                    </div>
                </div>

                <div class="card-grid">
                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">ACCOUNT BALANCE</h3>
                        </div>
                        <div class="dashboard-value" id="account-balance">--<span class="unit">₹</span></div>
                        <div class="trend">Available credit</div>
                    </div>

                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">CONSUMPTION RATE</h3>
                        </div>
                        <div class="dashboard-value" id="account-balance">--<span class="unit">₹/day</span></div>
                        <div class="trend">Average daily consumption</div>
                    </div>
                </div>

                <div class="card">
                    <div class="card-header">
                        <h3 class="card-title">RECHARGE ACCOUNT</h3>
                    </div>
                    <div id="recharge-alert" style="display:none;" class="alert"></div>
                    <div class="form-group">
                        <label for="recharge-amount" class="form-label">Recharge Amount</label>
                        <div class="input-group">
                            <input type="number" id="recharge-amount" class="form-input" value="100" min="10" step="10">
                            <div class="input-append">₹</div>
                        </div>
                    </div>
                    <button class="btn btn-primary" onclick="rechargeAccount()">Process Recharge</button>
                </div>
            </div>

            <!-- Control Section -->
            <div id="control-section" class="page-section">
                <div class="page-header">
                    <div>
                        <h1 class="page-title">System Control</h1>
                        <p class="page-subtitle">Monitor and control your energy system</p>
                    </div>
                </div>

                <div class="card-grid">
                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">POWER CONTROL</h3>
                        </div>
                        <div id="relay-control-alert" style="display:none;" class="alert"></div>
                        <p style="margin:15px 0;">Current power status: <span id="relay-control-status">Loading...</span></p>
                        <div style="display:flex;gap:10px;">
                            <button id="control-power-on-btn" class="btn btn-success" onclick="setRelay(1)" style="flex:1;" disabled>Turn Power ON</button>
                            <button id="control-power-off-btn" class="btn btn-danger" onclick="setRelay(0)" style="flex:1;" disabled>Turn Power OFF</button>
                        </div>
                    </div>

                    <div class="card">
                        <div class="card-header">
                            <h3 class="card-title">SYSTEM STATUS</h3>
                        </div>
                        <div class="status-indicators">
                            <div class="status-item">
                                <div class="status-indicator" id="control-voltage-indicator"></div>
                                <div class="status-label">Voltage Status</div>
                                <div class="status-value" id="control-voltage-status">--</div>
                            </div>
                            <div class="status-item">
                                <div class="status-indicator" id="control-current-indicator"></div>
                                <div class="status-label">Current Status</div>
                                <div class="status-value" id="control-current-status">--</div>
                            </div>
                            <div class="status-item">
                                <div class="status-indicator" id="control-fault-indicator"></div>
                                <div class="status-label">Fault Status</div>
                                <div class="status-value" id="control-fault-status">--</div>
                            </div>
                            <div class="status-item">
                                <div class="status-indicator" id="control-theft-indicator"></div>
                                <div class="status-label">Theft Detection</div>
                                <div class="status-value" id="control-theft-status">--</div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Settings Section -->
            <div id="settings-section" class="page-section">
                <div class="page-header">
                    <div>
                        <h1 class="page-title">System Settings</h1>
                        <p class="page-subtitle">Configure thresholds and system parameters</p>
                    </div>
                </div>

                <div id="threshold-alert" style="display:none;" class="alert"></div>

                <form id="threshold-form">
                    <div class="card settings-section">
                        <div class="settings-header">
                            <span class="settings-icon">⚠️</span>
                            <h3 class="settings-title">Safety Thresholds</h3>
                        </div>
                        <div class="settings-grid">
                            <div class="form-group">
                                <label for="overVoltage" class="form-label">Over Voltage Threshold</label>
                                <div class="input-group">
                                    <input type="number" id="overVoltage" name="overVoltage" class="form-input" value="260" step="1">
                                    <div class="input-append">V</div>
                                </div>
                            </div>
                            <div class="form-group">
                                <label for="overCurrent" class="form-label">Over Current Threshold</label>
                                <div class="input-group">
                                    <input type="number" id="overCurrent" name="overCurrent" class="form-input" value="10.00" step="0.1">
                                    <div class="input-append">A</div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="card settings-section">
                        <div class="settings-header">
                            <span class="settings-icon">🔒</span>
                            <h3 class="settings-title">Security Settings</h3>
                        </div>
                        <div class="settings-grid">
                            <div class="form-group">
                                <label for="theftCurrent" class="form-label">Theft Detection Threshold</label>
                                <div class="input-group">
                                    <input type="number" id="theftCurrent" name="theftCurrent" class="form-input" value="0.020" step="0.001">
                                    <div class="input-append">A</div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="card settings-section">
                        <div class="settings-header">
                            <span class="settings-icon">💰</span>
                            <h3 class="settings-title">Billing Settings</h3>
                        </div>
                        <div class="settings-grid">
                            <div class="form-group">
                                <label for="minBalance" class="form-label">Minimum Balance Threshold</label>
                                <div class="input-group">
                                    <input type="number" id="minBalance" name="minBalance" class="form-input" value="0.00" step="1">
                                    <div class="input-append">₹</div>
                                </div>
                            </div>
                            <div class="form-group">
                                <label for="costPerKWh" class="form-label">Cost per kWh</label>
                                <div class="input-group">
                                    <input type="number" id="costPerKWh" name="costPerKWh" class="form-input" value="0.20" step="0.01">
                                    <div class="input-append">₹</div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <button type="button" class="btn btn-primary" onclick="setThresholds()">Save All Settings</button>
                </form>

                 <!-- Factory Reset Button -->
                <div class="card settings-section">
                    <div class="settings-header">
                        <span class="settings-icon" style="color: var(--danger);">⚠️</span>
                        <h3 class="settings-title" style="color: var(--danger);">Factory Reset</h3>
                    </div>
                    <p style="margin-top: 10px; margin-bottom: 20px;">Reset all settings to default and clear energy data. This action is irreversible.</p>
                    <button type="button" class="btn btn-danger" onclick="factoryReset()">Factory Reset</button>
                    <div id="factory-reset-alert" style="display:none;" class="alert"></div>
                </div>

            </div>

            <!-- Reports Section (placeholder) -->
            <div id="reports-section" class="page-section">
                <div class="page-header">
                    <div>
                        <h1 class="page-title">Reports & Analytics</h1>
                        <p class="page-subtitle">View energy consumption reports and analytics</p>
                    </div>
                </div>

                <div class="card">
                    <div class="card-header">
                        <h3 class="card-title">ENERGY CONSUMPTION</h3>
                    </div>
                    <p style="text-align:center;padding:40px 0;">Consumption reports and charts will be displayed here</p>
                </div>
            </div>

            <!-- Help Section (placeholder) -->
            <div id="help-section" class="page-section">
                <div class="page-header">
                    <div>
                        <h1 class="page-title">Help & Support</h1>
                        <p class="page-subtitle">Get help with using your prepaid energy monitor</p>
                    </div>
                </div>

                <div class="card">
                    <div class="card-header">
                        <h3 class="card-title">SUPPORT INFORMATION</h3>
                    </div>
                    <p style="text-align:center;padding:40px 0;">Help and support information will be displayed here</p>
                </div>
            </div>

            <div class="dashboard-footer">
                <p id="update-status">Last Update: --</p>
                <p>Prepaid Energy Monitor Pro © 2025</p>
            </div>
        </div>
    </div>

    <script>
        let relayState = false; // Track relay state on client-side

        // Show selected page section
        function showPage(page) {
            // Hide all page sections
            const pageSections = document.querySelectorAll('.page-section');
            pageSections.forEach(section => {
                section.classList.remove('active');
            });

            // Show selected page section
            document.getElementById(page + '-section').classList.add('active');

            // Update active nav link
            const navLinks = document.querySelectorAll('.nav-link');
            navLinks.forEach(link => {
                link.classList.remove('active');
            });
            document.querySelector(`.nav-link[href="#${page}"]`).classList.add('active');

            // Update content for specific pages
            if (page === 'account') {
                const dashboardBalance = document.getElementById('balance').textContent;
                document.getElementById('account-balance').textContent = dashboardBalance;
            }
        }

        function updateValues() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Dashboard Section Updates
                    updateDashboardValues(data);
                    updateSystemStatus(data);
                    updatePowerControlUI(data.relayState);

                    // Account Section Updates
                    updateAccountSection(data);

                    // Control Section Updates
                    updateControlSection(data);

                    // Update timestamp
                    const now = new Date();
                    document.getElementById('update-timestamp').textContent = `Last updated: ${now.toLocaleTimeString()}`;
                    document.getElementById('update-status').textContent = `Last Update: ${now.toLocaleTimeString()}`;

                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                    showAlert('Connection error. Retrying...', 'warning', 'main-alert');
                    document.getElementById('update-status').textContent = 'Update Failed';
                });
        }

        function updateDashboardValues(data) {
            updateElement('balance', data.balance.toFixed(2) + '<span class="unit">₹</span>');
            updateElement('voltage', data.voltage.toFixed(1) + '<span class="unit">V</span>');
            updateElement('current', data.current.toFixed(3) + '<span class="unit">A</span>');
            updateElement('power', data.power.toFixed(1) + '<span class="unit">W</span>');
            updateElement('energy-value', data.energy.toFixed(3) + ' kWh');
        }

        function updateSystemStatus(data) {
            updateStatusIndicator('power-indicator', data.relayState ? 'status-active' : 'status-inactive');
            updateElement('power-status', data.relayState ? 'ON' : 'OFF');

            updateStatusIndicator('fault-indicator', data.faultDetected ? 'status-danger' : 'status-active');
            updateElement('fault-status', data.faultDetected ? 'FAULT' : 'OK');

            updateStatusIndicator('theft-indicator', data.theftDetected ? 'status-warning' : 'status-active');
            updateElement('theft-status', data.theftDetected ? 'DETECTED' : 'OK');
        }

        function updatePowerControlUI(serverRelayState) {
            relayState = serverRelayState;
            const onButtonDashboard = document.getElementById('power-on-btn');
            const offButtonDashboard = document.getElementById('power-off-btn');
            const onButtonControl = document.getElementById('control-power-on-btn');
            const offButtonControl = document.getElementById('control-power-off-btn');

            if (relayState) {
                onButtonDashboard.disabled = true;
                offButtonDashboard.disabled = false;
                onButtonControl.disabled = true;
                offButtonControl.disabled = false;
            } else {
                onButtonDashboard.disabled = false;
                offButtonDashboard.disabled = true;
                onButtonControl.disabled = false;
                offButtonControl.disabled = true;
            }

            const relayStatusText = relayState ? 'ON' : 'OFF';
            updateElement('relay-control-status', relayStatusText);
        }


        function updateAccountSection(data) {
            updateElement('account-balance', data.balance.toFixed(2) + '<span class="unit">₹</span>');
            // Example consumption rate calculation (needs actual logic)
            const consumptionRate = (data.energy * data.costPerKWh).toFixed(2); // Example
            updateElement('consumption-rate', consumptionRate + '<span class="unit">₹/day</span>'); // Needs proper daily calculation
        }

        function updateControlSection(data) {
            updateElement('relay-control-status', data.relayState ? 'ON' : 'OFF');

            updateStatusIndicator('control-voltage-indicator', (data.voltage < 180 || data.voltage > 250) ? 'status-warning' : 'status-active');
            updateElement('control-voltage-status', (data.voltage < 180) ? 'LOW' : (data.voltage > 250 ? 'HIGH' : 'OK'));

            updateStatusIndicator('control-current-indicator', (data.current > data.overCurrent) ? 'status-warning' : 'status-active');
            updateElement('control-current-status', (data.current > data.overCurrent) ? 'OVER' : 'OK');

            updateStatusIndicator('control-fault-indicator', data.faultDetected ? 'status-danger' : 'status-active');
            updateElement('control-fault-status', data.faultDetected ? 'FAULT' : 'OK');

            updateStatusIndicator('control-theft-indicator', data.theftDetected ? 'status-warning' : 'status-active');
            updateElement('control-theft-status', data.theftDetected ? 'DETECTED' : 'OK');

            updatePowerControlUI(data.relayState); // Keep control section buttons in sync
        }


        function updateElement(id, html) {
            const element = document.getElementById(id);
            if (element) {
                element.innerHTML = html;
            }
        }

        function updateStatusIndicator(id, className) {
            const indicator = document.getElementById(id);
            if (indicator) {
                indicator.className = 'status-indicator ' + className;
            }
        }


        function setRelay(state) {
            const onButtonDashboard = document.getElementById('power-on-btn');
            const offButtonDashboard = document.getElementById('power-off-btn');
            const onButtonControl = document.getElementById('control-power-on-btn');
            const offButtonControl = document.getElementById('control-power-off-btn');
            const alertElementDashboard = document.getElementById('power-control-alert');
            const alertElementControl = document.getElementById('relay-control-alert');

            // Disable buttons to prevent multiple clicks
            onButtonDashboard.disabled = true;
            offButtonDashboard.disabled = true;
            onButtonControl.disabled = true;
            offButtonControl.disabled = true;

            const alertElement = document.querySelector('.page-section.active').id === 'dashboard-section' ? alertElementDashboard : alertElementControl;

            showAlert('Processing power control...', 'warning', alertElement.id);

            let overrideState = 0; // Assume no override
            if (state === 0) {
                overrideState = 1; // Set override if turning off
            } else {
                overrideState = 0; // Clear override if turning on
            }

            fetch(`/setRelay?state=${state}&override=${overrideState}`, { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        updatePowerControlUI(state === 1); // Update UI based on requested state
                        showAlert(`Power turned ${state === 1 ? 'ON' : 'OFF'} successfully.`, 'success', alertElement.id);
                    } else {
                        showAlert('Power control failed: ' + (data.message || 'Unknown error'), 'danger', alertElement.id);
                        updatePowerControlUI(relayState); // Revert UI to previous state if failed
                    }
                     // Re-enable buttons after request completes (or fails)
                    onButtonDashboard.disabled = false;
                    offButtonDashboard.disabled = false;
                    onButtonControl.disabled = false;
                    offButtonControl.disabled = false;
                })
                .catch(error => {
                    console.error('Error controlling relay:', error);
                    showAlert('Relay control error', 'danger', alertElement.id);
                    updatePowerControlUI(relayState); // Revert UI on error
                     // Re-enable buttons even on error
                    onButtonDashboard.disabled = false;
                    offButtonDashboard.disabled = false;
                    onButtonControl.disabled = false;
                    offButtonControl.disabled = false;
                });
        }


        function rechargeAccount() {
            const amount = document.getElementById('recharge-amount').value;
            const rechargeAlert = document.getElementById('recharge-alert');
            const rechargeButton = document.querySelector('#recharge-alert + button.btn-primary, #account-section button.btn-primary');

            rechargeButton.disabled = true; // Disable recharge button during request
            showAlert('Processing recharge...', 'warning', 'recharge-alert');


            fetch(`/recharge?amount=${amount}`, { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        showAlert(`Recharge successful. New balance: ₹${data.balance.toFixed(2)}`, 'success', 'recharge-alert');
                        updateElement('balance', data.balance.toFixed(2) + '<span class="unit">₹</span>');
                        updateElement('account-balance', data.balance.toFixed(2) + '<span class="unit">₹</span>');
                    } else {
                        showAlert('Recharge failed. ' + (data.message || 'Please try again.'), 'danger', 'recharge-alert');
                    }
                    rechargeButton.disabled = false; // Re-enable button after request completes
                })
                .catch(error => {
                    console.error('Recharge error:', error);
                    showAlert('Recharge error. Please try again.', 'danger', 'recharge-alert');
                    rechargeButton.disabled = false; // Re-enable button even on error
                });
        }

        function quickRecharge() {
            const amount = document.getElementById('quick-recharge-amount').value;
            const rechargeAlert = document.getElementById('recharge-alert');
            const rechargeButton = document.querySelector('#dashboard-section .action-card button.btn-primary');

            rechargeButton.disabled = true;
            showAlert('Processing recharge...', 'warning', 'power-control-alert');


            fetch(`/recharge?amount=${amount}`, { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        showAlert(`Recharge successful. New balance: ₹${data.balance.toFixed(2)}`, 'success', 'power-control-alert');
                        updateElement('balance', data.balance.toFixed(2) + '<span class="unit">₹</span>');
                        updateElement('account-balance', data.balance.toFixed(2) + '<span class="unit">₹</span>');
                    } else {
                        showAlert('Recharge failed. ' + (data.message || 'Please try again.'), 'danger', 'power-control-alert');
                    }
                    rechargeButton.disabled = false;
                })
                .catch(error => {
                    console.error('Recharge error:', error);
                    showAlert('Recharge error. Please try again.', 'danger', 'power-control-alert');
                    rechargeButton.disabled = false;
                });
        }


        function showAlert(message, type, alertElementId) {
            const alertDiv = document.getElementById(alertElementId);
            if(alertDiv) {
                alertDiv.textContent = message;
                alertDiv.className = `alert alert-${type}`;
                alertDiv.style.display = 'block';
                setTimeout(() => {
                    alertDiv.style.display = 'none';
                }, 5000);
            }
        }

        function setThresholds() {
            const form = document.getElementById('threshold-form');
            const formData = new FormData(form); // Use FormData to easily collect form data
            const thresholdAlert = document.getElementById('threshold-alert');
            const saveButton = document.querySelector('#threshold-form button.btn-primary'); // Select save button
            const data = {};
            formData.forEach((value, key) => {
                data[key] = parseFloat(value); // Convert string values from form to floats
            });

            saveButton.disabled = true; // Disable save button during request
            showAlert('Saving settings...', 'warning', 'threshold-alert');


            fetch('/setThresholds', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json' // Set content type to JSON
                },
                body: JSON.stringify(data) // Send form data as JSON
            })
            .then(response => response.json())
            .then(responseData => {
                if (responseData.success) {
                    showAlert('Settings updated successfully.', 'success', 'threshold-alert');
                    // Optionally update displayed threshold values on the page if you wish
                } else {
                    showAlert('Failed to update settings: ' + (responseData.message || 'Unknown error'), 'danger', 'threshold-alert');
                }
                saveButton.disabled = false; // Re-enable button after request completes
            })
            .catch(error => {
                console.error('Error setting thresholds:', error);
                showAlert('Error updating settings. Please try again.', 'danger', 'threshold-alert');
                saveButton.disabled = false; // Re-enable button even on error
            });
        }

        function factoryReset() {
            const resetAlert = document.getElementById('factory-reset-alert');
            const resetButton = document.querySelector('#settings-section button.btn-danger');

            resetButton.disabled = true;
            showAlert('Performing factory reset...', 'warning', 'factory-reset-alert');

            fetch('/factoryReset', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        showAlert('Factory reset successful. System restarting...', 'success', 'factory-reset-alert');
                        setTimeout(() => {
                            window.location.href = '/login'; // Redirect to login page after reset
                        }, 3000); // Wait 3 seconds before redirecting and restarting ESP
                    } else {
                        showAlert('Factory reset failed: ' + (data.message || 'Unknown error'), 'danger', 'factory-reset-alert');
                    }
                    resetButton.disabled = false;
                })
                .catch(error => {
                    console.error('Factory reset error:', error);
                    showAlert('Factory reset error. Please try again.', 'danger', 'factory-reset-alert');
                    resetButton.disabled = false;
                });
        }


        // Initial update and set interval
        updateValues();
        setInterval(updateValues, 3000); // Update every 3 seconds

        // Set initial page to dashboard
        showPage('dashboard');
    </script>
</body>
</html>
)rawliteral";


// Login page HTML (No changes needed unless further UI/UX improvements are desired)
const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <title>Login - ESP32 Prepaid Energy Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <style>
    :root {
      --primary-color: #3498db;
      --secondary-color: #2c3e50;
      --background-color: #f5f7fa;
      --card-background: #ffffff;
      --text-color: #333333;
      --text-light: #7f8c8d;
      --shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
      --transition: all 0.3s ease;
    }

body {
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  background-color: var(--background-color);
  margin: 0;
  padding: 0;
  color: var(--text-color);
  line-height: 1.6;
}

.login-container {
  max-width: 400px;
  margin: 100px auto;
  background: var(--card-background);
  padding: 30px;
  border-radius: 8px;
  box-shadow: var(--shadow);
}

.login-container h2 {
  text-align: center;
  color: var(--secondary-color);
  margin-bottom: 30px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.login-container h2 i {
  margin-right: 10px;
  color: var(--primary-color);
}

.form-group {
  margin-bottom: 20px;
}

.form-group label {
  display: block;
  margin-bottom: 5px;
  font-weight: bold;
  color: var(--secondary-color);
}

.form-group input {
  width: 100%;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 16px;
}

.button {
  background: linear-gradient(to right, var(--primary-color), var(--secondary-color));
  color: white;
  border: none;
  padding: 12px 24px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 16px;
  font-weight: bold;
  transition: var(--transition);
  display: inline-flex;
  align-items: center;
  width: 100%;
  justify-content: center;
}

.button i {
  margin-right: 8px;
}

.button:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15);
}

.alert {
  padding: 10px 15px;
  border-radius: 4px;
  margin-bottom: 15px;
  font-size: 14px;
}

.alert-danger {
  background-color: #f8d7da;
  color: #721c24;
  border: 1px solid #f5c6cb;
}
  </style>
</head>
<body>
  <div class="login-container">
    <h2><i class="fas fa-lock"></i> Login</h2>

<div id="error-message" class="alert alert-danger" style="display: none;">
  Invalid username or password
</div>

<form action="/login" method="post">
  <div class="form-group">
    <label for="username">Username</label>
    <input type="text" id="username" name="username" required>
  </div>

  <div class="form-group">
    <label for="password">Password</label>
    <input type="password" id="password" name="password" required>
  </div>

  <button type="submit" class="button">
    <i class="fas fa-sign-in-alt"></i> Login
  </button>
</form>
  </div>

  <script>// Check for error parameter in URL
    const urlParams = new URLSearchParams(window.location.search);
    if (urlParams.has('error')) {
      document.getElementById('error-message').style.display = 'block';
    }
  </script>

</body>
</html>
)rawliteral";


void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(500);

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);

  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);

  // Initialize Button Pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);  // Adjust rotation if needed for your display
  tft.fillScreen(BLACK);
  tft.setTextWrap(false);

  // Display startup sequence
  displayStartupSequence();

  // Load saved balance and thresholds from EEPROM
  loadBalance();
  loadThresholds();

  // Configure WiFi (Simplified)
  configureWiFi();

  // Store IP address as string
  if (WiFi.status() == WL_CONNECTED) {
    ipAddressStr = WiFi.localIP().toString();
    showNotification("IP: " + ipAddressStr, GREEN, 5000);
  } else {
    showNotification("WiFi Failed to connect", RED, 5000);
  }

  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/login", HTTP_GET, []() {
    server.send(200, "text/html", login_html);
  });
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/data", HTTP_GET, handleData);
  server.on("/recharge", HTTP_POST, handleRecharge);
  server.on("/reset", HTTP_POST, handleReset);
  server.on("/setRelay", HTTP_POST, handleSetRelay);
  server.on("/setThresholds", HTTP_POST, handleSetThresholds);  // New handler for thresholds
  server.on("/factoryReset", HTTP_POST, handleFactoryReset);    // New handler for factory reset
  server.onNotFound(handleNotFound);

  // Start web server
  server.begin();

  // Show notification
  showNotification("System Ready", GREEN, 3000);
}

void loop() {
  // Handle web server client requests
  server.handleClient();

  // Read PZEM data at specified intervals
  if (millis() - lastReadTime >= measurementInterval) {
    readPzemData();
    lastReadTime = millis();

    // Check for faults and theft
    checkFaultsAndTheft();

    // Control relay
    controlRelay();
  }

  // Update balance based on energy usage
  if (millis() - balanceUpdateTime >= BALANCE_UPDATE_INTERVAL) {
    updateBalance();
    balanceUpdateTime = millis();
    saveBalance();
  }

  // **Button Handling for Page Change and Manual Relay Override**
  int reading = digitalRead(BUTTON_PIN);
  bool currentButtonState = (reading == LOW);  // Button is active low

  if (currentButtonState && !lastButtonState) {  // Button pressed
    if (millis() - lastButtonPress > debounceDelay) {
      lastButtonPress = millis();
      buttonPressStartTime = millis();
      manualRelayControlActive = true;  // Start checking for long press
    }
  } else if (!currentButtonState && lastButtonState) {  // Button released
    if (manualRelayControlActive) {
      if (millis() - buttonPressStartTime >= 1000) {  // Long press detected (3 seconds)
        manualOverride = !manualOverride;             // Toggle manual override state
        relayState = !relayState;                     // Toggle relay state
        digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);
        showNotification(relayState ? "Manual Relay ON" : "Manual Relay OFF", YELLOW);
        // Web server will reflect state on next /data request
      } else {
        displayPage = (displayPage + 1) % 2;  // Toggle page on short press
        pageChangeTime = millis();
        layoutDrawn = false;  // Redraw layout for new page if needed
      }
      manualRelayControlActive = false;
    }
  }
  lastButtonState = currentButtonState;


  // Update display with page switching
  if (millis() - screenRefreshTime >= SCREEN_REFRESH_INTERVAL) {
    screenRefreshTime = millis();
    if (!layoutDrawn) {
      drawLayout();  // Draw common layout elements (title, separators) - Draw only once per page change
      layoutDrawn = true;
    }

    if (millis() - pageChangeTime >= PAGE_INTERVAL) {
      // Automatic page switch removed due to button implementation, keep it on current page until button press
      // displayPage = (displayPage + 1) % 2; // Switch between 2 pages (0 and 1) - Removed Automatic switching
      pageChangeTime = millis();  //reset page change timer, even if not auto-switching now
    }

    updateDisplayValues();  // Update dynamic values on the current page - Update only values, not full page redraw
  }

  // Clear notification if display time has elapsed
  if (notificationEndTime > 0 && millis() >= notificationEndTime) {
    clearNotification();
  }

  // Periodic notifications every 5 minutes
  if (millis() - lastNotificationTime >= NOTIFICATION_INTERVAL) {
    if (faultDetected) {
      showNotification("Fault Detected! Check System.", RED);
    } else if (theftDetected) {
      showNotification("Theft Detected! Investigate.", RED);
    } else if (balance <= minimumBalance) {  // Use variable threshold
      showNotification("Low Balance! Recharge Soon.", YELLOW);
    } else if (!pzemConnected) {
      showNotification("PZEM Disconnected!", RED);
    } else if (WiFi.status() != WL_CONNECTED) {
      showNotification("WiFi Disconnected!", RED);
    }
    // Add other conditions for different types of notifications as needed.

    lastNotificationTime = millis();
  }

  // Check WiFi connection periodically and attempt to reconnect
  checkWiFiConnection();
}

// Function to check WiFi connection and reconnect if necessary
void checkWiFiConnection() {
  if (millis() - lastWifiCheckTime >= WIFI_RECHECK_INTERVAL) {
    lastWifiCheckTime = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi Disconnected. Attempting reconnection...");
      WiFi.reconnect();
      showNotification("WiFi Disconnected! Reconnecting...", YELLOW);
      // Optionally add a timeout for reconnection attempts to avoid indefinite blocking
    } else if (WiFi.status() == WL_CONNECTED && currentNotification.startsWith("WiFi Disconnected!")) {
      showNotification("WiFi Reconnected!", GREEN);
    }
  }
}


// Read data from PZEM sensor
void readPzemData() {
  readings.voltage = pzem.voltage();
  readings.current = pzem.current();
  readings.power = pzem.power();
  readings.energy = pzem.energy();

  // Determine if reading is valid (voltage is used as indicator)
  readings.isValid = !(isnan(readings.voltage) || readings.voltage < 1.0);

  // Update connection status
  bool newConnectionStatus = readings.isValid;
  if (pzemConnected != newConnectionStatus) {
    pzemConnected = newConnectionStatus;
    if (pzemConnected) {
      showNotification("PZEM Connected", GREEN);
    } else {
      showNotification("PZEM Disconnected", RED);
    }
  }

  // Debug output
  printDebugInfo("PZEM Reading", readings.isValid);
}

// Draw common layout elements (Title, separators, static labels) - Reduced redraw to title and separator only
void drawLayout() {
  tft.fillScreen(BLACK);  // Clear the whole screen only on page change for clean transition
  tft.setTextSize(1);

  // Title Bar - Compact for 1.8 inch display
  setTextProperties(tft, YELLOW);
  tft.setCursor(2, 2);
  tft.print("ENERGY METER");
  tft.drawFastHLine(0, 14, tft.width(), BLUE);  // Header bar background

  // Page Content Area (below title bar)
  if (displayPage == 0) {
    drawLiveDataPage();  // Draw Live Data Page elements
  } else if (displayPage == 1) {
    drawAlertsPage();  // Draw Alerts Page elements
  }
  layoutDrawn = true;  // Ensure layout is marked as drawn after drawing page specific elements
}

// Constants for layout spacing and text positions - Optimized for 1.8 inch display
const int HEADER_HEIGHT = 14;
const int BOX_HEIGHT = 30;
const int BOX_ROUND_RADIUS = 3;
const int BOX_VERTICAL_START = HEADER_HEIGHT + 3;
const int ROW_SPACING = BOX_HEIGHT + 2;
const int LABEL_TEXT_Y_OFFSET = 3;
const int VALUE_TEXT_Y_OFFSET = 18;
const int FOOTER_AREA_HEIGHT = 20;
const int FOOTER_TEXT_Y_OFFSET = 6;


// Draw Live Data Page - Improved Layout with constants
void drawLiveDataPage() {
  // Draw header bar
  tft.fillRect(0, 0, tft.width(), HEADER_HEIGHT, BLUE);
  tft.setTextSize(1);
  setTextProperties(tft, WHITE);
  tft.setCursor(5, 4);
  tft.print("ESP32 PREPAID METER");

  // Row 1: Voltage & Current
  tft.fillRoundRect(2, BOX_VERTICAL_START + (ROW_SPACING * 0), tft.width() - 4, BOX_HEIGHT, BOX_ROUND_RADIUS, DARKGRAY);
  tft.drawRoundRect(2, BOX_VERTICAL_START + (ROW_SPACING * 0), tft.width() - 4, BOX_HEIGHT, BOX_ROUND_RADIUS, WHITE);

  // Row 2: Power & Energy
  tft.fillRoundRect(2, BOX_VERTICAL_START + (ROW_SPACING * 1), tft.width() - 4, BOX_HEIGHT, BOX_ROUND_RADIUS, DARKGRAY);
  tft.drawRoundRect(2, BOX_VERTICAL_START + (ROW_SPACING * 1), tft.width() - 4, BOX_HEIGHT, BOX_ROUND_RADIUS, WHITE);

  // Row 3: Balance & Status (Replaced Frequency & PF)
  tft.fillRoundRect(2, BOX_VERTICAL_START + (ROW_SPACING * 2), tft.width() - 4, BOX_HEIGHT, BOX_ROUND_RADIUS, DARKGRAY);
  tft.drawRoundRect(2, BOX_VERTICAL_START + (ROW_SPACING * 2), tft.width() - 4, BOX_HEIGHT, BOX_ROUND_RADIUS, WHITE);

  // Draw labels for each row
  tft.setTextSize(1);
  setTextProperties(tft, CYAN);

  // Row 1 labels
  tft.setCursor(5, BOX_VERTICAL_START + (ROW_SPACING * 0) + LABEL_TEXT_Y_OFFSET);
  tft.print("VOLTAGE");
  tft.setCursor(tft.width() / 2 + 5, BOX_VERTICAL_START + (ROW_SPACING * 0) + LABEL_TEXT_Y_OFFSET);
  tft.print("CURRENT");

  // Row 2 labels
  tft.setCursor(5, BOX_VERTICAL_START + (ROW_SPACING * 1) + LABEL_TEXT_Y_OFFSET);
  tft.print("POWER");
  tft.setCursor(tft.width() / 2 + 5, BOX_VERTICAL_START + (ROW_SPACING * 1) + LABEL_TEXT_Y_OFFSET);
  tft.print("ENERGY");

  // Row 3 labels (Balance & Status)
  tft.setCursor(5, BOX_VERTICAL_START + (ROW_SPACING * 2) + LABEL_TEXT_Y_OFFSET);
  tft.print("BALANCE");
  tft.setCursor(tft.width() / 2 + 5, BOX_VERTICAL_START + (ROW_SPACING * 2) + LABEL_TEXT_Y_OFFSET);
  tft.print("STATUS");

  // Draw notification area and IP address at bottom
  tft.drawRect(2, 113, tft.width() - 4, FOOTER_AREA_HEIGHT, WHITE);

  // Display IP address in footer
  tft.setTextSize(1);
  setTextProperties(tft, GREEN);
  tft.setCursor(5, 113 + FOOTER_TEXT_Y_OFFSET);
  tft.print("IP: ");

  // Use stored IP address string
  if (WiFi.status() == WL_CONNECTED) {
    tft.print(ipAddressStr);
  } else {
    setTextProperties(tft, RED);
    tft.print("Not Connected");
    setTextProperties(tft, GREEN);  // Reset color for IP: prefix for next redraw
  }

  // Reset all text buffers when drawing a new page
  memset(lastVoltageText, 0, sizeof(lastVoltageText));
  memset(lastCurrentText, 0, sizeof(lastCurrentText));
  memset(lastPowerText, 0, sizeof(lastPowerText));
  memset(lastEnergyText, 0, sizeof(lastEnergyText));
  memset(lastBalanceText, 0, sizeof(lastBalanceText));
  memset(lastStatusText, 0, sizeof(lastStatusText));
}

void drawAlertsPage() {
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  setTextProperties(tft, YELLOW);

  tft.setCursor(5, 10);
  tft.print("ALERTS");
  tft.drawFastHLine(0, 30, tft.width(), DARKGRAY);

  setTextProperties(tft, CYAN);
  int y_offset = 40;

  tft.setTextSize(1);
  setTextProperties(tft, CYAN);  // Set color for labels on Alerts page
  tft.setCursor(5, 40);
  tft.print("Over Voltage Limit:");
  tft.setCursor(5, 55);
  tft.print("Over Current Limit:");
  tft.setCursor(5, 70);
  tft.print("Theft Current:");
  tft.setCursor(5, 85);
  tft.print("Min Balance:");


  // Footer with smaller text
  tft.setTextSize(1);
  setTextProperties(tft, DARKGRAY);
  tft.setCursor(5, tft.height() - 10);
  tft.print("2/2");

  // Reset alert text buffers
  memset(lastFaultText, 0, sizeof(lastFaultText));
  memset(lastTheftText, 0, sizeof(lastTheftText));
}

void updateDisplayValues() {
  if (displayPage == 0) {  // Live Data Page
    int x_value_start_left = 5;
    int x_value_start_right = tft.width() / 2 + 5;
    char newText[20];  // Increased buffer size for values with units

    tft.setTextSize(1);
    setTextProperties(tft, WHITE);

    // Row 1: Voltage
    tft.fillRect(x_value_start_left, BOX_VERTICAL_START + (ROW_SPACING * 0) + VALUE_TEXT_Y_OFFSET, tft.width() / 2 - 10, BOX_HEIGHT - VALUE_TEXT_Y_OFFSET - 2, BLACK);
    tft.setCursor(x_value_start_left, BOX_VERTICAL_START + (ROW_SPACING * 0) + VALUE_TEXT_Y_OFFSET);
    if (readings.isValid) {
      sprintf(newText, "%.1fV", readings.voltage);
      tft.print(newText);
      strcpy(lastVoltageText, newText);
    } else {
      tft.print("0.0V");  // Display 0.0V when no reading
      strcpy(lastVoltageText, "0.0V");
    }


    // Row 1: Current
    tft.fillRect(x_value_start_right, BOX_VERTICAL_START + (ROW_SPACING * 0) + VALUE_TEXT_Y_OFFSET, tft.width() / 2 - 10, BOX_HEIGHT - VALUE_TEXT_Y_OFFSET - 2, BLACK);
    tft.setCursor(x_value_start_right, BOX_VERTICAL_START + (ROW_SPACING * 0) + VALUE_TEXT_Y_OFFSET);
    if (readings.isValid) {
      sprintf(newText, "%.3fA", readings.current);
      tft.print(newText);
      strcpy(lastCurrentText, newText);
    } else {
      tft.print("0.000A");  // Display 0.000A when no reading
      strcpy(lastCurrentText, "0.000A");
    }

    // Row 2: Power
    tft.fillRect(x_value_start_left, BOX_VERTICAL_START + (ROW_SPACING * 1) + VALUE_TEXT_Y_OFFSET, tft.width() / 2 - 10, BOX_HEIGHT - VALUE_TEXT_Y_OFFSET - 2, BLACK);
    tft.setCursor(x_value_start_left, BOX_VERTICAL_START + (ROW_SPACING * 1) + VALUE_TEXT_Y_OFFSET);
    if (readings.isValid) {
      sprintf(newText, "%.1fW", readings.power);
      tft.print(newText);
      strcpy(lastPowerText, newText);
    } else {
      tft.print("0.0W");  // Display 0.0W when no reading
      strcpy(lastPowerText, "0.0W");
    }


    // Row 2: Energy
    tft.fillRect(x_value_start_right, BOX_VERTICAL_START + (ROW_SPACING * 1) + VALUE_TEXT_Y_OFFSET, tft.width() / 2 - 10, BOX_HEIGHT - VALUE_TEXT_Y_OFFSET - 2, BLACK);
    tft.setCursor(x_value_start_right, BOX_VERTICAL_START + (ROW_SPACING * 1) + VALUE_TEXT_Y_OFFSET);
    if (readings.isValid) {
      sprintf(newText, "%.2fkWh", readings.energy);
      tft.print(newText);
      strcpy(lastEnergyText, newText);
    } else {
      tft.print("0.00kWh");  // Display 0.00kWh when no reading
      strcpy(lastEnergyText, "0.00kWh");
    }


    // Row 3: Balance
    tft.fillRect(x_value_start_left, BOX_VERTICAL_START + (ROW_SPACING * 2) + VALUE_TEXT_Y_OFFSET, tft.width() / 2 - 10, BOX_HEIGHT - VALUE_TEXT_Y_OFFSET - 2, BLACK);
    tft.setCursor(x_value_start_left, BOX_VERTICAL_START + (ROW_SPACING * 2) + VALUE_TEXT_Y_OFFSET);
    sprintf(newText, "%.2f Rs", balance);
    tft.print(newText);
    strcpy(lastBalanceText, newText);


    // Row 3: Status
    tft.fillRect(x_value_start_right, BOX_VERTICAL_START + (ROW_SPACING * 2) + VALUE_TEXT_Y_OFFSET, tft.width() / 2 - 10, BOX_HEIGHT - VALUE_TEXT_Y_OFFSET - 2, BLACK);
    tft.setCursor(x_value_start_right, BOX_VERTICAL_START + (ROW_SPACING * 2) + VALUE_TEXT_Y_OFFSET);

    if (balance <= minimumBalance) {
      setTextProperties(tft, RED);
      strcpy(newText, "LOW BAL");
    } else if (faultDetected) {
      setTextProperties(tft, RED);
      strcpy(newText, "FAULT");
    } else if (theftDetected) {
      setTextProperties(tft, ORANGE);  // Use Orange for theft
      strcpy(newText, "THEFT");
    } else if (!pzemConnected) {
      setTextProperties(tft, RED);
      strcpy(newText, "PZEM OFF");
    } else if (relayState) {
      setTextProperties(tft, GREEN);
      strcpy(newText, "ACTIVE");
    } else {
      setTextProperties(tft, DARKGRAY);  // Use dark gray for OFF status
      strcpy(newText, "OFF");
    }
    tft.print(newText);
    strcpy(lastStatusText, newText);
    setTextProperties(tft, WHITE);  // Reset text color to white for other values

  } else if (displayPage == 1) {  // Alerts Page
    tft.setTextSize(1);
    setTextProperties(tft, WHITE);
    int x_value_start = 130;  // Adjusted x position for values on alerts page

    char newText[20];
    setTextProperties(tft, WHITE);  // Set color for threshold values on Alerts page

    tft.fillRect(x_value_start, 40, tft.width() - x_value_start - 5, 8, BLACK);
    tft.setCursor(x_value_start, 40);
    sprintf(newText, "%.1fV", overVoltageThreshold);
    tft.print(newText);

    tft.fillRect(x_value_start, 55, tft.width() - x_value_start - 5, 8, BLACK);
    tft.setCursor(x_value_start, 55);
    sprintf(newText, "%.2fA", overCurrentThreshold);
    tft.print(newText);

    tft.fillRect(x_value_start, 70, tft.width() - x_value_start - 5, 8, BLACK);
    tft.setCursor(x_value_start, 70);
    sprintf(newText, "%.3fA", theftCurrentThreshold);
    tft.print(newText);

    tft.fillRect(x_value_start, 85, tft.width() - x_value_start - 5, 8, BLACK);
    tft.setCursor(x_value_start, 85);
    sprintf(newText, "%.2f Rs", minimumBalance);
    tft.print(newText);
  }

  // Notification area - Moved higher for better visibility
  static unsigned long lastNotificationUpdate = 0;
  static char lastNotificationText[30] = "";
  int notificationY = 113 + FOOTER_TEXT_Y_OFFSET;  // Adjusted notification Y

  if (notificationEndTime > 0 && millis() < notificationEndTime) {
    if (strcmp(currentNotification.c_str(), lastNotificationText) != 0 || millis() - lastNotificationUpdate > 500) {

      tft.fillRect(2, 113, tft.width() - 4, FOOTER_AREA_HEIGHT, BLACK);  // Clear footer area
      tft.setTextSize(1);
      setTextProperties(tft, notificationColor);
      tft.setCursor(2, notificationY);
      tft.print(currentNotification);

      strcpy(lastNotificationText, currentNotification.c_str());
      lastNotificationUpdate = millis();
    }
  } else if (notificationEndTime > 0 && millis() >= notificationEndTime) {
    // Clear notification when expired
    tft.fillRect(2, 113, tft.width() - 4, FOOTER_AREA_HEIGHT, BLACK);  // Clear footer area
    notificationEndTime = 0;
    lastNotificationText[0] = '\0';
    // Redraw IP address after clearing notification
    setTextProperties(tft, GREEN);
    tft.setCursor(5, 113 + FOOTER_TEXT_Y_OFFSET);
    tft.print("IP: ");
    if (WiFi.status() == WL_CONNECTED) {
      tft.print(ipAddressStr);
    } else {
      setTextProperties(tft, RED);
      tft.print("Not Connected");
      setTextProperties(tft, GREEN);  // Reset color for IP: prefix for next redraw
    }
  }
}

void showNotification(const String& message, uint16_t color, unsigned long duration) {
  currentNotification = message;
  notificationColor = color;
  notificationEndTime = millis() + duration;

  // Notification area positioned higher
  int notificationY = 113 + FOOTER_TEXT_Y_OFFSET;
  tft.fillRect(2, 113, tft.width() - 4, FOOTER_AREA_HEIGHT, BLACK);  // Clear footer area
  tft.setTextSize(1);
  setTextProperties(tft, color);
  tft.setCursor(2, notificationY);
  tft.print(message);
}

void clearNotification() {
  // Clear the notification area at its new position
  tft.fillRect(2, 113, tft.width() - 4, FOOTER_AREA_HEIGHT, BLACK);
  notificationEndTime = 0;
  // Redraw IP address after clearing notification
  setTextProperties(tft, GREEN);
  tft.setCursor(5, 113 + FOOTER_TEXT_Y_OFFSET);
  tft.print("IP: ");
  if (WiFi.status() == WL_CONNECTED) {
    tft.print(ipAddressStr);
  } else {
    setTextProperties(tft, RED);
    tft.print("Not Connected");
    setTextProperties(tft, GREEN);  // Reset color for IP: prefix for next redraw
  }
}

void displayStartupSequence() {
  tft.fillScreen(BLACK);
  tft.setTextSize(2);  // Larger text for startup
  setTextProperties(tft, GREEN);

  // Center the title text
  int titleX = (tft.width() - 12 * 11) / 2;  // Approximate centering for "ENERGY METER"
  tft.setCursor(titleX > 0 ? titleX : 0, 10);
  tft.print("ENERGY METER");

  tft.setTextSize(1);             // Smaller text for status messages
  setTextProperties(tft, WHITE);  // Set color for status text to white
  tft.setCursor(5, 35);
  tft.print("Booting...");

  // Progress bar sized appropriately for display
  int barWidth = tft.width() - 10;
  tft.drawRect(5, 45, barWidth, 8, WHITE);

  for (int i = 0; i < barWidth - 4; i += 2) {
    tft.fillRect(7, 47, i, 4, BLUE);
    delay(25);  // Faster animation
  }

  // Status messages positioned better
  clearTextArea(5, 60, tft.width() - 10, 8);
  setTextProperties(tft, WHITE);  // Ensure color is set before each text print
  tft.setCursor(5, 60);
  tft.print("Init sensors...");
  delay(300);

  clearTextArea(5, 60, tft.width() - 10, 8);
  setTextProperties(tft, WHITE);
  tft.setCursor(5, 60);
  tft.print("Web server...");
  delay(300);

  clearTextArea(5, 60, tft.width() - 10, 8);
  setTextProperties(tft, WHITE);
  tft.setCursor(5, 60);
  tft.print("System ready!");
  delay(500);
}

void configureWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  tft.fillScreen(BLACK);
  tft.setTextSize(1);  // Smaller text to fit more info
  setTextProperties(tft, WHITE);
  tft.setCursor(5, 10);
  tft.print("Connecting WiFi...");

  // Add a small animation during connection
  int dotCount = 0;
  int timeout = 0;

  while (WiFi.status() != WL_CONNECTED && timeout < 30) {
    delay(1000);
    Serial.print(".");

    // Clear animation line and redraw
    tft.fillRect(5, 25, tft.width() - 10, 8, BLACK);
    setTextProperties(tft, WHITE);  // Set color before printing dots
    tft.setCursor(5, 25);
    for (int i = 0; i < dotCount; i++) {
      tft.print(".");
    }

    dotCount = (dotCount + 1) % 4;  // Cycle between 0-3 dots
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    tft.fillRect(5, 25, tft.width() - 10, 16, BLACK);
    setTextProperties(tft, WHITE);
    tft.setCursor(5, 25);
    tft.print("Connected!");
    tft.setCursor(5, 40);
    tft.print(WiFi.localIP().toString());

    Serial.println("\nWiFi Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    tft.fillRect(5, 25, tft.width() - 10, 8, BLACK);
    setTextProperties(tft, RED);
    tft.setCursor(5, 25);
    tft.print("Connection Failed!");

    Serial.println("\nWiFi Connection Failed!");
  }

  delay(1500);  // Show the result briefly
}
// Save balance to EEPROM - No changes needed, looks good
void saveBalance() {
  EEPROM.writeFloat(BALANCE_ADDR, balance);
  EEPROM.writeFloat(ENERGY_ADDR, lastEnergy);
  EEPROM.commit();
  printDebugInfo("Save Balance", true);
}

// Load balance from EEPROM - No changes needed, looks good
void loadBalance() {
  balance = EEPROM.readFloat(BALANCE_ADDR);
  lastEnergy = EEPROM.readFloat(ENERGY_ADDR);

  // Check if values are valid (not NaN or negative)
  if (isnan(balance) || balance < 0) {
    balance = 0.0;
  }

  if (isnan(lastEnergy) || lastEnergy < 0) {
    lastEnergy = 0.0;
  }

  printDebugInfo("Load Balance", true);
}

// Save thresholds to EEPROM - No changes needed, looks good
void saveThresholds() {
  EEPROM.writeFloat(OVER_VOLTAGE_THRESHOLD_ADDR, overVoltageThreshold);
  EEPROM.writeFloat(OVER_CURRENT_THRESHOLD_ADDR, overCurrentThreshold);
  EEPROM.writeFloat(THEFT_CURRENT_THRESHOLD_ADDR, theftCurrentThreshold);
  EEPROM.writeFloat(MINIMUM_BALANCE_ADDR, minimumBalance);
  EEPROM.writeFloat(COST_PER_KWH_ADDR, costPerKWh);
  EEPROM.commit();
  printDebugInfo("Save Thresholds", true);
}

// Load thresholds from EEPROM - No changes needed, looks good
void loadThresholds() {
  overVoltageThreshold = EEPROM.readFloat(OVER_VOLTAGE_THRESHOLD_ADDR);
  overCurrentThreshold = EEPROM.readFloat(OVER_CURRENT_THRESHOLD_ADDR);
  theftCurrentThreshold = EEPROM.readFloat(THEFT_CURRENT_THRESHOLD_ADDR);
  minimumBalance = EEPROM.readFloat(MINIMUM_BALANCE_ADDR);
  costPerKWh = EEPROM.readFloat(COST_PER_KWH_ADDR);  // Corrected usage - making sure macro is spelled correctly

  // Check if values are valid (not NaN or negative/zero where appropriate)
  if (isnan(overVoltageThreshold) || overVoltageThreshold <= 0) {
    overVoltageThreshold = DEFAULT_OVER_VOLTAGE_THRESHOLD;
  }
  if (isnan(overCurrentThreshold) || overCurrentThreshold <= 0) {
    overCurrentThreshold = DEFAULT_OVER_CURRENT_THRESHOLD;
  }
  if (isnan(theftCurrentThreshold) || theftCurrentThreshold < 0) {
    theftCurrentThreshold = DEFAULT_THEFT_CURRENT_THRESHOLD;
  }
  if (isnan(minimumBalance)) {  // Minimum balance can be negative if you want to allow credit
    minimumBalance = DEFAULT_MINIMUM_BALANCE;
  }
  if (isnan(costPerKWh) || costPerKWh <= 0) {
    costPerKWh = DEFAULT_COST_PER_KWH;
  }
  printDebugInfo("Load Thresholds", true);
}

// Update balance based on energy consumption - No changes needed, looks good
void updateBalance() {
  if (!readings.isValid) return;

  float energyUsed = readings.energy - lastEnergy;

  // If energy counter was reset or first reading
  if (energyUsed < 0) {
    energyUsed = readings.energy;
  }

  // Calculate cost
  float cost = energyUsed * costPerKWh;

  // Deduct from balance if relay is on and not manually overridden off
  if (relayState && !manualOverride && cost > 0) {
    balance -= cost;

    // Ensure balance doesn't go negative
    if (balance < 0) {
      balance = 0;
    }

    printDebugInfo("Balance Updated", true);
  }

  // Update last energy reading
  lastEnergy = readings.energy;
}

// Check for electrical faults and theft - No changes needed, looks good
void checkFaultsAndTheft() {
  if (!readings.isValid) return;

  // Check for faults (over voltage or over current)
  bool newFaultState = (readings.voltage > overVoltageThreshold || readings.current > overCurrentThreshold);

  // Only show notification if fault state changes
  if (newFaultState != faultDetected) {
    faultDetected = newFaultState;
    if (faultDetected) {
      showNotification("Fault Detected!", RED);
    } else {
      showNotification("Fault Cleared", GREEN);
    }
  }

  // Check for theft (current flowing when relay is off)
  bool newTheftState = (!relayState && readings.current > theftCurrentThreshold);

  // Only show notification if theft state changes
  if (newTheftState != theftDetected) {
    theftDetected = newTheftState;
    if (theftDetected) {
      showNotification("Theft Detected!", RED);
    } else {
      showNotification("Theft Alert Cleared", GREEN);
    }
  }
}

// Control relay based on balance and faults - Modified to respect manual override
void controlRelay() {
  if (manualOverride) return;  // Do not auto-control relay if manually overridden

  bool shouldBeOn = (balance > minimumBalance && !faultDetected);

  // Only change relay state if needed
  if (relayState != shouldBeOn) {
    relayState = shouldBeOn;
    digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);

    if (relayState) {
      showNotification("Power Enabled", GREEN);
    } else {
      showNotification("Power Disabled", RED);
    }
  }
}

// Print debug information to serial - No changes needed, looks good
void printDebugInfo(const char* action, bool success) {
  Serial.print("[");
  Serial.print(action);
  Serial.print("] ");
  if (success) {
    Serial.println("Success");
  } else {
    Serial.println("Failed");
  }
}

// Web server handlers - No changes needed, looks good
void handleRoot() {
  if (!server.authenticate(admin_username, admin_password)) {
    return server.requestAuthentication();
  }
  server.send(200, "text/html", index_html);
}

void handleLogin() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  if (server.arg("username") == admin_username && server.arg("password") == admin_password) {
    server.sendHeader("Location", "/");
    server.send(303);
  } else {
    server.sendHeader("Location", "/login?error=1");
    server.send(303);
  }
}

void handleRecharge() {
  if (!server.authenticate(admin_username, admin_password)) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  if (server.hasArg("amount")) {
    float amount = server.arg("amount").toFloat();
    if (amount > 0) {
      balance += amount;
      saveBalance();

      // Create JSON response
      String response = "{\"success\":true,\"balance\":" + String(balance, 2) + "}";
      server.send(200, "application/json", response);

      showNotification("Recharged: Rs" + String(amount, 2), GREEN);
      return;
    }
  }

  server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid amount\"}");
}

void handleData() {
  if (!server.authenticate(admin_username, admin_password)) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  // Create JSON document
  DynamicJsonDocument doc(1024);

  // Add readings
  doc["isValid"] = readings.isValid;
  doc["voltage"] = readings.voltage;
  doc["current"] = readings.current;
  doc["power"] = readings.power;
  doc["energy"] = readings.energy;

  // If reading is invalid, send 0 values for web dashboard to clear values
  if (!readings.isValid) {
    doc["voltage"] = 0.0;
    doc["current"] = 0.0;
    doc["power"] = 0.0;
    doc["energy"] = 0.0;
  }

  // Add status information
  doc["balance"] = balance;
  doc["relayState"] = relayState;
  doc["faultDetected"] = faultDetected;
  doc["theftDetected"] = theftDetected;
  doc["overVoltageThreshold"] = overVoltageThreshold;
  doc["overCurrentThreshold"] = overCurrentThreshold;
  doc["theftCurrentThreshold"] = theftCurrentThreshold;
  doc["minimumBalance"] = minimumBalance;
  doc["costPerKWh"] = costPerKWh;
  doc["manualOverride"] = manualOverride;  // Send manual override status


  // Serialize JSON to string
  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);
}

void handleReset() {
  if (!server.authenticate(admin_username, admin_password)) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  // Reset energy counter on PZEM
  bool success = pzem.resetEnergy();

  // Reset stored energy value
  lastEnergy = 0.0;
  saveBalance();

  String response = "{\"success\":" + String(success ? "true" : "false") + "}";
  server.send(200, "application/json", response);

  if (success) {
    showNotification("Energy Counter Reset", YELLOW);
  } else {
    showNotification("Reset Failed", RED);
  }
}

void handleSetRelay() {
  if (!server.authenticate(admin_username, admin_password)) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  if (server.hasArg("state") && server.hasArg("override")) {
    int state = server.arg("state").toInt();
    int override_manual = server.arg("override").toInt();
    bool success = false;

    if (state == 0) {
      relayState = false;
      digitalWrite(RELAY_PIN, HIGH);
      success = true;
      showNotification("Power Off", RED);
      manualOverride = (override_manual == 1);  // Set manual override based on parameter

    } else if (state == 1) {
      if (balance > minimumBalance && !faultDetected) {
        relayState = true;
        digitalWrite(RELAY_PIN, LOW);
        success = true;
        showNotification("Power On", GREEN);
        manualOverride = false;  // Clear manual override when turned on manually or automatically
      }
    }

    String response = "{\"success\":" + String(success ? "true" : "false") + "}";
    server.send(200, "application/json", response);
    return;
  }

  server.send(400, "application/json", "{\"success\":false,\"message\":\"Missing state parameter\"}");
}

void handleSetThresholds() {
  if (!server.authenticate(admin_username, admin_password)) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  DynamicJsonDocument doc(512);  // Adjust size as needed
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
    return;
  }

  if (doc.containsKey("overVoltage") && doc.containsKey("overCurrent") && doc.containsKey("theftCurrent") && doc.containsKey("minBalance") && doc.containsKey("costPerKWh")) {
    overVoltageThreshold = doc["overVoltage"].as<float>();
    overCurrentThreshold = doc["overCurrent"].as<float>();
    theftCurrentThreshold = doc["theftCurrent"].as<float>();
    minimumBalance = doc["minBalance"].as<float>();
    costPerKWh = doc["costPerKWh"].as<float>();

    saveThresholds();  // Save new thresholds to EEPROM

    String response = "{\"success\":true}";
    server.send(200, "application/json", response);
    showNotification("Thresholds Updated", GREEN);
    return;
  } else {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Missing parameters\"}");
    return;
  }
}

void handleFactoryReset() {
  if (!server.authenticate(admin_username, admin_password)) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  resetEEPROMToDefaults();                                 // Reset EEPROM values to defaults
  showNotification("Factory Resetting...", YELLOW, 2000);  // Notify user on TFT
  delay(2000);                                             // Short delay to show notification
  server.send(200, "application/json", "{\"success\":true}");
  ESP.restart();  // Restart ESP32 to apply defaults immediately
}

void resetEEPROMToDefaults() {
  balance = DEFAULT_BALANCE;
  lastEnergy = DEFAULT_ENERGY;
  overVoltageThreshold = DEFAULT_OVER_VOLTAGE_THRESHOLD;
  overCurrentThreshold = DEFAULT_OVER_CURRENT_THRESHOLD;
  theftCurrentThreshold = DEFAULT_THEFT_CURRENT_THRESHOLD;
  minimumBalance = DEFAULT_MINIMUM_BALANCE;
  costPerKWh = DEFAULT_COST_PER_KWH;

  saveBalance();
  saveThresholds();

  // Optionally clear the entire EEPROM (be cautious, this is usually not needed and can reduce EEPROM life if done excessively)
  // for (int i = 0; i < EEPROM_SIZE; i++) {
  //     EEPROM.write(i, 0);
  // }
  // EEPROM.commit();
  printDebugInfo("EEPROM Reset to Defaults", true);
}


void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}
