#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Configuración I2C
#define I2C_ADDR 0x3C          // Dirección típica del controlador SSD1306 para I2C
#define OLED_SDA_PIN 4         // Pin SDA para I2C
#define OLED_SCL_PIN 5         // Pin SCL para I2C
#define OLED_I2C_SPEED 100000  // Frecuencia de la comunicación I2C (100 kHz)

// Dimensiones de la pantalla OLED
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// Buffer de datos para la pantalla OLED
uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

// Definición de caracteres en formato 5x7
const uint8_t font[52][5] = {
    {0x7E, 0x09, 0x09, 0x09, 0x7E}, // de la A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, 
    {0x3E, 0x41, 0x41, 0x41, 0x22}, 
    {0x7F, 0x41, 0x41, 0x41, 0x3E}, 
    {0x7F, 0x49, 0x49, 0x49, 0x41}, 
    {0x7F, 0x09, 0x09, 0x09, 0x01}, 
    {0x3E, 0x41, 0x49, 0x49, 0x2E}, 
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, 
    {0x41, 0x41, 0x7F, 0x41, 0x41}, 
    {0x20, 0x40, 0x40, 0x40, 0x3F}, 
    {0x7F, 0x08, 0x14, 0x22, 0x41}, 
    {0x7F, 0x40, 0x40, 0x40, 0x40}, 
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, 
    {0x7F, 0x02, 0x04, 0x08, 0x7F}, 
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, 
    {0x7F, 0x09, 0x09, 0x09, 0x06}, 
    {0x3E, 0x41, 0x49, 0x49, 0x3E}, 
    {0x7F, 0x09, 0x19, 0x29, 0x46}, 
    {0x46, 0x49, 0x49, 0x49, 0x31}, 
    {0x01, 0x01, 0x7F, 0x01, 0x01}, 
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, 
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, 
    {0x3F, 0x40, 0x30, 0x40, 0x3F}, 
    {0x63, 0x14, 0x08, 0x14, 0x63}, 
    {0x31, 0x40, 0x40, 0x40, 0x31}, 
    {0x71, 0x49, 0x45, 0x43, 0x71}, // hasta la Z
    {0x3C, 0x4A, 0x4A, 0x4A, 0x30}, // de la a
    {0x7F, 0x44, 0x44, 0x44, 0x38}, 
    {0x38, 0x44, 0x44, 0x44, 0x28}, 
    {0x38, 0x44, 0x44, 0x44, 0x7F}, 
    {0x38, 0x54, 0x54, 0x54, 0x18}, 
    {0x04, 0x7E, 0x05, 0x01, 0x02}, 
    {0x18, 0xA4, 0xA4, 0xA4, 0x7C}, 
    {0x7F, 0x08, 0x08, 0x08, 0x70}, 
    {0x00, 0x44, 0x7D, 0x40, 0x00}, 
    {0x40, 0x80, 0x80, 0x7A, 0x00}, 
    {0x7F, 0x10, 0x28, 0x44, 0x00}, 
    {0x41, 0x7F, 0x40, 0x00, 0x00}, 
    {0x7C, 0x04, 0x78, 0x04, 0x78}, 
    {0x7C, 0x08, 0x04, 0x04, 0x78}, 
    {0x38, 0x44, 0x44, 0x44, 0x38}, 
    {0xFC, 0x18, 0x24, 0x24, 0x18}, 
    {0x18, 0x24, 0x24, 0x18, 0xFC}, 
    {0x7C, 0x08, 0x04, 0x04, 0x08}, 
    {0x48, 0x54, 0x54, 0x54, 0x20}, 
    {0x04, 0x3F, 0x44, 0x40, 0x20}, 
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, 
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, 
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, 
    {0x44, 0x28, 0x10, 0x28, 0x44}, 
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, 
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // hasta la z
};

// Inicialización del protocolo I2C
void init_i2c() {
    i2c_init(i2c0, OLED_I2C_SPEED);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);
}

// Función para enviar comandos a la pantalla OLED
void oled_send_command(uint8_t command) {
    uint8_t data[2] = {0x00, command};  // 0x00 indica que es un comando
    i2c_write_blocking(i2c0, I2C_ADDR, data, 2, false);
}

