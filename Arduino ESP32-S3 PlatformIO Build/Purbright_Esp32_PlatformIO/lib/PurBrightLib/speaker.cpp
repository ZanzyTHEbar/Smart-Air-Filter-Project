#include "speaker.hpp"

Speaker::Speaker()
{
}

Speaker::~Speaker()
{
}

void Speaker::SetupSpeaker()
{
    pinMode(BUZZER_PIN, OUTPUT);
}

Speaker speaker;