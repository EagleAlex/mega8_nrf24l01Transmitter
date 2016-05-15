
#define SBR(port, bit)        port |= (1<<bit)
#define CBR(port, bit)        port &= (~(1<<bit))
#define INV(port, bit)        port ^= (1<<bit)
#define BitIsClear(port, bit)      ((port & (1<<bit)) == 0)
#define BitIsSet(port, bit)      ((port & (1<<bit)) != 0)

#define SetBit(x,y) x |= (1<<(y))
#define ClearBit(x,y) x &= (~(1<<(y)))

//#define IncCounter(Var, LowVal, HiVal)   if (Var < HiVal) Var++; else Var = LowVal;

//#define DecCounter(Var, LowVal, HiVal)   if (Var > LowVal) Var--; else Var = HiVal;
