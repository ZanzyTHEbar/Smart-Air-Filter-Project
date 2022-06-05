#include "speaker.hpp"

Speaker::Speaker() : buzzer_pin(10), _itchy("itchy:d=8,o=6,b=160:c,a5,4p")
{
    // Setup the buzzer
    pinMode(buzzer_pin, OUTPUT);
    digitalWrite(buzzer_pin, LOW);
}

Speaker::~Speaker()
{
}

Speaker speaker;