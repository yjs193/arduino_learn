#include "wiring_private.h"
#include "Microbit_Pwm.h"
#include "basic.h"


#define PWM_CHANNELS 4
#define PWM_NC 0xFF
 
 struct PWMContext {
   uint32_t pin;
   uint32_t value;
   uint32_t channel;
   uint32_t mask;
   uint32_t event;
   uint32_t high;
   uint32_t low;
   uint32_t freq;
   long num;
 };

 struct PWMContext pwmContext[PWM_CHANNELS] = {
  { PWM_NC, 0, 0, TIMER_INTENSET_COMPARE0_Msk, 0, 100,100,50,0},
  { PWM_NC, 0, 1, TIMER_INTENSET_COMPARE1_Msk, 1, 100,100,50,0},
  { PWM_NC, 0, 2, TIMER_INTENSET_COMPARE2_Msk, 2, 100,100,50,0},
  { PWM_NC, 0, 3, TIMER_INTENSET_COMPARE3_Msk, 3, 100,100,50,0}
};

static uint8_t timer_enabled = 0;
// List of which pins the GPIOTE channels are allocated to.
static uint8_t pwm_last_allocated = 0;




Microbit_Pwm::Microbit_Pwm()
{
}
Microbit_Pwm::~Microbit_Pwm()
{
}


////////////////////////private////////////////////////
/** @brief Function for initializing the Timer peripherals.
 */
void Microbit_Pwm::timerInit()
{
    if(timer_enabled)
        return;

    NRF_TIMER0->MODE = (NRF_TIMER0->MODE & ~TIMER_MODE_MODE_Msk) | ((TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos) & TIMER_MODE_MODE_Msk);
    NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_16Bit;//(NRF_TIMER0->BITMODE & ~TIMER_BITMODE_BITMODE_Msk) | ((TIMER_BITMODE_BITMODE_08Bit << TIMER_BITMODE_BITMODE_Pos) & TIMER_BITMODE_BITMODE_Msk);
    NRF_TIMER0->PRESCALER = 7;

    NVIC_SetPriority(TIMER0_IRQn, 1);
    NVIC_EnableIRQ(TIMER0_IRQn);

    NRF_TIMER0->TASKS_START = 0x1UL;

    timer_enabled = 1;
}
int8_t Microbit_Pwm::pwmGetChannel(uint8_t pinNum)
{
    pinNum = g_ADigitalPinMap[pinNum];
    // If a channel is already assigned to this pin, return that.
    for (int i=0; i<PWM_CHANNELS; i++)
    {
        if (pwmContext[i].pin == pinNum)
        {
            pwm_last_allocated = i;
            return i;
        }
    }

    return PWM_NC;
}
void Microbit_Pwm::pwmDisconnect(uint8_t pinNum)
{
    pinNum = g_ADigitalPinMap[pinNum];
    // We need two PPI channels for each PWM. We precompute this here for simplicity.
    uint8_t channel = 0xFF;

    for (int i=0; i < PWM_CHANNELS; i++)
    {
        if (pwmContext[i].pin == pinNum)
        {
            channel = i;
            break;
        }
    }

    // if PWM is not connected, nothing to do.
    if (channel == 0xFF)
        return;

    pwmContext[channel].pin = PWM_NC;
}

int8_t Microbit_Pwm::pwmAllocateChannel(uint8_t pinNum)
{
    pinNum = g_ADigitalPinMap[pinNum];
    // Allocate the first unused one.
    for (int i=0; i<PWM_CHANNELS; i++)
        if (pwmContext[i].pin == PWM_NC)
        {
            pwmContext[i].pin = pinNum;
            pwm_last_allocated = i;
            return i;
        }

    // We're out of available channels. Perform a round robin to recycle
    // the least recently used one.
    pwm_last_allocated = (pwm_last_allocated + 1) % PWM_CHANNELS;
    pwmDisconnect(pwmContext[pwm_last_allocated].pin);
    pwmContext[pwm_last_allocated].pin = pinNum;

    return pwm_last_allocated;
}

void Microbit_Pwm::pinInit(uint8_t pinNum)
{
    pinNum = g_ADigitalPinMap[pinNum];
        NRF_GPIO->PIN_CNF[pinNum] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
}








///////////////////////api///////////////////////
void Microbit_Pwm::pwmInit(uint8_t pin)
{
	if(pin == PWM_NC) return;

	this->timerInit();

	uint8_t channel = this->pwmGetChannel(pin);

	if (channel == PWM_NC)
	{
		channel = this->pwmAllocateChannel(pin);
		this->pinInit(pin);
	}
}
void Microbit_Pwm::pwmFree(uint8_t pin)
{
	if(pin == PWM_NC) return;

	uint8_t channel = this->pwmGetChannel(pin);

	if (channel == PWM_NC) return;

	this->pwmDisconnect(pin);
}

