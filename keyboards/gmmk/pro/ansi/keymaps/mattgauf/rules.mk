# Build rules
LIB_SRC += keymap_helpers.c
LIB_SRC += ansi.c

# Overrides
MOUSEKEY_ENABLE = yes           # Mouse keys
EXTRAKEY_ENABLE = yes           # Audio control and System control
CONSOLE_ENABLE = no             # Console for debug
COMMAND_ENABLE = no             # Commands for debug and configuration
NKRO_ENABLE = yes               # USB Nkey Rollover
RAW_ENABLE = yes                # USB HID
BACKLIGHT_ENABLE = no           # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no            # Enable keyboard RGB underglow
MIDI_ENABLE = no                # MIDI support
UNICODE_ENABLE = no             # Unicode
BLUETOOTH_ENABLE = no           # Enable Bluetooth
AUDIO_ENABLE = no               # Audio output
ENCODER_ENABLE = yes            # Rotary encoder support
DIGITIZER_ENABLE = no           # Digitizer support
DYNAMIC_MACRO_ENABLE = yes      # Dynamic macro support
DEBOUNCE_TYPE = sym_defer_pk    # Per key debouncing
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = AW20216

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no           # Breathing sleep LED during USB suspend
