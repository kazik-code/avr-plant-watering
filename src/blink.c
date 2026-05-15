#include <stdint.h>
#include <avr/io.h>

int main(void) {
    // Set pin 5 of PORTB as output
    DDRB |= (1 << DDB5);

    while (1) {
        // Toggle pin 5 of PORTB
        PORTB ^= (1 << PORTB5);
        // Simple delay loop
        for (volatile uint32_t i = 0; i < 10000; i++);
    }

    return 0;
}