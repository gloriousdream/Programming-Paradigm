# 🏰 部落冲突 (Clash of Clans) - 程序设计范式期末项目

## 一. 项目简介

**项目名称**：部落冲突 (Clash of Clans)

本项目以《部落冲突》这款游戏为原版，基于 **Cocos2d-x 4.0** 引擎开发出的 2D 版本。

游戏实现了“本部造兵、他方打仗”的基本玩法和功能。设置了两个核心场景：**大本营**（经营）和**敌方阵营**（战斗）。为了增加可玩性，战斗场景设有三个难度等级（简单、普通、困难）。这是一个典型的经营塔防类游戏项目。

---

## 二. 成员分工及贡献

| 姓名 | 分工描述 | 贡献度 |
| :--- | :--- | :--- |
| **韩昊苏** | **主要负责主场景建设，建筑类的设计**。<br>包括：建筑血条、建筑战斗动画、血条响应等等。<br>具体见 Git 提交记录。 | 0.5 |
| **崔少坤** | **主要负责战斗场景设计，士兵设计**。<br>包括：寻路算法、走路动画效果、战斗逻辑等。<br>具体见 Git 提交记录。 | 0.5 |

---

## 三. 项目组成与架构

1.  **场景模块**:
    * `StartBackground`: 游戏入口
    * `GameScene`: 主城经营场景
    * `FightScene`: 战斗/回放场景
2.  **实体模块**:
    * **Soldier**: 基类及 4 种衍生兵种 (`Archer`, `Barbarian`, `Giant`, `Bomber`)
    * **Building**: 基类及 9 种衍生建筑 (`TownHall`, `Cannon`, `Resource collectors` 等)
3.  **管理模块**:
    * `SoldierManager`, `BuildingManager` (单例模式，负责工厂创建与全局管理)
4.  **UI 模块**:
    * `BuildMenu` (建造菜单)
    * `Soldiermenu` (造兵菜单)

---

## 四. 基本要求完成情况 (C++ 技术实现)

### 1. STL 容器的使用
* **(1) std::vector 在路径点存储中的使用**
    * **位置**: `Soldier.h`
    * **代码**:
        ```cpp
        std::vector<cocos2d::Vec2> _pathPoints; // 存储寻路算法生成的路径节点
        ```
* **(2) std::vector 在回放系统数据结构中的使用**
    * **位置**: `FightScene.h`
    * **代码**:
        ```cpp
        std::vector<ReplayActionData> actions; // 存储录制的玩家操作序列
        ```

### 2. 迭代器
* **(1) 基于范围的 for 循环 (隐式迭代器)**
    * **位置**: `SoldierManager.cpp`
    * **代码**:
        ```cpp
        for (auto soldier : _soldiers) { ... } // 遍历所有存活士兵进行状态更新
        ```
* **(2) 标准迭代器遍历 UI 列表**
    * **位置**: `BuildMenu.cpp`
    * **代码**:
        ```cpp
        for (const auto& info : buildings) { ... } // 遍历建筑配置结构体以生成图标
        ```

### 3. 类与多态
* **(1) 继承关系**
    * **位置**: `Archer.h`
    * **代码**:
        ```cpp
        class Archer : public Soldier { ... }; // 弓箭手继承自士兵基类
        ```
* **(2) 纯虚函数与多态实现**
    * **位置**: `Soldier.h` (基类) 定义 `virtual void actionAttack() = 0;`
    * **位置**: `Archer.cpp` (子类) 实现具体的远程攻击逻辑：
        ```cpp
        void Archer::actionAttack() { ... }
        ```

### 4. 模板
* **(1) Cocos2d 模板容器 Vector<T>**
    * **位置**: `SoldierManager.h`
    * **代码**:
        ```cpp
        cocos2d::Vector<Soldier*> _soldiers; // 使用模板容器管理对象生命周期
        ```

