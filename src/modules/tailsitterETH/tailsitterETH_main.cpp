#include "tailsitterETH.hpp"

using namespace time_literals;

tailsitterETH::tailsitterETH():ModuleParams(nullptr),
	WorkItem(MODULE_NAME, px4::wq_configurations::rate_ctrl),
	_loop_perf(perf_alloc(PC_ELAPSED, "tailsiiter_global_control"))
{

}

tailsitterETH::~tailsitterETH()
{
	perf_free(_loop_perf);
}

int tailsitterETH::task_spawn(int argc, char *argv[])
{
	tailsitterETH *instance = new tailsitterETH();

	if(instance){
		_object.store(instance);
		_task_id = task_id_is_work_queue;

		if(instance->init()){
			return PX4_OK;
		}
	}else {
		PX4_ERR("alloc failed");
	}

	delete instance;
	_object.store(nullptr);
	_task_id = -1;

	return PX4_ERROR;
}

bool
tailsitterETH::init()
{
	if(!_local_pos_sub.registerCallback()){
		PX4_ERR("vehicle_local_position callback registration failed!");
		return false;
	}

	_local_pos_sub.set_interval_ms(20_ms);

	_time_stamp_last_loop = hrt_absolute_time();

	return true;
}

int tailsitterETH::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int tailsitterETH::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}
	return 0;
}

void tailsitterETH::Run()
{
	if (should_exit()) {
		_local_pos_sub.unregisterCallback();
		exit_and_cleanup();
		return;
	}

	perf_begin(_loop_perf);

	perf_end(_loop_perf);
}

int tailsitterETH_main(int argc, char *argv[])
{
	return tailsitterETH::main(argc, argv);
}
