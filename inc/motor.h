/**
 * @file motor.h
 * @author José Carvalho, João Carneiro
 * @brief Motor module header
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef MOTOR_H
#define MOTOR_H

/**
 * @brief Load the motor driver device driver
 */
void initMotor();

/**
 * @brief Remove the motor driver device driver
 */
void remMotor();

/**
 * @brief Start the motor
 * 
 * Writes to the device driver so that the motor
 * is enabled. 
 * 
 * @return int: Returns 0 on error, or 1 if sucessful
 */
int startMotor();

/**
 * @brief Stop the motor
 * 
 * Writes to the device driver so that the motor
 * is disabled.
 *
 * @return int: Returns 0 on error, or 1 if sucessful
 */
int stopMotor();

/**
 * @brief Get the Motor Status object
 * 
 * @return bool: Returns the status of the motor
 */
_Bool getMotorStatus();

#endif