### 5. 异常处理
* **(1) 抛出异常**
    * **位置**: `GameScene.cpp`
    * **代码**:
        ```cpp
        if (area.size.width <= 0) throw std::runtime_error("Invalid spawn area...");
        ```
* **(2) 捕获异常**
    * **位置**: `GameScene.cpp` (spawnHomeSoldier函数内)
    * **代码**:
        ```cpp
        try { ... } catch (const std::exception& e) { CCLOG("Error: %s", e.what()); }
        ```

### 6. C++11 或以上功能
* **(1) Lambda 表达式**
    * **位置**: `Soldiermenu.cpp`
    * **代码**:
        ```cpp
        btnTrain->setCallback([=](Ref*){ ... }); // 捕获上下文处理按钮点击
        ```
* **(2) auto 类型推导**
    * **位置**: `GameScene.cpp`
    * **代码**:
        ```cpp
        auto visibleSize = Director::getInstance()->getVisibleSize();
        ```

### 7. 代码格式统一
* **说明**: 项目整体采用 4 空格缩进，大括号换行风格 (Allman/BSD)，类成员变量统一使用下划线前缀 (如 `_soldiers`, `_curMoveDir`)，保持了高度的一致性。

### 8. Google C++ Style
* **说明**:
    * 头文件使用 `#ifndef/#define/#endif` 护盾 (如 `__SOLDIER_H__`)。
    * 包含头文件顺序规范 (先库文件后本地文件)。
    * 避免使用全局变量，通过 Singleton 模式 (`SoldierManager`) 管理全局状态。

### 9. C++ 风格类型转换
* **(1) static_cast 进行数值转换**
    * **位置**: `GoldStage.cpp`
    * **代码**:
        ```cpp
        float percent = static_cast<float>(currentGold) / static_cast<float>(maxGold);
        ```
* **(2) static_cast 计算圣水比例**
    * **位置**: `ElixirTank.cpp`
    * **代码**:
        ```cpp
        float percent = static_cast<float>(currentHoly) / static_cast<float>(maxHoly);
        ```

### 10. Const 的合理使用
* **(1) Const 成员函数 (保证不修改成员变量)**
    * **位置**: `Building.h`
    * **代码**:
        ```cpp
        int getHP() const { return currentHP; }
        ```
* **(2) Const 引用传参 (避免拷贝)**
    * **位置**: `FightScene.cpp` (createReplayScene)
    * **代码**:
        ```cpp
        static FightScene* createReplayScene(const ReplayData& data);
        ```

### 11. 注释规范
* **说明**:
    * 关键算法 (`Cannon.cpp` 的攻击逻辑，A*寻路算法) 有详细步骤注释。
    * 复杂结构体 (`FightScene.h` 的 `ReplayData`) 有字段含义注释。
    * 函数声明 (`SoldierManager.h`) 有功能性注释 (如 `// 清理已经死亡或被移除的士兵`)。

---

## 五. 扩展功能与特性

### 1. 扩展玩法功能

* **2.1 回放系统**
    * **位置**: `FightScene.h` / `FightScene.cpp`
    * **核心代码**:
        ```cpp
        struct ReplayData { unsigned int seed; std::vector<ReplayActionData> actions; };
        FightScene::createReplayScene(const ReplayData& data) { ... } // 重注入种子和操作
        ```
    * **简述**: 通过记录随机数种子(seed)和操作时间戳，以极小的数据量完整复现了整场战斗过程。

* **2.2 建筑施工时间（加速水晶）**
    * **位置**: `Building.cpp`
    * **核心代码**:
        ```cpp
        void updateUpgradeTimer(float dt) { _remainingTime -= dt; ... }
        void skipUpgradeTimer() { onUpgradeFinished(0); } // 立即完成
        ```
    * **简述**: 实现了模拟经营游戏核心的时间管理机制，支持倒计时等待和立即加速完成。

