/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <string>
#include <GLES3/gl3.h>
#include "Type/csmVector.hpp"
#include "LAppAllocator_Common.hpp"

class LAppView;
class LAppTextureManager;

namespace rive_rk3566 { class DRMEGLContext; }

class LAppDelegate
{
public:
    static LAppDelegate* GetInstance();
    static void ReleaseInstance();

    bool Initialize();
    void Release();
    void Run();

    static void GetClientSize(int& rWidth, int& rHeight);

    LAppView* GetView() { return _view; }
    bool GetIsEnd() { return _isEnd; }
    void AppEnd() { _isEnd = true; }

    void SetExecuteAbsolutePath();
    std::string GetExecuteAbsolutePath(){ return _executeAbsolutePath;}
    LAppTextureManager* GetTextureManager() { return _textureManager; }

    int GetWindowWidth() const { return _windowWidth; }
    int GetWindowHeight() const { return _windowHeight; }

private:
    LAppDelegate();
    ~LAppDelegate();
    void InitializeCubism();

    LAppAllocator_Common _cubismAllocator;
    Csm::CubismFramework::Option _cubismOption;
    rive_rk3566::DRMEGLContext* _drmContext;
    LAppView* _view;
    bool _captured;
    float _mouseX;
    float _mouseY;
    bool _isEnd;
    LAppTextureManager* _textureManager;
    std::string _executeAbsolutePath;

    int _windowWidth;
    int _windowHeight;
    int _displayWidth;
    int _displayHeight;
};
