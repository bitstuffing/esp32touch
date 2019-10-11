#ifndef MAX1726X_H
#define MAX1726X_H

int InitalizeFuelGauge();
void UpdateStatus();

typedef struct
{
    uint16_t RemainingCapacity;
    uint16_t StateOfChargePercentage;
    uint16_t EstimateTimeToEmpty;
    uint16_t FullCapRep;

} FuelGaugeReads_t;

#endif /* MAX1726X */