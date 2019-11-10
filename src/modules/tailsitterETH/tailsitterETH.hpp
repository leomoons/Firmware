/* created in 2019.11.10
*  By liyuanhao
*/

#pragma once

#include <drivers/drv_hrt.h>
#include <px4_config.h>
#include <px4_defines.h>
#include <px4_module.h>
#include <px4_module_params.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_posix.h>
#include <px4_tasks.h>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionCallback.hpp>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_rates_setpoint.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/home_position.h>


extern "C" __EXPORT int tailsitterETH_main(int argc, char *argv[]);

// ModuleParams应该是继承里面关于参数更新和修改的函数
class tailsitterETH : public ModuleBase<tailsitterETH>, public ModuleParams,
	public px4::WorkItem
{
public:
	tailsitterETH();

	virtual ~tailsitterETH();

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	void Run() override;

	bool init();


private:

	uORB::SubscriptionCallbackWorkItem _local_pos_sub{this, ORB_ID(vehicle_local_position)};	/**< vehicle local position */

	uORB::Subscription _home_pos_sub{ORB_ID(home_position)};
	uORB::Subscription _v_att_sub{ORB_ID(vehicle_attitude)};
	uORB::Subscription _v_att_sp_sub{ORB_ID(vehicle_attitude_setpoint)};		/**< vehicle attitude setpoint subscription */
	uORB::Subscription _v_rates_sp_sub{ORB_ID(vehicle_rates_setpoint)};

	struct vehicle_attitude_s		_v_att {};		/**< vehicle attitude */
	struct vehicle_attitude_setpoint_s	_v_att_sp {};		/**< vehicle attitude setpoint */
	struct vehicle_rates_setpoint_s		_v_rates_sp {};		/**< vehicle rates setpoint */
	struct vehicle_local_position_s 	_local_pos{};		/**< vehicle local position */
	struct home_position_s			_home_pos{};		/**< home position */
	perf_counter_t _loop_perf;	/**< loop performance counter */

	hrt_abstime 	_time_stamp_last_loop{0};		/**< time stamp of last loop iteration */

};
