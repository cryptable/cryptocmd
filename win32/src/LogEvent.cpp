/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 17/08/2020
 */

#include "LogEvent.h"
#include "common.h"
#include "KSException.h"
#include "event_codes.h"

#define LOG_ALL         0
#define LOG_TRACE       1
#define LOG_DEBUG       2
#define LOG_INFO        3
#define LOG_WARNING     4
#define LOG_ERROR       5
#define LOG_FATAL       6
#define LOG_NOTHING     10

LogEvent LogEvent::logEvent;

LogEvent::LogEvent(): logLevel{LOG_ERROR}, auditEnabled(true) {
}

void LogEvent::trace(int eventCode, std::string &l) {
    log(LOG_TRACE, l, eventCode);
}

void LogEvent::debug(int eventCode, std::string &l) {
    log(LOG_DEBUG, l, eventCode);
}

void LogEvent::info(int eventCode, std::string &l) {
    log(LOG_INFO, l, eventCode);
}

void LogEvent::warning(int eventCode, std::string &l) {
    log(LOG_WARNING, l, eventCode);
}

void LogEvent::error(int eventCode, std::string &l) {
    log(LOG_ERROR, l, eventCode);
}

void LogEvent::fatal(int eventCode, std::string &l) {
    log(LOG_FATAL, l, eventCode);
}

void LogEvent::log(int level, std::string &log, int eventCode) {
    if (level < logLevel)
        return;

    std::string logLine = "";
    WORD type = 0;
    DWORD eventId = (KEYMANAGMENT_FACILITY << 16) + eventCode;
    switch (level) {
        case LOG_TRACE:
            type = EVENTLOG_INFORMATION_TYPE;
            logLine = "TRACE: " + log;
            eventId |= 0x60000000;
            break;
        case LOG_DEBUG:
            type = EVENTLOG_INFORMATION_TYPE;
            logLine = "DEBUG: " + log;
            eventId |= 0x60000000;
            break;
        case LOG_INFO:
            type = EVENTLOG_INFORMATION_TYPE;
            logLine = "INFO: " + log;
            eventId |= 0x60000000;
            break;
        case LOG_WARNING:
            type = EVENTLOG_WARNING_TYPE;
            logLine = "WARN: " + log;
            eventId |= 0xA0000000;
            break;
        case LOG_ERROR:
            type = EVENTLOG_ERROR_TYPE;
            logLine = "ERROR: " + log;
            eventId |= 0xE0000000;
            break;
        default: // LOG_FATAL
            type = EVENTLOG_ERROR_TYPE;
            logLine = "FATAL: " + log;
            eventId |= 0xE0000000;
            break;
    }

    LPCTSTR message = log.c_str();
    eventSource = RegisterEventSource(NULL, APP_NAME);
    if (eventSource == NULL) {
        throw KSException(__func__, __LINE__, GetLastError());
    }
    if (FALSE == ReportEventA(eventSource,
                              type,
                              0,
                              eventId,
                              NULL,
                              1,
                              0,
                              &message,
                              NULL)) {
        DeregisterEventSource(eventSource);
        throw KSException(__func__, __LINE__, GetLastError());
    }
    DeregisterEventSource(eventSource);
}

void LogEvent::auditFailure(int eventCode, std::string &log) {
    audit(EVENTLOG_AUDIT_FAILURE, eventCode, log);
}

void LogEvent::auditSuccess(int eventCode, std::string &log) {
    audit(EVENTLOG_AUDIT_SUCCESS, eventCode, log);
}

void LogEvent::audit(int eventType, int eventCode, std::string &log) {

    if (auditEnabled) {
        LPCTSTR message = log.c_str();
        eventSource = RegisterEventSource(NULL, APP_NAME);
        DWORD eventId = (KEYMANAGMENT_FACILITY << 16) + eventCode;
        if (eventSource == NULL) {
            throw KSException(__func__, __LINE__, GetLastError());
        }
        if (FALSE == ReportEventA(eventSource,
                                  EVENTLOG_AUDIT_FAILURE,
                                  0,
                                  eventId,
                                  NULL,
                                  1,
                                  0,
                                  &message,
                                  NULL)) {
            DeregisterEventSource(eventSource);
            throw KSException(__func__, __LINE__, GetLastError());
        }
        DeregisterEventSource(eventSource);
    }
}

LogEvent & LogEvent::GetInstance() {
    return logEvent;
}

LogEvent::~LogEvent() {
}