// Función para enviar datos al controlador OLED
void oled_send_data(uint8_t *data, size_t length) {
    uint8_t buffer[length + 1];
    buffer[0] = 0x40;  // 0x40 indica que son datos
    for (size_t i = 0; i < length; i++) {
        buffer[i + 1] = data[i];
    }
    i2c_write_blocking(i2c0, I2C_ADDR, buffer, length + 1, false);
}

// Función para inicializar la pantalla OLED
void oled_init() {
    sleep_ms(100);  // Espera para que la pantalla se estabilice

    // Secuencia de comandos para inicializar el SSD1306
    oled_send_command(0xAE); // Apaga la pantalla
    oled_send_command(0x20); // Modo de memoria
    oled_send_command(0x00); // Modo horizontal
    oled_send_command(0xB0); // Dirección de la página
    oled_send_command(0xC8); // Configura la dirección de escaneo de COM
    oled_send_command(0x00); // Columna baja
    oled_send_command(0x10); // Columna alta
    oled_send_command(0x40); // Línea de inicio
    oled_send_command(0x81); // Configura el contraste
    oled_send_command(0xFF); // Nivel máximo de contraste
    oled_send_command(0xA1); // Remap de segmento
    oled_send_command(0xA6); // Modo de pantalla normal
    oled_send_command(0xA8); // Multiplexado de pantalla
    oled_send_command(0x3F); // 1/64 duty ratio
    oled_send_command(0xA4); // Activa la pantalla a partir de la RAM
    oled_send_command(0xD3); // Configura el desplazamiento vertical
    oled_send_command(0x00); // Sin desplazamiento
    oled_send_command(0xD5); // Configuración del reloj
    oled_send_command(0xF0); // Ajuste de frecuencia del oscilador
    oled_send_command(0xD9); // Configura el período de pre-carga
    oled_send_command(0x22); 
    oled_send_command(0xDA); // Configuración de hardware de pantalla
    oled_send_command(0x12);
    oled_send_command(0xDB); // Configuración de nivel de descarga
    oled_send_command(0x20); 
    oled_send_command(0x8D); // Habilita la bomba de carga de voltaje
    oled_send_command(0x14);
    oled_send_command(0xAF); // Enciende la pantalla
}

// Limpia el buffer de la pantalla
void oled_clear_buffer() {
    for (int i = 0; i < sizeof(display_buffer); i++) {
        display_buffer[i] = 0x00;
    }
}

// Muestra el contenido del buffer en la pantalla
void oled_update_display() {
    for (uint8_t page = 0; page < 8; page++) {
        oled_send_command(0xB0 + page); // Página actual
        oled_send_command(0x00); // Columna baja
        oled_send_command(0x10); // Columna alta
        oled_send_data(&display_buffer[DISPLAY_WIDTH * page], DISPLAY_WIDTH);
    }
}

// Dibuja un carácter en el buffer en la posición (x, y)
void oled_draw_char(char c, int x, int y) {
    if (c < 'A' || (c > 'Z' && c < 'a') || c > 'z') return; // Solo letras A-Z, a-z
    int index;
    if (c >= 'A' && c <= 'Z') {
        index = c - 'A'; // Índice para mayúsculas
    } else {
        index = c - 'a' + 26; // Índice para minúsculas
    }
    for (int i = 0; i < 5; i++) {
        uint8_t col = font[index][i];
        for (int j = 0; j < 8; j++) {
            if (col & (1 << j)) {
                display_buffer[(x + i) + ((y + j) / 8) * DISPLAY_WIDTH] |= (1 << (y + j) % 8);
            }
        }
    }
}

// Muestra un mensaje en la pantalla OLED
void oled_show_text(const char *text, int x, int y) {
    int posX = x;
    while (*text) {
        oled_draw_char(*text++, posX, y);
        posX += 6; // Separación entre caracteres
        if (posX >= DISPLAY_WIDTH - 6) {
            break; // Previene desbordamientos de texto
        }
    }
}

// Función principal
int main() {
    stdio_init_all();  // Inicializa entrada y salida estándar
    init_i2c();        // Inicializa I2C
    oled_init();       // Inicializa la pantalla OLED
    oled_clear_buffer(); // Limpia el contenido del buffer

    // Muestra el nombre en la pantalla OLED
    oled_show_text("Noe Mendoza R", 0, 16);
    oled_update_display(); // Envía el buffer a la pantalla para su visualización

    while (1) {
        // Bucle infinito para mantener el programa activo
    }

    return 0;
}
