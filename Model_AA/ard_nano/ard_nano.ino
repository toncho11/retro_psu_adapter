/*
 * VAXstation 3100 ATX / DCOK controller
 *
 * WARNING: This firmware uses ATmega328P-specific low-level registers
 * and interrupts (PINB, PORTB, PCINT0_vect). It may not work on
 * other Arduino variants without modification. Verify compatibility
 * and DCOK behavior before use.
 *
 * D11 = ATX PWR_OK
 * A0  = VAX +5V sense (47k / 10k divider)
 * D12 = DCOK control
 * D3  = red fault LED
 *
 * D12 HIGH -> DCOK LOW
 * D12 LOW  -> DCOK HIGH
 *
 * Green LED follows actual DCOK through Q2.
 *
 * Logic:
 * - Startup requires PWR_OK HIGH + 4.85..5.20 V for 1 second.
 * - Once running, valid TAP voltage is 4.80..5.25 V.
 * - PWR_OK falling edge drops DCOK immediately by interrupt.
 * - <4.80 V or >5.25 V also drops DCOK and starts recovery.
 * - A runtime fault turns the red LED ON for a maximum of 5 seconds.
 * - Recovery waits 1 second, then repeats startup qualification.
 * - The red LED turns OFF after 5 seconds even if the fault persists.
 * - Another warning requires a successful return to RUNNING and a new fault.
 *
 * The green LED indicates the DCOK status. The red LED indicates a loss of
 * PWR_OK or an invalid +5 V supply voltage. It provides a short visible warning
 * so the user can notice a power interruption. The controller cannot distinguish
 * a genuine power fault from an intentional PSU shutdown, so either event can
 * trigger the warning. The LED turns OFF after 5 seconds regardless of whether
 * power has recovered; a continuing fault does not keep it illuminated.
 *
 * My Arduino Nano clone required:
 *  - A USB data cable (not a charge-only cable).
 *  - Older CH340/CH341 driver CH341SER 3.7.2022.1 (bug 13148652511260):
 *    https://support.arduino.cc/hc/en-us/articles/13148652511260-avrdude-ser-open-can-t-set-com-state-for-COMn
 *  - Select "ATmega328P (Old Bootloader)".
 */

const byte PWR_OK = 11;
const byte VAX5 = A0;
const byte DCOK = 12;
const byte RED_LED = 3;

// Default for 47k/10k divider + internal ~1.1 V reference.
// Calibrate: SCALE = multimeter_voltage / ADC_count
const float SCALE = 0.006001;

const float VMIN_BAD  = 4.80;
const float VMIN_GOOD = 4.85;
const float VMAX_GOOD = 5.20;
const float VMAX_BAD  = 5.25;
const unsigned long START_MS = 1000;
const unsigned long red_led_on_fault_duration_ms = 5000; // Maximum red LED warning time per fault.
const unsigned long recovery_pause_ms = 1000;             // Wait before restarting startup validation.

bool running = false;
bool fault = false;
bool voltageGood = false;
bool redWarningActive = false;
volatile bool pgDropped = false;
const bool SERIAL_DEBUG = true; // Enable or disable serial diagnostics every 250 ms

unsigned long goodSince = 0;
unsigned long faultSince = 0;


/* D11 = PB3/PCINT3, D12 = PB4 */
ISR(PCINT0_vect)
{
    if (!(PINB & _BV(PB3))) {
        PORTB |= _BV(PB4);   // D12 HIGH -> Q1 ON -> DCOK LOW
        pgDropped = true;
    }
}


float readVax5(int &adc)
{
    long sum = 0;

    for (byte i = 0; i < 16; i++) {
        sum += analogRead(VAX5);
        delayMicroseconds(200);
    }

    adc = (sum + 8) >> 4;    // rounded average of 16 samples
    return adc * SCALE;
}


void setDCOK(bool on)
{
    digitalWrite(DCOK, on ? LOW : HIGH);
}


