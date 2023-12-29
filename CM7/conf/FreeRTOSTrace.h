/**
 ******************************************************************************
 * @file           FreeRTOSTrace.h
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2023
 * @brief          FreeRTOS Trace configuration file
 ******************************************************************************
 * @attention
 * Copyright (c) 2023 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

// clang-format off

#ifndef FREERTOS_TRACE_H
#define FREERTOS_TRACE_H

#ifdef FREERTOS_USE_TRACE

#include <stddef.h>
#include <stdint.h>

extern void carbon_freertos_trace_malloc(void *address, size_t size);
extern void carbon_freertos_trace_free(void *address, size_t size);
extern void carbon_freertos_trace_switched_in(uint32_t number);
extern void carbon_freertos_trace_switched_out(uint32_t number);

#undef traceMALLOC
#define traceMALLOC(pvAddress, uiSize) \
	carbon_freertos_trace_malloc(pvAddress, uiSize)

#undef traceFREE
#define traceFREE(pvAddress, uiSize) \
	carbon_freertos_trace_free(pvAddress, uiSize)

#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
	carbon_freertos_trace_switched_in(pxCurrentTCB->uxTCBNumber)

#undef traceTASK_SWITCHED_OUT
#define traceTASK_SWITCHED_OUT() \
	carbon_freertos_trace_switched_out(pxCurrentTCB->uxTCBNumber)

// #define traceSTART()
// #define traceEND()
// #define traceBLOCKING_ON_QUEUE_PEEK(pxQueue)
// #define traceBLOCKING_ON_QUEUE_RECEIVE(pxQueue)
// #define traceBLOCKING_ON_QUEUE_SEND(pxQueue)
// #define traceBLOCKING_ON_STREAM_BUFFER_RECEIVE(xStreamBuffer)
// #define traceBLOCKING_ON_STREAM_BUFFER_SEND(xStreamBuffer)
// #define traceCREATE_COUNTING_SEMAPHORE()
// #define traceCREATE_COUNTING_SEMAPHORE_FAILED()
// #define traceCREATE_MUTEX(pxNewQueue)
// #define traceCREATE_MUTEX_FAILED()
// #define traceEVENT_GROUP_CLEAR_BITS(xEventGroup, uxBitsToClear)
// #define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(xEventGroup, uxBitsToClear)
// #define traceEVENT_GROUP_CREATE(xEventGroup)
// #define traceEVENT_GROUP_CREATE_FAILED()
// #define traceEVENT_GROUP_DELETE(xEventGroup)
// #define traceEVENT_GROUP_SET_BITS(xEventGroup, uxBitsToSet)
// #define traceEVENT_GROUP_SET_BITS_FROM_ISR(xEventGroup, uxBitsToSet)
// #define traceEVENT_GROUP_SYNC_BLOCK(xEventGroup, uxBitsToSet, uxBitsToWaitFor)
// #define traceEVENT_GROUP_SYNC_END(xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred)
// #define traceEVENT_GROUP_WAIT_BITS_BLOCK(xEventGroup, uxBitsToWaitFor)
// #define traceEVENT_GROUP_WAIT_BITS_END(xEventGroup, uxBitsToWaitFor, xTimeoutOccurred)
// #define traceGIVE_MUTEX_RECURSIVE(pxMutex)
// #define traceGIVE_MUTEX_RECURSIVE_FAILED(pxMutex)
// #define traceINCREASE_TICK_COUNT(x)
// #define traceLOW_POWER_IDLE_BEGIN()
// #define traceLOW_POWER_IDLE_END()
// #define traceMOVED_TASK_TO_READY_STATE(pxTCB)
// #define tracePEND_FUNC_CALL(xFunctionToPend, pvParameter1, ulParameter2, ret)
// #define tracePEND_FUNC_CALL_FROM_ISR(xFunctionToPend, pvParameter1, ulParameter2, ret)
// #define tracePOST_MOVED_TASK_TO_READY_STATE(pxTCB)
// #define traceQUEUE_CREATE(pxNewQueue)
// #define traceQUEUE_CREATE_FAILED(ucQueueType)
// #define traceQUEUE_DELETE(pxQueue)
// #define traceQUEUE_PEEK(pxQueue)
// #define traceQUEUE_PEEK_FAILED(pxQueue)
// #define traceQUEUE_PEEK_FROM_ISR(pxQueue)
// #define traceQUEUE_PEEK_FROM_ISR_FAILED(pxQueue)
// #define traceQUEUE_RECEIVE(pxQueue)
// #define traceQUEUE_RECEIVE_FAILED(pxQueue)
// #define traceQUEUE_RECEIVE_FROM_ISR(pxQueue)
// #define traceQUEUE_RECEIVE_FROM_ISR_FAILED(pxQueue)
// #define traceQUEUE_REGISTRY_ADD(xQueue, pcQueueName)
// #define traceQUEUE_SEND(pxQueue)
// #define traceQUEUE_SEND_FAILED(pxQueue)
// #define traceQUEUE_SEND_FROM_ISR(pxQueue)
// #define traceQUEUE_SEND_FROM_ISR_FAILED(pxQueue)
// #define traceSTREAM_BUFFER_CREATE(pxStreamBuffer, xIsMessageBuffer)
// #define traceSTREAM_BUFFER_CREATE_FAILED(xIsMessageBuffer)
// #define traceSTREAM_BUFFER_CREATE_STATIC_FAILED(xReturn, xIsMessageBuffer)
// #define traceSTREAM_BUFFER_DELETE(xStreamBuffer)
// #define traceSTREAM_BUFFER_RECEIVE(xStreamBuffer, xReceivedLength)
// #define traceSTREAM_BUFFER_RECEIVE_FAILED(xStreamBuffer)
// #define traceSTREAM_BUFFER_RECEIVE_FROM_ISR(xStreamBuffer, xReceivedLength)
// #define traceSTREAM_BUFFER_RESET(xStreamBuffer)
// #define traceSTREAM_BUFFER_SEND(xStreamBuffer, xBytesSent)
// #define traceSTREAM_BUFFER_SEND_FAILED(xStreamBuffer)
// #define traceSTREAM_BUFFER_SEND_FROM_ISR(xStreamBuffer, xBytesSent)
// #define traceTAKE_MUTEX_RECURSIVE(pxMutex)
// #define traceTAKE_MUTEX_RECURSIVE_FAILED(pxMutex)
// #define traceTASK_CREATE(pxNewTCB)
// #define traceTASK_CREATE_FAILED()
// #define traceTASK_DELAY()
// #define traceTASK_DELAY_UNTIL(x)
// #define traceTASK_DELETE(pxTaskToDelete)
// #define traceTASK_INCREMENT_TICK(xTickCount)
// #define traceTASK_NOTIFY()
// #define traceTASK_NOTIFY_FROM_ISR()
// #define traceTASK_NOTIFY_GIVE_FROM_ISR()
// #define traceTASK_NOTIFY_TAKE()
// #define traceTASK_NOTIFY_TAKE_BLOCK()
// #define traceTASK_NOTIFY_WAIT()
// #define traceTASK_NOTIFY_WAIT_BLOCK()
// #define traceTASK_PRIORITY_DISINHERIT(pxTCBOfMutexHolder, uxOriginalPriority)
// #define traceTASK_PRIORITY_INHERIT(pxTCBOfMutexHolder, uxInheritedPriority)
// #define traceTASK_PRIORITY_SET(pxTask, uxNewPriority)
// #define traceTASK_RESUME(pxTaskToResume)
// #define traceTASK_RESUME_FROM_ISR(pxTaskToResume)
// #define traceTASK_SUSPEND(pxTaskToSuspend)
// #define traceTIMER_COMMAND_RECEIVED(pxTimer, xMessageID, xMessageValue)
// #define traceTIMER_COMMAND_SEND(xTimer, xMessageID, xMessageValueValue, xReturn)
// #define traceTIMER_CREATE(pxNewTimer)
// #define traceTIMER_CREATE_FAILED()
// #define traceTIMER_EXPIRED(pxTimer)

#endif // defined(FREERTOS_USE_TRACE)

#endif // FREERTOS_TRACE_H