* **2.3 陷阱炸弹系统**
    * **位置**: `Boom.cpp`
    * **核心代码**:
        ```cpp
        void setToFightMode() { this->setVisible(false); } // 战斗开始时隐形
        if (dist < _triggerRange) playExplodeEffect(); // 敌人进入范围触发
        ```
    * **简述**: 实现了“隐形-触发-显形爆炸”的完整逻辑，增加了战斗的策略深度。

* **2.4 丰富的天气，昼夜交替**
    * **位置**: `GameScene.cpp`
    * **核心代码**:
        ```cpp
        void update(float dt) { _dayNightTimer += dt; _nightLayer->setOpacity(...); }
        ```
    * **简述**: 通过 update 里的时间累积动态调整黑色遮罩层的透明度，模拟昼夜光照变化。

* **2.5 不同的战斗难度**
    * **位置**: `FightScene.cpp`
    * **核心代码**:
        ```cpp
        static FightScene* create(int difficulty); // 工厂方法传入难度参数
        ```
    * **简述**: 在创建场景时接受难度参数，可据此调整敌人生成的频率或属性。

* **2.6 存档功能**
    * **位置**: `GameScene.cpp`
    * **核心代码**:
        ```cpp
        UserDefault::getInstance()->setIntegerForKey("GOLD", gold);
        ```
    * **简述**: 利用 UserDefault 实现本地数据持久化，保证玩家资源数据不丢失。

* **2.7 战斗奖励，实时 UI 变化**
    * **位置**: `GoldStage.cpp` / `GameScene.cpp`
    * **核心代码**:
        ```cpp
        EventCustom event("LOOT_GOLD_EVENT"); _eventDispatcher->dispatchEvent(&event);
        ```
    * **简述**: 采用观察者模式，建筑受伤时分发掉落事件，UI 层监听事件实时更新资源数字。

### 2. 加分项：版本控制与协作

#### 2.1 Github 使用规范
* **提交**: 遵循标准工作流 `git pull origin main` 同步 -> `git add .` -> `git commit -m "描述"` -> `git push`。
* **分支策略**: 采用主分支开发，经测试无误后通过 Pull 合并，有效沟通避免了代码冲突与环境污染。

#### 2.2 合理分工
* **面向接口的模块化分工**: 将核心业务逻辑解耦为“建筑系统”与“士兵系统”两大模块。成员各自独立负责模块内部的具体实现，通过预先定义好的标准接口（Interface）进行交互。
* **依赖驱动的开发流程**: 遵循符合工程逻辑的线性开发路线。优先构建作为数据基础与经济核心的 `GameScene`，在确保资源产出、建筑生成等底层逻辑稳定后，再基于此开发负责战斗演算的 `FightScene`。

### 3. 加分项：代码质量与稳定性

#### 3.1 合理错误处理
* **(1) 关键逻辑的异常捕获 (Try-Catch)**
    * **代码位置**: `GameScene.cpp` (spawnHomeSoldier 函数)
    * **描述**: 在涉及游戏核心逻辑（如生成单位）时，使用了 C++ 标准异常处理机制。
    * **代码**:
        ```cpp
        try {
            Rect area = BuildingManager::getInstance()->getSoldierSpawnArea();
            if (area.size.width <= 0) { // 防御性编程：检查区域是否合法
                throw std::runtime_error("Invalid spawn area from BuildingManager");
            }
            ...
        } catch (const std::exception& e) {
            CCLOG("Error spawning soldier: %s", e.what()); // 捕获异常并打印日志
        }
        ```
* **(2) 资源加载的容错处理**
    * **代码位置**: `Archer.cpp` / `Barbarian.cpp`
    * **描述**: 自动降级查找逻辑，确保即使资源命名有细微差别（如 "01.png" vs "_01.png"）也能正常加载。
* **(3) 工厂方法的空值检查**
    * **代码位置**: `SoldierManager.cpp`
    * **描述**: 在工厂模式创建对象后，立即检查返回值是否为 `nullptr`。