void updateVoltageState(float v)
{
    if (voltageGood) {
        if (v < VMIN_BAD || v > VMAX_BAD)
            voltageGood = false;
    }
    else {
        if (v >= VMIN_GOOD && v <= VMAX_GOOD)
            voltageGood = true;
    }
}


void setup()
{
    if (SERIAL_DEBUG)
      Serial.begin(9600);

    pinMode(PWR_OK, INPUT);

    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, LOW);

    analogReference(INTERNAL);
    delay(5);
    analogRead(VAX5);        // discard first ADC reading

    digitalWrite(DCOK, HIGH);
    pinMode(DCOK, OUTPUT);   // safe: Q1 ON -> DCOK LOW

    // Pin-change interrupt on D11 / PB3 / PCINT3.
    PCMSK0 |= _BV(PCINT3);
    PCIFR  |= _BV(PCIF0);
    PCICR  |= _BV(PCIE0);

    if (SERIAL_DEBUG)
      Serial.println("VAX controller ready");
}


void loop()
{
    int adc;
    float v = readVax5(adc);
    bool pg = digitalRead(PWR_OK);
    unsigned long now = millis();

    updateVoltageState(v);

    // pgDropped catches even a short PWR_OK falling pulse.
    bool error = !pg || !voltageGood || pgDropped;

    if (fault) {
        setDCOK(false);

        // After the recovery pause, return to the original startup checks.
        if (now - faultSince >= recovery_pause_ms) {
            fault = false;
            goodSince = 0;

            // A PWR_OK pulse during the pause cannot block a fresh start.
            if (pg && voltageGood)
                pgDropped = false;
        }
    }

    // Startup qualification.
    else if (!running) {
        setDCOK(false);

        if (error) {
            goodSince = 0;

            // A startup PWR_OK glitch restarts the 1 s qualification.
            if (pg && voltageGood)
                pgDropped = false;
        }
        else {
            if (!goodSince)
                goodSince = now;

            if (now - goodSince >= START_MS) {
                /*
                 * Release DCOK atomically with a final PWR_OK check.
                 * If PWR_OK falls afterwards, the ISR immediately
                 * drives D12 HIGH again.
                 */
                noInterrupts();

                if ((PINB & _BV(PB3)) && !pgDropped) {
                    PORTB &= ~_BV(PB4);   // D12 LOW -> DCOK HIGH
                    running = true;
                }
                else {
                    goodSince = 0;
                }

                interrupts();

                if (running)
                    Serial.println("*** DCOK ENABLED ***");
            }
        }
    }

    // Fault after successful startup.
    else if (error) {
        setDCOK(false);
        running = false;
        fault = true;
        faultSince = now;
        goodSince = 0;
        redWarningActive = true;
        digitalWrite(RED_LED, HIGH);

        Serial.println("*** POWER FAULT ***");
    }

    // One warning per runtime fault, lasting no longer than 5 seconds.
    // A persistent fault does not restart the timer. Only a new fault
    // after successful RUNNING can start another warning.
    if (redWarningActive &&
        now - faultSince >= red_led_on_fault_duration_ms)
        redWarningActive = false;

    digitalWrite(RED_LED, redWarningActive ? HIGH : LOW);

    // Diagnostics every 250 ms.
    static unsigned long lastPrint = 0;

    if (SERIAL_DEBUG && now - lastPrint >= 250) {
        lastPrint = now;

        Serial.print("ADC=");
        Serial.print(adc);

        Serial.print(" V=");
        Serial.print(v, 3);

        Serial.print(" PG=");
        Serial.print(pg ? "HIGH" : "LOW");

        Serial.print(" VGOOD=");
        Serial.print(voltageGood ? "YES" : "NO");

        Serial.print(" DCOK=");
        Serial.print(running ? "ON" : "OFF");

        Serial.print(" RED=");
        Serial.print(digitalRead(RED_LED) ? "ON" : "OFF");

        Serial.print(" FAULT=");
        Serial.println(fault ? "YES" : "NO");
    }
}
