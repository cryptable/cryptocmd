/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 17/08/2020
 */

#ifndef KSMGMNT_LOGEVENT_H
#define KSMGMNT_LOGEVENT_H
#include <windows.h>
#include <string>

class LogEvent {
public:
    static LogEvent &GetInstance();

    LogEvent(LogEvent const&)        = delete;

    void operator=(LogEvent const&)  = delete;

    void setLevel(int eventCode, const std::string &level);

    void trace(int eventCode, const std::string &log);

    void debug(int eventCode, const std::string &log);

    void info(int eventCode, const std::string &log);

    void warning(int eventCode, const std::string &log);

    void error(int eventCode, const std::string &log);

    void fatal(int eventCode, const std::string &log);

    void setAudit(bool enable);

    void auditFailure(int eventCode, const std::string &log);

    void auditSuccess(int eventCode, const std::string &log);

    ~LogEvent();

private:
    LogEvent();

    HANDLE eventSource;

    int logLevel;

    bool auditEnabled;

    void log(int level, const std::string &log, int eventCode);

    void audit(int eventType, int eventCode, const std::string &log);

    static LogEvent logEvent;
};


#endif //KSMGMNT_LOGEVENT_H
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2020 Cryptable BV
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/
