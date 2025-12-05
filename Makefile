# ============================================
# Configuration du projet Yubino
# ============================================
TARGET := yubino
MCU := atmega328p
F_CPU := 16000000UL
PORT := COM7
PROGRAMMER := arduino
BAUD := 115200

# ============================================
# Fichiers sources
# ============================================
SRCS := main.c uart.c buffer.c button.c led.c crypto.c eeprom_registry.c
SRCS += micro-ecc/uECC.c

LIBS :=

# ============================================
# Flags de compilation
# ============================================
WARNINGS := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wredundant-decls -Winline -Wno-long-long

CFLAGS := -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) $(WARNINGS)
CFLAGS += -std=gnu99
CFLAGS += -mcall-prologues
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fpack-struct -fshort-enums
CFLAGS += -funsigned-char -funsigned-bitfields

# Flags micro-ecc - VERSION C PURE
CFLAGS += -DuECC_PLATFORM=uECC_arch_other
CFLAGS += -DuECC_CURVE=uECC_secp160r1
CFLAGS += -DuECC_SQUARE_FUNC=0
CFLAGS += -DuECC_OPTIMIZATION_LEVEL=0
CFLAGS += -DuECC_SUPPORTS_secp160r1=1

# ============================================
# Flags de linkage
# ============================================
LDFLAGS := -mmcu=$(MCU)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(TARGET).map

# ============================================
# Outils
# ============================================
CC := avr-gcc
OBJCOPY := avr-objcopy
SIZE := avr-size
AVRDUDE := avrdude

# ============================================
# Règles
# ============================================
all: $(TARGET).hex size

$(TARGET).elf: $(SRCS) $(LIBS)
	@echo "=== Compilation ==="
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS) $(LIBS)
	@echo "✓ Compilation OK"

$(TARGET).hex: $(TARGET).elf
	@echo "=== Création HEX ==="
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo "✓ HEX créé"

# Affiche la taille (version la plus simple)
size: $(TARGET).elf
	@echo ""
	@echo "=== Taille du programme ==="
	@$(SIZE) $(TARGET).elf
	@echo ""

# Affiche taille détaillée
size-detail: $(TARGET).elf
	@echo "=== Détail des sections ==="
	@$(SIZE) -A $(TARGET).elf

# Upload vers Arduino
flash: $(TARGET).hex
	@echo "=== Upload vers $(PORT) ==="
	$(AVRDUDE) -v -p$(MCU) -c$(PROGRAMMER) -P$(PORT) -b$(BAUD) -D -Uflash:w:$<:i
	@echo "✓ Upload OK"

# Nettoyage
clean:
	@echo "=== Nettoyage ==="
	rm -f *.elf *.hex *.map *.o micro-ecc/*.o
	@echo "✓ Nettoyé"

# Recompilation complète
rebuild: clean all

# Test de connexion
test:
	@echo "=== Test connexion ==="
	$(AVRDUDE) -p$(MCU) -c$(PROGRAMMER) -P$(PORT) -v

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make              - Compile le projet"
	@echo "  make flash        - Compile et upload"
	@echo "  make clean        - Nettoie les fichiers"
	@echo "  make rebuild      - Recompile tout"
	@echo "  make size         - Affiche la taille"
	@echo "  make size-detail  - Affiche détails"
	@echo "  make test         - Test connexion"
	@echo ""
	@echo "Config: MCU=$(MCU) PORT=$(PORT)"

.PHONY: all clean rebuild flash size size-detail test help