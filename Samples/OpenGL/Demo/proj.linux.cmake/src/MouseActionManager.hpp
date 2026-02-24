/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GLES3/gl3.h>

#include <CubismFramework.hpp>

#include "MouseActionManager_Common.hpp"

/**
* @brief マウスの動作を通知するクラス
*
* マウス操作等を Cubism へ橋渡しする。
*
*/
class MouseActionManager : public MouseActionManager_Common
{
public:
  /**
  * @brief   クラスのインスタンス（シングルトン）を返す
  *
  * インスタンスが生成されていない場合は内部でインスタンスを生成する
  *
  * @return  クラスのインスタンス
  */
  static MouseActionManager* GetInstance();

  /**
  * @brief   クラスのインスタンス（シングルトン）を解放する
  *
  */
  static void ReleaseInstance();

  MouseActionManager(); ///< コンストラクタ
  virtual ~MouseActionManager(); ///< デストラクタ

  /**
  * @brief   マウスボタンコールバック用関数。
  *
  * @param[in]       button            ボタン種類
  * @param[in]       action            実行結果
  * @param[in]       modify
  */
  void OnMouseCallBack(int button, int action, int modify);

  /**
  * @brief   カーソル位置コールバック用関数。
  *
  * @param[in]       x                 x座標
  * @param[in]       y                 y座標
  */
  void OnMouseCallBack(double x, double y);
};

class EventHandler
{
public:
    /**
    * @brief   マウスボタンコールバック関数。
    */
    static void OnMouseCallBack(int button, int action, int modify)
    {
        MouseActionManager::GetInstance()->OnMouseCallBack(button, action, modify);
    }

    /**
    * @brief   カーソル位置コールバック関数。
    */
    static void OnMouseCallBack(double x, double y)
    {
        MouseActionManager::GetInstance()->OnMouseCallBack(x, y);
    }
};
