/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file board_logger_topics.h
 *
 * GFF4 logger default topic list (CONFIG_LOGGER_BOARD_TOPICS).
 *
 * Included inside LoggedTopics::add_default_topics() in src/modules/logger/logged_topics.cpp,
 * so it may only contain add_topic*() calls. No include guard on purpose.
 *
 * FPV multicopter with 192 KiB RAM: the full default list has 133 subscriptions and most of
 * them belong to other airframes (rover, fixed wing, VTOL, gimbal, camera, ...) and never
 * publish here.  Each subscription costs RAM, and logging with the full list keeps the heap
 * above COM_RAM_MAX (95%), which blocks arming.  Log only what is needed to tune an FPV
 * acro/stabilized multicopter, plus position topics for GPS position-mode analysis.
 */

// vehicle state / safety
add_topic("vehicle_status");
add_topic("vehicle_control_mode");
add_topic("actuator_armed");
add_topic("failsafe_flags");
add_topic("commander_state");
add_topic("vehicle_land_detected");
add_topic("vehicle_command");
add_topic("vehicle_command_ack");
add_topic("parameter_update");
add_topic("cpuload");
add_topic("system_power", 500);
add_topic("failure_detector_status", 100);

// IMU / sensors (vibration and filter tuning)
add_topic("sensor_combined");
add_topic("sensors_status_imu", 200);
add_topic("sensor_selection");
add_topic_multi("vehicle_imu_status", 1000, 1);
add_topic("vehicle_air_data", 200);
add_topic("vehicle_gps_position", 100);
add_topic_multi("battery_status", 200, 1);

// attitude / rates / control (PID tuning)
add_topic("vehicle_attitude", 50);
add_topic("vehicle_attitude_setpoint", 50);
add_topic("vehicle_angular_velocity", 20);
add_topic("vehicle_acceleration", 50);
add_topic("vehicle_rates_setpoint", 20);
add_topic_multi("vehicle_thrust_setpoint", 20, 1);
add_topic_multi("vehicle_torque_setpoint", 20, 1);
add_optional_topic_multi("rate_ctrl_status", 200, 1);
add_optional_topic_multi("control_allocator_status", 200, 1);
add_optional_topic("actuator_controls_status_0", 300);

// outputs
add_topic("actuator_motors", 100);
add_optional_topic_multi("actuator_outputs", 100, 1);

// pilot input
add_topic("input_rc", 500);
add_topic("manual_control_setpoint", 200);
add_topic("manual_control_switches");

// estimator health (kept small)
add_optional_topic_multi("estimator_status", 200, 1);
add_optional_topic_multi("estimator_status_flags", 10, 1);
add_optional_topic_multi("estimator_event_flags", 10, 1);
// position / navigation (GPS position-mode analysis; official rates; ~10 more subscriptions)
add_topic("vehicle_local_position", 100);
add_topic("vehicle_global_position", 200);
add_topic("vehicle_local_position_setpoint", 100);
add_optional_topic("trajectory_setpoint", 200);
add_optional_topic("position_setpoint_triplet", 200);
add_topic("home_position");
add_optional_topic("navigator_status");
add_optional_topic("mission_result");
add_topic_multi("sensor_gps", 1000, 1);
add_optional_topic_multi("estimator_sensor_bias", 1000, 1);
