/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <libgen.h>
#include <signal.h>
#include <GLES3/gl3.h>
#include "drm_egl_context.h"
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
    LAppDelegate* s_instance = NULL;
    volatile sig_atomic_t g_running = 1;

    void signalHandler(int) { g_running = 0; }
}

LAppDelegate* LAppDelegate::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppDelegate();
    }
    return s_instance;
}

void LAppDelegate::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }
    s_instance = NULL;
}

bool LAppDelegate::Initialize()
{
    if (DebugLogEnable)
    {
        LAppPal::PrintLogLn("START (GBM/DRM direct)");
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    _drmContext = new rive_rk3566::DRMEGLContext();
    if (!_drmContext->initialize())
    {
        LAppPal::PrintLogLn("Failed to init DRM/EGL: %s", _drmContext->lastError().c_str());
        return false;
    }

    _windowWidth = 500;
    _windowHeight = 500;
    _displayWidth = _drmContext->width();
    _displayHeight = _drmContext->height();
    LAppPal::PrintLogLn("Native display: %dx%d, render viewport: %dx%d",
        _displayWidth, _displayHeight, _windowWidth, _windowHeight);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, _windowWidth, _windowHeight);

    InitializeCubism();
    SetExecuteAbsolutePath();

    LAppLive2DManager::GetInstance();

    _view->Initialize(_windowWidth, _windowHeight);
    _view->InitializeSprite();

    return true;
}

void LAppDelegate::Release()
{
    delete _textureManager;
    delete _view;

    LAppLive2DManager::ReleaseInstance();
    CubismFramework::Dispose();

    if (_drmContext)
    {
        delete _drmContext;
        _drmContext = nullptr;
    }
}

void LAppDelegate::Run()
{
    int frameCount = 0;
    double fpsTimer = LAppPal::GetCurrentTimeSeconds();
    double totalFrameTime = 0.0;
    double minFrameTime = 1e9;
    double maxFrameTime = 0.0;

    double totalClear = 0, totalCpu = 0, totalSwap = 0;

    while (g_running && !_isEnd)
    {
        double t0 = LAppPal::GetCurrentTimeSeconds();

        // Clear full screen
        glViewport(0, 0, _displayWidth, _displayHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render Live2D in centered viewport
        int offsetX = (_displayWidth - _windowWidth) / 2;
        int offsetY = (_displayHeight - _windowHeight) / 2;
        glViewport(offsetX, offsetY, _windowWidth, _windowHeight);

        double t1 = LAppPal::GetCurrentTimeSeconds();

        LAppPal::UpdateTime();
        _view->Render();

        double t2 = LAppPal::GetCurrentTimeSeconds();

        _drmContext->swapBuffers();

        double t3 = LAppPal::GetCurrentTimeSeconds();

        double clearMs = (t1 - t0) * 1000.0;
        double cpuMs = (t2 - t1) * 1000.0;
        double swapMs = (t3 - t2) * 1000.0;
        double frameTime = (t3 - t0) * 1000.0;

        totalClear += clearMs;
        totalCpu += cpuMs;
        totalSwap += swapMs;
        totalFrameTime += frameTime;
        if (frameTime < minFrameTime) minFrameTime = frameTime;
        if (frameTime > maxFrameTime) maxFrameTime = frameTime;
        frameCount++;

        double elapsed = t3 - fpsTimer;
        if (elapsed >= 1.0)
        {
            double avgFps = frameCount / elapsed;
            LAppPal::PrintLogLn("[PERF] FPS: %.1f | clear: %.2f | cpu: %.2f | swap: %.2f | total: %.2f ms | frames: %d",
                avgFps, totalClear / frameCount, totalCpu / frameCount,
                totalSwap / frameCount, totalFrameTime / frameCount, frameCount);
            frameCount = 0;
            totalFrameTime = 0.0;
            totalClear = totalCpu = totalSwap = 0.0;
            minFrameTime = 1e9;
            maxFrameTime = 0.0;
            fpsTimer = t3;
        }
    }

    Release();
    LAppDelegate::ReleaseInstance();
}

LAppDelegate::LAppDelegate():
    _cubismOption(),
    _drmContext(nullptr),
    _captured(false),
    _mouseX(0.0f),
    _mouseY(0.0f),
    _isEnd(false),
    _windowWidth(0),
    _windowHeight(0),
    _displayWidth(0),
    _displayHeight(0)
{
    _executeAbsolutePath = "";
    _view = new LAppView();
    _textureManager = new LAppTextureManager();
}

LAppDelegate::~LAppDelegate()
{
}

void LAppDelegate::InitializeCubism()
{
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    _cubismOption.LoadFileFunction = LAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = LAppPal::ReleaseBytes;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);
    CubismFramework::Initialize();

    CubismMatrix44 projection;
    LAppPal::UpdateTime();
}

void LAppDelegate::GetClientSize(int& rWidth, int& rHeight)
{
    rWidth = GetInstance()->_windowWidth;
    rHeight = GetInstance()->_windowHeight;
}

void LAppDelegate::SetExecuteAbsolutePath()
{
    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, 1024 - 1);
    if (len != -1)
    {
        path[len] = '\0';
    }
    this->_executeAbsolutePath = dirname(path);
    this->_executeAbsolutePath += "/";
}