#### 3.2 无内存泄漏 (Memory Leak Prevention)
* **(1) 智能容器与自动引用计数**
    * **描述**: 全面采用了 `cocos2d::Vector<T>` 容器，自动执行 `retain()`/`release()`，杜绝悬空指针。
* **(2) 异步回调中的手动内存管理**
    * **代码位置**: `Cannon.cpp` (fireAt 函数)
    * **描述**: 在 Lambda 中捕获 target 时，显式管理引用计数，防止目标在炮弹飞行过程中死亡导致野指针。
    * **代码**:
        ```cpp
        auto hitCallback = CallFunc::create([target, ball, damage]() {
            // 安全检查：确保 target 逻辑上存活
            if (target && target->getHP() > 0 && target->getParent()) {
                target->takeDamage(damage);
            }
            ball->removeFromParent();
            target->release(); // 对应外部的 retain()
        });
        ```
* **(3) 对象的自我销毁机制**
    * **描述**: 对于一次性对象，使用 `RemoveSelf` 动作。

### 4. 开发特性 (C++ 高级特性与架构)

#### 4.1 C++ 特性使用丰富
* **Lambda 表达式**: 用于 UI 按钮回调、动作序列。
* **auto 类型推导**: 减少代码冗余。
* **std::function 与回调机制**: 替代传统函数指针，解耦 UI 与逻辑。
* **override 关键字**: 显式标记虚函数，防止多态失效。
* **enum class**: 强类型枚举，提升类型安全。
* **std::to_string**: 类型安全的字符串转换。
* **nullptr**: 全面替代 NULL。

#### 4.2 架构设计优雅
* **MVC 变体结构**:
    * **Data**: Manager 类维护全局数据。
    * **View**: Scene 和 Layer 专注渲染。
    * **Logic**: Entity 类封装状态机。
* **工厂模式 (Factory Pattern)**: `SoldierManager::createSoldier` 和 `BuildingManager::createBuilding`，新增类型无需修改调用处。
* **观察者模式 (Observer Pattern)**: 利用 `EventDispatcher` 实现模块间松耦合。

#### 4.3 目录结构清晰
* **Classes**: 分层清晰 (Building, Soldier, Scene)。
* **Resources**: 素材分类存放。

### 5. 界面体验
* **界面精美**: 实现了 `getNextLevelTextureName` 接口，建筑外观随等级提升自动焕新；`updateVisuals` 动态渲染圣水罐液面升降。
* **不卡顿不崩溃**: 深度优化核心逻辑与内存管理，经过多轮压力测试，杜绝闪退。
* **流畅的动画**: 使用 `SpriteFrameCache` 预加载图集，配合状态机逻辑实现动作平滑过渡。

---

## 六. 超级加分：Android 平台支持

* **核心依据**:
    * 代码完全基于 **Cocos2d-x** 跨平台引擎编写。
    * 使用了 `EventListenerTouchOneByOne` (通用触摸事件) 而非鼠标事件，完美适配手机触摸屏。
    * 使用了 `UserDefault` 和 `AudioEngine` 等标准跨平台接口，无 Windows 专用 API 依赖 (如 windows.h)，可直接编译为 APK。

---

## 七. 复刻核心玩法

1.  **自由建造与经营**: 基于 BuildingManager 的网格吸附与碰撞检测算法，支持资源生产、存储及大本营升级。
2.  **策略塔防战斗**: 基于 A* 或方向向量的自动寻路 AI。
    * **巨人**: 高血量。
    * **炸弹人**: 自爆攻击。
    * **弓箭手**: 远程攻击。
    * **防御塔**: 自动索敌与冷却。
3.  **完整的游戏循环**: 生产 -> 收集 -> 造兵 -> 战斗 -> 掠夺 -> 回放。
4.  **细节打磨**: 昼夜循环、陷阱机制、完整的音效与动画状态机。

---

## 八. Git 规范提交记录
*(此处保留 Git 记录截图或链接)*