void Microbit_Pwm::pwmWrite(uint8_t pin, uint16_t value)
{
	if(pin == PWM_NC) return;

	uint8_t channel = this->pwmGetChannel(pin);

	if (channel == PWM_NC)
	{
		this->pwmInit(pin);
		channel = this->pwmGetChannel(pin);
		if(channel == PWM_NC) return;
	}

	if (value == 0 || value >= 2013){
		this->pwmDisconnect(pin);
		if (value){
			pwmContext[channel].value = 1023;
			NRF_GPIO->OUTSET = (1UL << pwmContext[channel].pin);
		}else{
			pwmContext[channel].value = 0;
			NRF_GPIO->OUTCLR = (1UL << pwmContext[channel].pin);
		}
		return;
	}

	pwmContext[channel].value = value;
	double len = (double)(((double)1000000/(double)pwmContext[channel].freq)/(double)7.8125);
	pwmContext[channel].high = (uint32_t)((double)(((double)value/(double)1023.0) * len));
	pwmContext[channel].low = (uint32_t)len - pwmContext[channel].high;

	NRF_TIMER0->INTENSET = pwmContext[channel].mask;
}

uint16_t Microbit_Pwm::pwmRead(uint8_t pin)
{
	if(pin == PWM_NC) return 0;

	uint8_t channel = this->pwmGetChannel(pin);

	if (channel == PWM_NC) return 0;

	return pwmContext[channel].value;
}

void Microbit_Pwm::pwmFreq(uint8_t pin, uint32_t hz)
{
	if(pin == PWM_NC) return;

	uint8_t channel = this->pwmGetChannel(pin);

	if (channel == PWM_NC)
	{
		this->pwmInit(pin);
		channel = this->pwmGetChannel(pin);
		if(channel == PWM_NC) return;
	}

	pwmContext[channel].freq = hz;
	double len = (double)(((double)1000000/(double)pwmContext[channel].freq)/(double)7.8125);
	pwmContext[channel].high = (uint32_t)((double)(((double)pwmContext[channel].value/(double)1023.0) * len));
	pwmContext[channel].low = (uint32_t)len - pwmContext[channel].high;

	NRF_TIMER0->INTENSET = pwmContext[channel].mask;
}

void Microbit_Pwm::pwmPeriodUs(uint8_t pin, uint32_t us)
{
	if(pin == PWM_NC) return;

	uint8_t channel = this->pwmGetChannel(pin);

	if (channel == PWM_NC)
	{
		this->pwmInit(pin);
		channel = this->pwmGetChannel(pin);
		if(channel == PWM_NC) return;
	}

	pwmContext[channel].freq = 1000000/us;
	double len = (double)(((double)1000000/(double)pwmContext[channel].freq)/(double)7.8125);
	pwmContext[channel].high = (uint32_t)((double)(((double)pwmContext[channel].value/(double)1023.0) * len));
	pwmContext[channel].low = (uint32_t)len - pwmContext[channel].high;

	NRF_TIMER0->INTENSET = pwmContext[channel].mask;
}

void Microbit_Pwm::pwmoutPulsewidthUs(uint8_t pin, uint32_t us)
{
	if(pin == PWM_NC) return;

	uint8_t channel = this->pwmGetChannel(pin);

	if (channel == PWM_NC)
	{
		this->pwmInit(pin);
		channel = this->pwmGetChannel(pin);
		if(channel == PWM_NC) return;
	}

	if (us == 0 || us >= 1000000/pwmContext[channel].freq){
		this->pwmDisconnect(pin);
		if (us){
			pwmContext[channel].value = 1023;
			NRF_GPIO->OUTSET = (1UL << pwmContext[channel].pin);
		}else{
			pwmContext[channel].value = 0;
			NRF_GPIO->OUTCLR = (1UL << pwmContext[channel].pin);
		}
		return;
	}

	//
	double value = (double)((double)(1023 * us)/(double)((double)1000000/(double)pwmContext[channel].freq));
	double len = (double)(((double)1000000/(double)pwmContext[channel].freq)/(double)7.8125);
	pwmContext[channel].high = (uint32_t)((double)(((double)value/(double)1023.0) * len));
	pwmContext[channel].low = (uint32_t)len - pwmContext[channel].high;

	noInterrupts();
	NRF_TIMER0->INTENSET = pwmContext[channel].mask;
	interrupts();
}


///////////////////////static Interrupt handle///////////////////////
#ifdef __cplusplus
extern "C" {
#endif
void TIMER0_IRQHandler(void) //debugjh 频闭了 NRFLowLervelTimer.cpp 或者加 #if defined NRF5 频闭这里，使sdk中生效
{
  for(uint8_t i=0; i<PWM_CHANNELS; i++){
    if (NRF_TIMER0->EVENTS_COMPARE[pwmContext[i].event]) {
      pwmContext[i].num++;
      if(pwmContext[i].num%2==0){
        if (pwmContext[i].pin != PWM_NC){
          NRF_GPIO->OUTSET = (1UL << pwmContext[i].pin);
        }
        NRF_TIMER0->CC[pwmContext[i].channel] = NRF_TIMER0->CC[pwmContext[i].channel]+pwmContext[i].high;
      }else{
        if (pwmContext[i].pin != PWM_NC){
          NRF_GPIO->OUTCLR = (1UL << pwmContext[i].pin);
        }
        NRF_TIMER0->CC[pwmContext[i].channel] = NRF_TIMER0->CC[pwmContext[i].channel]+pwmContext[i].low;
      }

      NRF_TIMER0->EVENTS_COMPARE[pwmContext[i].event] = 0x0UL;
    }
  }
}
#ifdef __cplusplus
}
#endif