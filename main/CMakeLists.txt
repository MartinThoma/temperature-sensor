# Get WiFi credentials from environment variables
set(WIFI_SSID $ENV{WIFI_SSID})
set(WIFI_PASS $ENV{WIFI_PASS})

# Check if environment variables are set
if(NOT WIFI_SSID OR NOT WIFI_PASS)
  message(
    FATAL_ERROR
      "WiFi credentials not set. Please source .envrc or set WIFI_SSID and WIFI_PASS environment variables."
  )
endif()

idf_component_register(SRCS "main.c" "dht11.c" INCLUDE_DIRS ".")

# Add compile definitions for WiFi credentials
target_compile_definitions(${COMPONENT_LIB} PRIVATE WIFI_SSID="${WIFI_SSID}"
                                                    WIFI_PASS="${WIFI_PASS}")
