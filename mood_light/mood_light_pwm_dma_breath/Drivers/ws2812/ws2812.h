#ifndef __WS2812__H__
#define __WS2812__H__

#define WS2812_Num 60		//µÆÖéÓÐ¼¸¸ö
#define WS2812_Code_0 (32u)
#define WS2812_Code_1 (71u)

void WS2812_Start(void);
void WS2812_Send(void);
void light_mode_breath(void);
void light_mode_marquee(void);


#endif /*__WS2812__H__*/
