#include "tos_k.h"
#include "bsp_pm_device.h"
#include "bsp_tickless_alarm.h"

int tos_bsp_tickless_setup(void)
{
#if TOS_CFG_TICKLESS_EN > 0u
    tos_pm_device_register(&pm_device_uart);
		
		#if 1
			// sleep模式下的唤醒源，基本定时器
			tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_SLEEP, &tickless_wkup_alarm_tim);
			// 初始化唤醒源闹钟
			tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_SLEEP);
			// 设置tickless状态时进入sleep模式
			tos_pm_cpu_lpwr_mode_set(TOS_LOW_POWER_MODE_SLEEP);
	  #else
			/* we set a default one shot timer here(systick) */
			// tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_SLEEP, &tickless_wkup_alarm_systick);
			tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_SLEEP, &tickless_wkup_alarm_tim);
			tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_SLEEP);

			tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_STOP, &tickless_wkup_alarm_rtc_wkupirq);
			tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_STOP);

			tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_STANDBY, &tickless_wkup_alarm_rtc_alarmirq);
			tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_STANDBY);

			tos_pm_cpu_lpwr_mode_set(TOS_LOW_POWER_MODE_SLEEP);
	  #endif
#endif

    return 0;
}

