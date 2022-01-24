#include "speaker.hpp"

Speaker::Speaker()
{
    // SetupSpeaker();
    // InitSpeakerJson();
}

Speaker::~Speaker()
{
}

void Speaker::SetupSpeaker()
{
    pinMode(BUZZER_PIN, OUTPUT);

}